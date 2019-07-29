#include "msp430g2553.h"
#include "stdint.h"
#include "iostream.h"

#define tx_mode()       P2OUT   &= ~BIT1;
#define rx_mode()       P2OUT   |=  BIT1;

#define reg_acc()       P2OUT   |=  BIT0;
#define data_acc()      P2OUT   &= ~BIT0;

#define SS_set()        SS_Status = 1;
#define SS_clear()      SS_Status = 0;

#define Delay_mS()		__delay_cycles(1000);

#define End_Data() spi_transmit(0x00);

#define Button_Check_ON		(!(P1IN & BIT3))
#define Button_Check_OFF	((P1IN & BIT3))

#define Data_acc_Delay()		TACCR0 = 555
//7549
#define ISR_Timer_OFF()		TACCR0 = 0
#define Button_Check_OFF	((P1IN & BIT3))



//									|		     |			   |		    |  		     |
// bool CTRL_SETUP[] = {0, 0, 0, 0, |1, 0, 0, 1, |1, 1, 0, 0,  |1, 0, 1, 0, |1, 0, 0, 1, |1, 1, 1, ~,			 1}; // msb first
//									|		     |			   |		    |		 	 |   														                 
//						0x 0			  9		 |     C             A      |      7           F
// Test			[09,CA,7F]^  ^
int CTRL_SETUP[] = {0x13, 0x95, 0x3F}; // 12, 15, 3F

volatile int ctrl_index = 0;
volatile uint8_t  cData;
volatile uint8_t  rData;
volatile uint8_t  rData1;
volatile uint8_t  rData2;
volatile uint8_t  rData3;


volatile int state = 0;


void pinConfig(void)
{
	P1DIR |= BIT0 + BIT6; // red LED
	P2DIR |= BIT0 + BIT1; // red LED
	P1REN = BIT3; // push button
	//P1OUT = BIT3;	 
}

void TimerConfig(void)
{
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  TACTL = TASSEL_2 + MC_1 + ID_3;           // SMCLK/8, upmode  
  TACCR0 =  555;                           // 12.5 Hz   	 
}

void spiConfig(void)
{
	// setup spi pins: MISO on P1.1, MOSI on P1.2, CLK on P1.4
	while((!(P1IN & BIT4))); // wait until the clock signal arrives at P1.4
	
	P1SEL = BIT1 + BIT2 + BIT4;
	P1SEL2 = BIT1 + BIT2 + BIT4;
	UCA0CTL1 = UCSWRST; // put state machine in reset
	UCA0CTL0 |=  UCMSB + UCSYNC + UCCKPH; // msb first, synchronous, clk phase select (3-pin, 8-bit SPI slave)
	UCA0CTL1 &= ~UCSWRST; // initialize USCI state machine
//	

	__bis_SR_register((GIE)); // enable interrupts
}

void spi_transmit(uint8_t  cData)
{
    UCA0TXBUF = cData;
	//Wait for transmission complete
	while(!(IFG2 & UCA0TXIFG))
{
};
	
	
}

uint8_t spi_receive( uint8_t rData)
{
//Wait for reception complete
while(!UCA0TXIFG);
rData = UCA0RXBUF;
   
return rData;
}

void ST7540_setup()
{	
	 

	while((!(P1IN & BIT4)));
	
	reg_acc();
	tx_mode();
	 //Send configuration bytes to ST7540 control register

	spi_transmit(0x12);
	spi_transmit(0x15);	
	spi_transmit(0x3F);
	data_acc(); 
	rx_mode(); 
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// MAIN VOID /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int main( void )
{	

	WDTCTL = WDTPW + WDTHOLD;		//Stop the watchdog
	pinConfig();					//Configure Input/Output Pins
	spiConfig();					//Configure SPI USCI controller
	TimerConfig();
	ISR_Timer_OFF();
	
	data_acc();						//Data Mode
    rx_mode();						//Recieving data from MSP430
	
	Delay_mS();
	
    ST7540_setup();					//Send control word to ST7540
	End_Data();
	rx_mode();
	
//////////////////////////State Machine Infinite Loop//////////////////////////
	
while(1)
 {
  
  switch(state)
  {
			
	  //State 0: A Buffer stage where the user can wait to press
	  //		 a button till moving to next stage
	
	  case 0:	
		{
		if(Button_Check_ON)				//Waiting on first button press
		{
			P1OUT ^= BIT0; 			//Toggle red LED
				
		  while(1)					///////////////
		  {							//	
			if(Button_Check_ON)		//
			{						//
			state = 0; 				//
			}						//
			else    	            // A while loop that prevents
			{           	        // holding button from effecting
			state = 1;  		    // State machine
			Delay_mS()				//
			Delay_mS()				//
			break;          		//
			}                   	//
		  }                       	///////////////
		}
		break;
		}
		
	  //State 1: The State were we send the control word to ST7540, then moves
	  //		 onward to next state (an infinite loop)
		
	case 1:
	   {	
		//ST7540_setup()	;
		state = 3;					//Toggle State 3, an infinite loop (skips 
		break;						//state 2)
	   }

	  //State 2: UNUSED CURRENTLY
	  //		
	   
	case 2:
	   {
		reg_acc();
		rx_mode();
		while((!(P1IN & BIT4)));
		
		spi_receive(rData1);
		spi_receive(rData2);
		spi_receive(rData3);
		cout<< rData1;
		cout<< rData2;
		cout<< rData3;
		
		tx_mode();
		data_acc();
		state = 3;
		break;
	   }
  
	  //State 3: A temp while loop, here is where rest of data transfer code will
	  //		 go
	   
	case 3:
	  {
		while(1)
		{
		}	
	   break;
	 }
  }
 }
 
return 0;
}

//Timer ISR
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A_CCR0_ISR(void)
{
	TACCR0 = 0; //Increment Over-Flow Counter
	data_acc();
}

#pragma USCIA0_TX_USCIB0_TX_ISR
__interrupt void USCI_A0_ISR(void)
{
    volatile unsigned int i;

    switch(__even_in_range(UCA0IV,0x04))
    {
        case 0: break; // Vector 0 - no interrupt
        case 2:

            break;
        case 4:
		  while (P2OUT & BIT0);

            UCA0TXBUF = cData; // Transmit characters
            IE2 &= ~UCA0TXIE;
            break;

         default: break;
   }
}