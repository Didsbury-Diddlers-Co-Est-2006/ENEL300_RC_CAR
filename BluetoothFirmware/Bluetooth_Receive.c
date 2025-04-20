/*
 * File:   Bluetooth.c
 * Author: devon
 *
 * Created on April 19, 2025, 7:06 PM
 */


 #include <avr/io.h>
 #
 
 //Define Clock speed, 16MHZ and Do not enable global interrupts before running USART_Init()
 void setup_clock() {
     CCP = 0xD8;
     CLKCTRL.OSCHFCTRLA = 0b00011100;      // Set 16 MHz internal oscillator
     while (CLKCTRL.MCLKSTATUS & 0b00000001); // Wait for clock switch to complete
 }
 
 void USART_Transmit(uint16_t data) {
     while (!(USART1.STATUS & USART_DREIF_bm));  // Wait until buffer is empty
     USART1.TXDATAL = data;  // Send data
 }
 
 uint16_t USART_Receive(void) {
    
     while (!(USART1.STATUS & USART_RXCIF_bm)){
        
     }  // Wait for data
     return USART1.RXDATAL;  // Return received byte
 }
 
 void USART_Init() {
     // Set baud rate
     //USART1.BAUD = 0000010011100010;
     USART1.BAUD = 6666;
     // Set frame format: 8-bit, no parity, 1 stop bit (8N1)
     USART1.CTRLC = 0b00001011;
     
     USART1.CTRLA = 
    
     // Set TXD (PC0 and PC1) as output
     PORTC.DIRSET = 0b00000001;
     PORTC.DIRCLR = 0b00000010; 
    
     // Enable transmitter and receiver
     USART1.CTRLB = 0b11000000;
 }
 
 volatile uint16_t value = 0;
 
 int main(void) {
     setup_clock();
     USART_Init();
     
     while (1) {  
         value = USART_Receive();
     }
 }