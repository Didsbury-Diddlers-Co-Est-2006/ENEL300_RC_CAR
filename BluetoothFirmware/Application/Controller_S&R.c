/*
 * File:   Controller_S&R.c
 * Author: devon
 *
 * Created on April 19, 2025, 7:06 PM
 */


 #include <avr/io.h>
 
 #define PACKET_SIZE 3

 volatile uint8_t rx_buffer[PACKET_SIZE];
 volatile uint8_t tx_buffer[PACKET_SIZE];
 
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

 int main(void) {
    setup_clock();
    USART_Init();

    while (1) {
        // --- Transmit control packet ---
        tx_buffer[0] = left_motor_sm_value;
        tx_buffer[1] = right_motor_sm_value;
        tx_buffer[2] = light_toggle_bit;

        for (uint8_t i = 0; i < PACKET_SIZE; i++) {
            USART_Transmit(tx_buffer[i]);
        }

        // --- Receive response packet ---
        for (uint8_t i = 0; i < PACKET_SIZE; i++) {
            rx_buffer[i] = USART_Receive();
        }

        // Handle sensor display
        display_distance(rx_buffer[0]);
        display_metal(rx_buffer[1]);
    }
}
