#include <avr/io.h>

// === GLOBAL VARIABLES (I2C-assigned) ===
volatile uint8_t left_sm = 0;
volatile uint8_t right_sm = 0;

// === CLOCK SETUP ===
void setup_clock() {
    CCP = 0xD8;
    CLKCTRL.OSCHFCTRLA = 0b00010100;      // Set 16 MHz internal oscillator
    while (CLKCTRL.MCLKSTATUS & 0b00000001); // Wait for clock switch to complete
}

// === PIN SETUP ===
void setup_pins() {
    PORTC.DIRSET = 0b00001100;            // PC2 (TCB0), PC3 (TCB1) as PWM outputs
    PORTD.DIRSET = 0b00011110;            // PD1–PD4 as motor direction control outputs
}

// === PWM SETUP ===
void setup_pwm() {
    // TCB0: Left motor on PC2
    TCB0.CTRLA = 0b00000011;              // CLKSEL = CLKDIV1 (bit 0), ENABLE (bit 1)
    TCB0.CTRLB = 0b00000101;              // CNTMODE = PWM8
    TCB0.CCMP = 0;
    TCB0.CNT = 0;

    // TCB1: Right motor on PC3
    TCB1.CTRLA = 0b00000011;
    TCB1.CTRLB = 0b00000101;
    TCB1.CCMP = 0;
    TCB1.CNT = 0;
}

// === MOTOR CONTROL FUNCTION ===
void drive_motor(uint8_t sm, uint8_t fwd_pin, uint8_t bwd_pin, volatile TCB_t *tcb) {
    uint8_t sign = sm & 0x80;             // Bit 7 = direction
    uint8_t magnitude = sm & 0x7F;        // Bits 6–0 = speed

    uint16_t pwm = (uint16_t)magnitude * 515; // Scale 0–127 to 0–65535

    if (sign) {
        PORTD.OUTSET = (1 << bwd_pin);    // Set backward pin high
        PORTD.OUTCLR = (1 << fwd_pin);    // Clear forward pin
    } else {
        PORTD.OUTSET = (1 << fwd_pin);    // Set forward pin high
        PORTD.OUTCLR = (1 << bwd_pin);    // Clear backward pin
    }

    tcb->CCMP = pwm;                      // Update PWM duty cycle
}

// === MAIN LOOP ===
int main(void) {
    setup_clock();                        // Initialize system clock
    setup_pins();                         // Set up direction and PWM pins
    setup_pwm();                          // Configure PWM for TCB0 and TCB1

    while (1) {
        // `left_sm` and `right_sm` will be assigned externally by I2C library

        drive_motor(left_sm, 1, 2, &TCB0);   // PD1 = FWD, PD2 = BWD (left)
        drive_motor(right_sm, 3, 4, &TCB1);  // PD3 = FWD, PD4 = BWD (right)
    }
}
