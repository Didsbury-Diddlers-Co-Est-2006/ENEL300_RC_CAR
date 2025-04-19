Define Clock speed, 16MHZ and Do not enable global interrupts before running USART_Init()



void USART_Transmit(uint16_t data) {
    while (!(USART1.STATUS & USART_DREIF_bm));  // Wait until buffer is empty
    USART1.TXDATAL = data;  // Send data
}


uint16_t USART_Receive(void) {
   
    while (!(USART1.STATUS & USART_RXCIF_bm)){
       
    }  // Wait for data
    return USART1.RXDATAL;  // Return received byte
}


void USART_Init(uint16_t baud) {
    // Set baud rate
    //USART1.BAUD = 0000010011100010;
    USART1.BAUD = 6666;
    // Set frame format: 8-bit, no parity, 1 stop bit (8N1)
    USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;
   
    // Set TXD (PC0 and PC1) as output
    PORTC.DIRSET = PIN0_bm;
    PORTC.DIRCLR = PIN1_bm;  
   
    // Enable transmitter and receiver
    USART1.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
}