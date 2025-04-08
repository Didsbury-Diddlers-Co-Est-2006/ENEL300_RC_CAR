#include <avr/io.h>

int main(void) {
    /* Replace with your application code */
   
    CCP = 0xd8;
    CLKCTRL.OSCHFCTRLA = 0b00010100;
    while( CLKCTRL.MCLKSTATUS & 0b00000001 );

    // Set PC2 and PC3 as outputs
    PORTC.DIRSET = (1 << 2) | (1 << 3);
    // Set PD1?PD4 as outputs
    PORTD.DIRSET = 0b00011110;

    // Set PC2 and PC3 high
    PORTC.OUTSET = (1 << 2) | (1 << 3);
    // Set PD1 and PD3 high (LFWD and RFWD), PD2 and PD4 low (LBWD and RBWD)
    PORTD.OUT = (1 << 1) | (1 << 3);

    TCA0.SINGLE.CTRLA = 0b00001111;
    TCA0.SINGLE.PER = 0xffff;

    int period = 7813;

    while (1) {

        while( TCA0.SINGLE.CNT <= period );
        TCA0.SINGLE.CNT = 0;

        PORTA.OUT = 0b00010100;

        while( TCA0.SINGLE.CNT <= (period * 3));
        TCA0.SINGLE.CNT = 0;

        PORTA.OUT = 0b00000000;

        while( TCA0.SINGLE.CNT <= period);
        TCA0.SINGLE.CNT = 0;

        PORTA.OUT = 0b00001010;

        while( TCA0.SINGLE.CNT <= (period * 3));
        TCA0.SINGLE.CNT = 0;

        PORTA.OUT = 0b00000000;

    }
}
