#include <avr/io.h>

// === GLOBAL VARIABLES (I2C-assigned) ===
volatile uint8_t FB4B = 0;

// === CLOCK SETUP ===
void setup_clock() {
    CCP = 0xD8;
    CLKCTRL.OSCHFCTRLA = 0b00010100;      // Set 16 MHz internal oscillator
    while (CLKCTRL.MCLKSTATUS & 0b00000001); // Wait for clock switch to complete
}

// === PIN SETUP ===
void setup_pins() {
     // Set PC2 and PC3 as outputs
     PORTC.DIRSET = (1 << 2) | (1 << 3);

     // Set PC2 and PC3 high
     PORTC.OUTSET = (1 << 2) | (1 << 3);

     // Set PD1–PD4 as outputs
     PORTD.DIRSET = 0b00011110;
}

// === MOTOR CONTROL FUNCTION ===
void drive_motor(uint8_t direction, uint8_t fwd_pin, uint8_t bwd_pin) {
    if (direction == 0b10) {
        PORTD.OUTSET = (1 << fwd_pin);    // Forward
        PORTD.OUTCLR = (1 << bwd_pin);
    } else if (direction == 0b01) {
        PORTD.OUTSET = (1 << bwd_pin);    // Backward
        PORTD.OUTCLR = (1 << fwd_pin);
    } else {
        PORTD.OUTCLR = (1 << fwd_pin) | (1 << bwd_pin);  // Stop
    }
}

// === MAIN LOOP ===
int main(void) {
    setup_clock();                        // Initialize system clock
    setup_pins();                         // Set up direction and PWM pins

    while (1) {
        uint8_t left_sm = (FB4B & 0b00001100) >> 2;   // Extract bits 3-2
        uint8_t right_sm = FB4B & 0b00000011;         // Extract bits 1-0

        drive_motor(left_sm, 1, 2);   // PD1 = FWD, PD2 = BWD (left)
        drive_motor(right_sm, 3, 4);  // PD3 = FWD, PD4 = BWD (right)
    }
}


