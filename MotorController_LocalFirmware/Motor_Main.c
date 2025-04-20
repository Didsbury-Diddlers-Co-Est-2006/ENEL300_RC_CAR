#include <xc.h>

//******************************************************************************

// avr128db48-bare-metal-twi-mplab Main Initialization

//******************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "peripherals/CLKCTRL/CLKCTRL.h"
#include "peripherals/TWI/TWI_client.h"
#include "TWI_blockData.h"

FUSES = {
	.WDTCFG = 0x00, // WDTCFG {PERIOD=OFF, WINDOW=OFF}
	.BODCFG = 0x06, // BODCFG {SLEEP=SAMPLE, ACTIVE=ENABLE, SAMPFREQ=128Hz, LVL=BODLEVEL0}
	.OSCCFG = 0x00, // OSCCFG {CLKSEL=OSCHF}
	.SYSCFG0 = 0xC8, // SYSCFG0 {EESAVE=CLEAR, RSTPINCFG=RST, CRCSEL=CRC16, CRCSRC=NOCRC}
	.SYSCFG1 = 0x0C, // SYSCFG1 {SUT=8MS, MVSYSCFG=DUAL}
	.CODESIZE = 0x00, // CODESIZE {CODESIZE=User range:  0x0 - 0xFF}
	.BOOTSIZE = 0x00, // BOOTSIZE {BOOTSIZE=User range:  0x0 - 0xFF}
};

LOCKBITS = 0x5CC5C55C; // {KEY=NOLOCK}

#define DATA_SIZE 16

#define RIGHT 1
#define LEFT 0

//******************************************************************************

// Didsbury Diddlers Proprietary Function Definitions

//******************************************************************************

// Fairly sure that his clock setup is handled in the library

//// === CLOCK SETUP ===
//void setup_clock() {
//    CCP = 0xD8;
//    CLKCTRL.OSCHFCTRLA = 0b00010100;      // Set 16 MHz internal oscillator
//    while (CLKCTRL.MCLKSTATUS & 0b00000001); // Wait for clock switch to complete
//}

// Lowkey might need to readdress the pin setup, I have to fucking clue what
// the library is doing rn.

// === PIN SETUP ===
void setup_pins() {
    PORTMUX.TCAROUTEA = 0b00000010;
    PORTC.DIRSET = 0b00001100;            // PC2 (TCB0), PC3 (TCB1) as PWM outputs
    PORTD.DIRSET = 0b00011110;            // PD1–PD4 as motor direction control outputs
}

// === PWM SETUP ===
void setup_pwm() {
    TCA0.SPLIT.CTRLESET = 0b00001100;
    TCA0.SPLIT.CTRLD = 0b00000001;
    TCA0.SPLIT.LPER = 0xff;
    TCA0.SPLIT.HPER = 0xff;
    
    TCA0.SPLIT.HCMP0 = 0x00; // for right pwm, PC3
    TCA0.SPLIT.LCMP2 = 0x00; // for left pwm, PC2
    
    TCA0.SPLIT.CTRLB = 0b00010100;
    
    TCA0.SPLIT.CTRLA = 0b00000001;
    
}

// === MOTOR CONTROL FUNCTION ===
void drive_motor(uint8_t sm, uint8_t side) {
    uint8_t sign = sm & 0x80;             // Bit 7 = direction
    uint8_t magnitude = sm & 0x7F;        // Bits 6–0 = speed

    if(side == RIGHT){
        if (sign) {
            PORTD.OUTSET = (1 << 4);    // Set backward pin high
            PORTD.OUTCLR = (1 << 3);    // Clear forward pin
        } 
        else {
            PORTD.OUTSET = (1 << 3);    // Set forward pin high
            PORTD.OUTCLR = (1 << 4);    // Clear backward pin
        }
        // set PWM duty 0-ff for right side
        TCA0.SPLIT.HCMP0 = magnitude << 1;
    }
    else if (side == LEFT){
        if (sign) {
            PORTD.OUTSET = (1 << 2);    // Set backward pin high
            PORTD.OUTCLR = (1 << 1);    // Clear forward pin
        } else {
            PORTD.OUTSET = (1 << 1);    // Set forward pin high
            PORTD.OUTCLR = (1 << 2);    // Clear backward pin
        }
        // set PWM duty 0-ff for right side
        TCA0.SPLIT.LCMP2 = magnitude << 1;
    }
}

//******************************************************************************

// Main

//******************************************************************************

// === MAIN LOOP ===
int main(void) {
    
    setup_pins();                         // Set up direction and PWM pins
    setup_pwm();                          // Configure PWM for TCB0 and TCB1
       
    //Setup CPU Clocks
    CLKCTRL_init();
    
    //Setup TWI I/O
    TWI_initPins();
    
    //Setup TWI Interface
    TWI_initClient(0x11);
    
    //Data to R/W to. (Must be volatile)
    volatile uint8_t data[DATA_SIZE];
    
    //Initialize Memory to 0x00
    for (uint8_t i = 0; i < DATA_SIZE; i++)
    {
        data[i] = 0x00;
    }
    
    //Attach R/W Buffers
    //Note: The buffers can be separated, or they can be the same memory, if desired.
    setupReadBuffer(&data[0], 16);
    setupWriteBuffer(&data[8], 8);
    
    //Assign ISRs to Block Memory Library
    TWI_assignByteWriteHandler(&_TWI_StoreByte);
    TWI_assignByteReadHandler(&_TWI_RequestByte);
    TWI_assignStopHandler(&_onTWIStop);
    
    //Enable Interrupts
    sei();

//******************************************************************************
    
  // Super Loop  

//******************************************************************************
    
    // Responsible for motor control from ECU I2C data. Sending left then right
    // motor data.
    
    while (1)
    {   
        drive_motor(data[8], LEFT);   // PD1 = FWD, PD2 = BWD (left)
        drive_motor(data[9], RIGHT);  // PD3 = FWD, PD4 = BWD (right)
    }
}
