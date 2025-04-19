#include <avr/io.h>
volatile uint16_t result_right = 0;
volatile uint16_t result_left = 0;
volatile uint8_t result_8_bit = 0;

#include <stdint.h>

uint8_t adc_to_signmag_custom(uint16_t adc_val) {
    uint8_t sign = 0;
    uint8_t magnitude = 0;

    if (adc_val >= 2071) {
        // Reverse direction (negative)
        sign = 0x80;
        // Scale to 0?127, where 2071 maps to 0 and 4095 maps to 127
        magnitude = (uint8_t)((float)(adc_val - 2071) * 127 / (4095 - 2071));
    }
    else if (adc_val <= 2039) {
        // Forward direction (positive)
        sign = 0x00;
        // Scale to 0?127, where 0 maps to 127 and 2039 maps to 0
        magnitude = (uint8_t)((float)(2039 - adc_val) * 127 / 2039);
    }
    else {
        // Deadzone ? output zero
        return 0x00;
    }

    // Ensure magnitude is within the 0?127 range
    if (magnitude > 127) magnitude = 127;

    return sign | magnitude;
}
/*uint8_t adc_to_signmag_custom(uint16_t adc_val) {
    uint8_t result = 0b00000000; 
    if(adc_val >= 2071){
        result = 0b00000001;
    }else if(adc_val <= 2039){
        result = 0b00000010;
    }else{
        result = 0b00000000;
    }
    return result;
}*/

int main(void) {
    // Enable global interrupts.
    SREG = 0b10000000;

    // Set the ADC reference level to VDD.
    VREF.ADC0REF = 0b10000101;

    // Enable the ADC interrupt.
    ADC0.INTCTRL = 0b00000001;

    // Start by selecting PD2 (AIN7) as the ADC input.
    ADC0.MUXPOS = 0x03;  // 0x02 corresponds to PD2 (AIN7)

    // Select minimum clock divide.
    ADC0.CTRLC = 0x00;

    // Select single-ended mode, 12-bit resolution, and free-running mode.
    ADC0.CTRLA = 0b00000011;

    // Start conversion.
    ADC0.COMMAND = 0x01;

    // Set PORTD as input.
    PORTD.DIRCLR = 0b11111111;
    
    PORTA.DIRSET = 0b11111111;
    
    PORTA.OUT = 0b00001111;

    while (1) {
        // Check if ADC result is ready.
        if (ADC0.INTFLAGS & 0b00000001) {
            // Check which channel is selected
            if (ADC0.MUXPOS == 0x04) {   // PD2 (AIN7)
                result_left = adc_to_signmag_custom(ADC0.RES);
                //result_right = ADC0.RES;   // Read ADC value for PD2
                ADC0.MUXPOS = 0x03;       // Switch to PD1 (AIN6)
            } else if (ADC0.MUXPOS == 0x03) {  // PD1 (AIN6)
                result_right = adc_to_signmag_custom(ADC0.RES);   // Read ADC value for PD1
                //result_left = ADC0.RES;
                ADC0.MUXPOS = 0x04;       // Switch back to PD2 (AIN7)
            }
            ADC0.INTFLAGS = 0b00000001;
        }
    }
}