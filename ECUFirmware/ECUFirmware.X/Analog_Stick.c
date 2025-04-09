#include <avr/io.h>

short result_right = 0;
short result_left = 0;
uint8_t result_8_bit = 0;

uint8_t bit_converter(short result) {
    result_8_bit = result >> 5;  // Placeholder
    return 1;
}

int main(void) {
    // Enable global interrupts.
    SREG = 0b10000000;

    // Set the ADC reference level to VDD.
    VREF.ADC0REF = 0b10000101;

    // Enable the ADC interrupt.
    ADC0.INTCTRL = 0b00000001;

    // Start by selecting PD2 (AIN7) as the ADC input.
    ADC0.MUXPOS = 0x02;  // 0x02 corresponds to PD2 (AIN7)

    // Select minimum clock divide.
    ADC0.CTRLC = 0x00;

    // Select single-ended mode, 12-bit resolution, and free-running mode.
    ADC0.CTRLA = 0b00000011;

    // Start conversion.
    ADC0.COMMAND = 0x01;

    // Set PORTD as input.
    PORTD.DIRCLR = 0b11111111;

    while (1) {
        // Check if ADC result is ready.
        if (ADC0.INTFLAGS & 0b00000001) {
            // Check which channel is selected
            if (ADC0.MUXPOS == 0x02) {   // PD2 (AIN7)
                result_right = ADC0.RES;  // Read ADC value for PD2
                ADC0.MUXPOS = 0x01;       // Switch to PD1 (AIN6)
            } else if (ADC0.MUXPOS == 0x01) {  // PD1 (AIN6)
                result_left = ADC0.RES;   // Read ADC value for PD1
                ADC0.MUXPOS = 0x02;       // Switch back to PD2 (AIN7)
            }

            // Clear interrupt flag
            ADC0.INTFLAGS = 0b00000001;

            // Placeholder call
            bit_converter(result_right);
        }
    }
}