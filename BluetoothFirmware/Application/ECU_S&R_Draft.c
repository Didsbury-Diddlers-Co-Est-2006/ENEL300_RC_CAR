/*
 * File:   ECU_S&R.c
 * Author: devon
 *
 * Created on April 19, 2025, 7:06 PM
 */


 #include <avr/io.h>
 #include <util/delay.h>

 #define PACKET_SIZE 3

 volatile uint16_t rx_buffer[PACKET_SIZE];
 volatile uint16_t tx_buffer[PACKET_SIZE]; 
 
 
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

volatile uint16_t left_sm = 0;                // Or whatever variable you have for left motor speed & direction
volatile uint16_t right_sm = 0;               // Or whatever variable you have for right motor speed & direction
volatile uint16_t light_toggle = 0;

 int main(void) {
    setup_clock();
    USART_Init();

    while (1) {
        // --- Receive control packet ---
        for (uint16_t i = 0; i < PACKET_SIZE; i++) {
            rx_buffer[i] = USART_Receive();
        }

        // Decode motor values and light toggle
        left_sm = rx_buffer[0];                // Or whatever variable you have for left motor speed & direction
        right_sm = rx_buffer[2];               // Or whatever variable you have for right motor speed & direction
        light_toggle = rx_buffer[1];    //

        //update_motors(left_sm, right_sm); //Or whatever function you have for updating motor values
        //update_lights(light_toggle);      //Or whatever function you have for updating light values

        // --- Transmit response packet ---
        tx_buffer[0] = 0b0000000010101010;     // e.g., ultrasonic
        tx_buffer[1] = 0b0000000000000001;     // e.g., battery
        tx_buffer[2] = 0b0000000000000000;

        for (uint8_t i = 0; i < PACKET_SIZE; i++) {
            USART_Transmit(tx_buffer[i]);
            _delay_ms(10);
        }
    }
}