/*
       _________________                     _________________
       |     MSP430    |                     |     ST7540    |
       |               |                     |               |
       |   (REG_D) P2.0|---yellow -   red ---|2 Reg_Data     |
       |           GND |---green  - white ---|3 Gnd          |
       |    (MISO) P1.1|---red    -   red ---|4 RxD          |
       |    (RxTx) P2.1|---orange -orange ---|5 RxTx         |
       |    (MOSI) P1.2|---blue   -  blue ---|6 TxD          |
       |               |          -       ---|7 Bu/Therm     |
       |     (CLK) P1.4|---brown  - brown ---|8 CLR/T        |
       |               |                     |               |
       |               |                     |               |
       |_______________|                     |_______________|

ST7540 24-bit Control Register
0-2    Carrier Frequency [111]
3-4    Baud rate [11]
5      Deviation [1]
6      Watchdog [0]
7-8    Transmission time out [01]
9-10   Frequency detection time [01]
11     Reserved [0]
12-13  Detection method [10]
14     Mains interfacing mode [0]
15-16  Output clock [00]
17     Output voltage level freeze [1]
18     Header recognition [0]
19     Frame length count [0]
20     Header length [1]
21     Extended register mode [0]
22     Sensitivity mode [0]
23     Input filter [0]

MSB -> LSB
0 0 0 1 0 0 1 0 = 0x12
0 0 0 1 0 1 0 1 = 0x15
0 0 1 1 1 1 1 1 = 0x3F

Data and Control register access bits configuration:
              REG_DATA  RxTx
Data Tx          0       0
Data Rx          0       1
Ctrl Read        1       1
Ctrl Write       1       0
*/



#include "io430.h"
#include "msp430g2553.h"





#define F_CPU   16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "spi.h"
#include "spi.c"
#include "uart.h"
#include "uart.c"
#include "main.h"
#include "ST7540.h"
#include "ST7540.c"

void port_init(){
    //Configure Output
    DDR_LED |=  (1<<LED1) | (1<<LED2);
}

int main(void){
    uint8_t flag=0;
	
	// Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
    
    //Initilize ports
    port_init();
    //Initilize SPI
    spi_init();
    //Inilitize UART
    uart_init();
    //Initilize ST7540
    ST7540_init();
    //Setup ST7540
    ST7540_setup();
    //Enter data access mode
    data_acc();
    _delay_ms(1);
    //Enter register access mode
    reg_acc();
    //Enter receive mode
    rx_mode();
    //Wait for SPI rising edge
    while ((PIN_SPI  &   (1<<SCK))!=0);
    //Clear SS SPI pin
    ss_clear();
    //SPI Receive 3 bytes from ST7540 register
    spi_receive();
    spi_receive();
    spi_receive();
    //Set SS PIN
    ss_set();
    _delay_ms(1);
    data_acc();
    rx_mode();
    
    //Turn off indication LEDS
    PORT_LED    |=  (1<<LED1);
    PORT_LED    |=  (1<<LED2);
    //Enable Global interrupts
    sei();
    while (1) {
        //If there is no data on the Mains start transmittion
        if ((PIN_CD_PD & (1<<CD_PD))!=0) {
            //Check for available data on UART's buffer
            if (uart_buffer_write_pointer!=uart_buffer_read_pointer) {
                //ST540 transmit
                ST7540_putc(uart_buffer[uart_buffer_read_pointer]);
                //Blink led
                if (flag==0) {
                    PORT_LED    |=  (1<<LED1);
                    flag=1;
                }else{
                    PORT_LED    &=  ~(1<<LED1);
                    flag=0;
                }

                uart_buffer_read_pointer++;
                if (uart_buffer_read_pointer>BUFFER_SIZE-1) uart_buffer_read_pointer=0;
            }
        }
    }
    
    return 0;
}