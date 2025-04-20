===============================================================================
README - AVR128DB28 Bluetooth USART Communication
===============================================================================

PROJECT OVERVIEW
----------------
This project implements basic serial communication over Bluetooth using the 
USART1 peripheral of the Microchip AVR128DB28 microcontroller. It consists of 
two C files:

    - Bluetooth_Transmit.c : Transmits a fixed byte over USART1 repeatedly.
    - Bluetooth_Receive.c  : Waits for incoming data on USART1 and stores it.

The project is designed for compatibility with standard Bluetooth modules such 
as the HC-05 or HC-06, communicating at approximately 9600 baud.

===============================================================================
SYSTEM CONFIGURATION
===============================================================================

1. SYSTEM CLOCK SETUP
---------------------
Both files configure the AVR128DB28 to run from the internal 16 MHz oscillator:

    CCP = 0xD8;                           // Unlock protected register
    CLKCTRL.OSCHFCTRLA = 0b00011100;     // Select 16 MHz oscillator
    while (CLKCTRL.MCLKSTATUS & 0x01);   // Wait for clock switch to complete

This ensures that all system timing is based on a stable 16 MHz reference.

-------------------------------------------------------------------------------

2. USART1 CONFIGURATION
-----------------------
USART1 is configured in both files as follows:

    - Baud Rate      : 6666 (approx 9600 bps at 16 MHz)
    - Data Bits      : 8
    - Parity         : None
    - Stop Bits      : 1
    - Mode           : Asynchronous
    - Transmitter    : Enabled
    - Receiver       : Enabled

Register settings:

    USART1.BAUD  = 6666;                 // Baud rate (see notes below)
    USART1.CTRLC = 0b00001011;           // 8-bit, No parity, 1 stop bit (8N1)
    USART1.CTRLB = 0b11000000;           // TXEN and RXEN bits set

NOTE:
The value 6666 corresponds to the formula:
    BAUD = (f_CLK_PER / (2 * f_BAUD)) - 1
    = (16000000 / (2 * 9600)) - 1 ≈ 832
But in this code, a baud value of 6666 is used — this likely represents
a fractional setting or empirical tuning to match the module's expectations.

-------------------------------------------------------------------------------

3. PIN CONFIGURATION
--------------------
The TX and RX pins are assigned as follows:

    - PC0 : TXD (Output from AVR to Bluetooth RX)
    - PC1 : RXD (Input from Bluetooth TX to AVR)

Direction settings:
    PORTC.DIRSET = 0b00000001;  // Set PC0 (TXD) as output
    PORTC.DIRCLR = 0b00000010;  // Set PC1 (RXD) as input

Make sure Bluetooth modules are connected appropriately:
    - Bluetooth RX  <-- PC0 (TXD from AVR)
    - Bluetooth TX  --> PC1 (RXD into AVR)
    - Shared GND between Bluetooth and AVR

-------------------------------------------------------------------------------

4. TRANSMIT LOGIC (Bluetooth_Transmit.c)
----------------------------------------
Main loop continuously transmits a hardcoded 16-bit value (0x00FF).

Key functions:

    void USART_Transmit(uint16_t data)
    {
        while (!(USART1.STATUS & USART_DREIF_bm));
        USART1.TXDATAL = data;
    }

    int main(void)
    {
        setup_clock();
        USART_Init();

        while (1) {
            USART_Transmit(0x00FF);     // Continuously send 0x00FF
        }
    }

-------------------------------------------------------------------------------

5. RECEIVE LOGIC (Bluetooth_Receive.c)
--------------------------------------
Main loop blocks until a character is received and stores it in a global variable.

Key functions:

    uint16_t USART_Receive(void)
    {
        while (!(USART1.STATUS & USART_RXCIF_bm));  // Wait for RX complete
        return USART1.RXDATAL;
    }

    int main(void)
    {
        setup_clock();
        USART_Init();

        while (1) {
            value = USART_Receive();    // Read data into global variable
        }
    }

NOTE:
`value` is declared as volatile to prevent compiler optimization that 
could interfere with correct operation when reading asynchronously.

-------------------------------------------------------------------------------

6. PERIPHERAL ENABLEMENT
------------------------
    - No interrupts are used.
    - The global interrupt flag is NOT enabled (sei() is not called).
    - Communication is entirely polling-based, suitable for basic I/O.

-------------------------------------------------------------------------------

7. REGISTER USAGE SUMMARY
--------------------------

USART1 Registers:
-----------------
    - USART1.BAUD      : Baud rate divisor (16-bit)
    - USART1.CTRLB     : Enable TX (bit 6) and RX (bit 7)
    - USART1.CTRLC     : Frame format selection (8N1 = 0x0B)
    - USART1.STATUS    : Status flags (DREIF for TX ready, RXCIF for RX complete)
    - USART1.TXDATAL   : Data register for transmission
    - USART1.RXDATAL   : Data register for reception

PORTC Registers:
----------------
    - PORTC.DIRSET     : Set PC0 as output
    - PORTC.DIRCLR     : Set PC1 as input

CLKCTRL Registers:
------------------
    - CLKCTRL.OSCHFCTRLA  : Oscillator frequency selection
    - CLKCTRL.MCLKSTATUS  : Used to wait for oscillator to be stable

===============================================================================
HARDWARE REQUIREMENTS
===============================================================================

- Microcontroller:
    AVR128DB28 running at 5V or 3.3V

- Bluetooth Module:
    HC-05, HC-06, or equivalent, 3.3V or 5V tolerant

- Wiring:
    PC0 (TXD) -> Bluetooth RX
    PC1 (RXD) <- Bluetooth TX
    GND       -> Shared ground
    VCC       -> 3.3V or 5V depending on Bluetooth module

- Optional:
    - Voltage divider or level shifter if using 3.3V Bluetooth with 5V MCU

===============================================================================
ADDITIONAL NOTES
===============================================================================

- USART1 is used instead of USART0, freeing USART0 for other peripherals or debugging.

- The USART transmit/receive buffers are accessed directly — there is no DMA 
  or interrupt handling in this implementation.

- The current implementation is minimal, suitable for embedded device pairing 
  or diagnostics, but should be expanded with ring buffers or interrupt-driven 
  I/O for performance-critical applications.

- This configuration matches the Microchip-specified USART register layout 
  from the AVR128DB28 datasheet and USART profile summary.

===============================================================================
REFERENCES
===============================================================================

- AVR128DB28 Datasheet
- AVR128DB28 USART Profile (USART1 Register Map)
- Bluetooth Module HC-05/HC-06 Datasheet
- Atmel/Microchip USART application notes

===============================================================================
END OF README.TXT
===============================================================================
