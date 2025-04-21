/*
 * File:   ECU_S&R.c
 * Author: devon
 *
 * Created on April 19, 2025, 7:06 PM
 */

 #define F_CPU 16000000UL

 #include <avr/io.h>
 #include <util/delay.h>
 
 #define PACKET_SIZE 3 
  
 //Define Clock speed, 16MHZ and Do not enable global interrupts before running USART_Init()
 void setup_clock() {
      CCP = 0xD8;
      CLKCTRL.OSCHFCTRLA = 0b00011100;      // Set 16 MHz internal oscillator
      while (CLKCTRL.MCLKSTATUS & 0b00000001); // Wait for clock switch to complete
  }
  
  void USART_Transmit(uint16_t data) {
      while (!(USART0.STATUS & USART_DREIF_bm));  // Wait until buffer is empty
      USART0.TXDATAL = data;  // Send data
  }
  
  uint16_t USART_Receive(void) {
     
      while (!(USART0.STATUS & USART_RXCIF_bm)){
 
      }  // Wait for data
      return USART0.RXDATAL;  // Return received byte
  }
  
  void USART_Init() {
      // Set baud rate
      //USART1.BAUD = 0000010011100010;
      USART0.BAUD = 6666;
      // Set frame format: 8-bit, no parity, 1 stop bit (8N1)
      USART0.CTRLC = 0b00001011;
     
      PORTMUX.USARTROUTEA = 0b00000001;

      // Set TXD (PA0) and RXD (PA1)
      PORTA.DIRSET = 0b00010000;
      PORTA.DIRCLR = 0b00100000; 
     
      // Enable transmitter and receiver
      USART0.CTRLB = 0b11000000;
  }
 
 volatile uint16_t left_sm = 0;                // Or whatever variable you have for left motor speed & direction
 volatile uint16_t right_sm = 0;               // Or whatever variable you have for right motor speed & direction
 volatile uint16_t light_toggle = 0;
 volatile uint16_t rx_buff [3];
 
  int main(void) {
     setup_clock();
     USART_Init();
 
     while (1) {
    // --- Transmit response packet ---
     USART_Transmit(0x0069);  // Takes in sensor data variable -> converted to 0x00XX format        
 
     _delay_ms(1);
    // --- Receive control packet ---
 
     for (int i = 0; i < 3; i++) {
             rx_buff[i] = USART_Receive();
         }
     }
  }