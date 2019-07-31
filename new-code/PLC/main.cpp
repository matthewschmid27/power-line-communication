#include "msp430g2553.h"
#include "stdint.h"

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




//											     			   |		    |  		     |
// bool CTRL_SETUP[] = { 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0,  1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, ~,			 1}; // msb first
//											     			   |		    |		 	 |   														                 
//						0x 0			  9		 |     C             A      |      7           F
// Test			[09,CA,7F]^  ^
int CTRL_SETUP[] = {0x13, 0x95, 0x3F}; // 12, 15, 3F

volatile int ctrl_index = 0;
volatile uint8_t  cData;
volatile uint8_t  rData;



volatile int state = 0;
volatile int State_Data = 0;
volatile int State_Receiver = 0;
volatile uint8_t choice = 0;
volatile int state2 = 0;

uint8_t Data_Header = 0x10;
uint8_t Red_Rec     = 0x09;
uint8_t Blue_Rec    = 0x06;

volatile uint8_t Byte;
volatile uint8_t Receiver_Byte = 0x00;

volatile uint8_t Prep_Byte_LSN;
volatile uint8_t Prep_Byte_MSN;
volatile uint8_t Byte_MSN;
volatile uint8_t Byte_LSN;
volatile uint8_t Go = 1;
volatile uint8_t Go2 = 1;

void pinConfig(void)
{
	P1DIR |= BIT0 + BIT6; // red LED
	P2DIR |= BIT0 + BIT1; // red LED
	P1REN = BIT3; // push button
        P1OUT = BIT3;	 
}

void TimerConfig(void)
{
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  TACTL = TASSEL_2 + MC_1;           // SMCLK/8, upmode  
  TACCR0 =  1200;                           // 12.5 Hz   	 
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
    while((!(IFG2 & UCA0TXIFG)));
}



uint8_t spi_receive( uint8_t rData)
{
//Wait for reception complete
while(!UCA0TXIFG);
rData = UCA0RXBUF;
   
return rData;
}

void ST7540_setup(uint8_t  Firt_Byte, uint8_t  Second_Byte, uint8_t  Third_Byte) //TESTED AND WORKS
{	
	while((!(P1IN & BIT4)));
        state2 = 2;

        reg_acc();
        tx_mode();
        UCA0TXBUF = Firt_Byte;
        while((!(IFG2 & UCA0TXIFG)));

        UCA0TXBUF = Second_Byte;
        while((!(IFG2 & UCA0TXIFG)));

        UCA0TXBUF = Third_Byte;
        while((!(IFG2 & UCA0TXIFG)));
        TACCR0 = 2900;
        
        // 1111 : 1011 :   1010 :    
        //  F       B       A
        End_Data();
        rx_mode()

}

void Rec_Selectrion_Transmit(uint8_t  Rec_Sel_Byte)                              //TESTED AND WORKS
{	
	while((!(P1IN & BIT4)));
        state2 = 2;

        data_acc();
        tx_mode();
        Rec_Sel_Byte |= 0x10;
        UCA0TXBUF = Rec_Sel_Byte;
        while((!(IFG2 & UCA0TXIFG)));
        TACCR0 = 2900;
        End_Data();
        rx_mode();
}

void Receiver_Selection(uint8_t  choice)                                         //TESTED AND WORKS
{
  while (Receiver_Byte == 0x00)
  {
   while(Go2)
 {
  switch(State_Receiver)
  {
        //State_Receiver = 0: Infinite loop where we wait for button interrupt
	//		      stating a recievier has been selected
    
        case 0:                                
          {                                    
          while(1)
          {
            State_Receiver = 1;
            break;
          };
          }
        
        //State_Receiver = 1: An interrupt has occurred, check that status for 
	//		      wether its red or blue rec. Then prepare the Byte
        //                    for reciever seletion control
            
        case 1:                                                         
          {
          if (choice == Red_Rec)
            {
            Receiver_Byte |= Red_Rec;                         //Red Receiver control byte: 0x19
            Receiver_Byte |= Data_Header;                     //(Data header is the 0b0001 shifted
            State_Receiver = 2;
            }                                                 // to the most significant byte
          else if (choice == Blue_Rec)
            {
            Receiver_Byte |= Blue_Rec;      //Blue Receiver control byte: 0x16
            Receiver_Byte |= Data_Header;  //(Data header is the 0b0001 shifted
            State_Receiver = 2;
            }                                                 // to the most significant byte
          else
            {
              Receiver_Byte = 0x00;                           // No selection? shouldnt have gotten here idk
              State_Receiver = 0;
            }
          
          }
          
          if (Receiver_Byte != 0x00)
            break;
          
          else
            State_Receiver = 0;
          
          case 2:
           Go2 = 0;
  }; 
 }
}
}


void    Prepare_Data_Byte(uint8_t  Byte)                                         //TESTED AND WORKS
{
 Byte_MSN = 0x0;
 Byte_LSN = 0x0;

    
    Byte_MSN = (Byte>>4);                   // Shift the Byte over 4: I.E. (0b10100000) => (0b00001010)
    Byte_MSN |= Data_Header;                // We insert the Data header to the prepared,
                                            // the byte, I.E. (0b00001010) => (0b00011010)
    Byte_LSN = Byte;
    Byte_LSN &=0x0F;                        // We want to mask the most significant nibble when trying to transmit 
                                            // least signifanct nibble I.E (0b11001010) => (0b00001010)
    Byte_LSN |= Data_Header;                // Afterwards we insert the data header
    
    Prep_Byte_MSN = Byte_MSN;
    Prep_Byte_LSN = Byte_LSN;
    
    Byte_MSN = 0;
    Byte_LSN = 0;
    Byte = 0;
}                                     
  
  
  

void    Data_Transmit(uint8_t  Data_Byte, uint8_t Receiver_Byte)                 //TESTED AND WORKS
{	
  while(Go)
  {
	switch(State_Data)
        { 
          //State_Data = 0: First step of transmitting proccess, we are sending the receiver
          //                Selection control word, afterword we send data.
          
           case 0:
            {
              if (Receiver_Byte == 0)
                {
                State_Data = 0;
                break;
                }
              else
                {
                Rec_Selectrion_Transmit(Receiver_Byte);
                state2 = 2;
                State_Data = 1;
                Delay_mS();
                
                }
            }
        
          //State_Data = 1: Now we are actually sending Data bits, first we prepare the byte
          //                by seperating each nibble and adding a header.
        
           case 1:
              {
              Prepare_Data_Byte(0x52);
              Delay_mS();
              State_Data = 2; 
              }
        
          //State_Data = 2: Now we are actually sending Data bits, first we prepare the byte
          //                by seperating each nibble and adding a header.
        
           case 2:
              {
              while((!(P1IN & BIT4)));
              tx_mode();
              data_acc();
                
              UCA0TXBUF = Prep_Byte_MSN;
              while((!(IFG2 & UCA0TXIFG)));

              UCA0TXBUF = Prep_Byte_LSN;
              while((!(IFG2 & UCA0TXIFG)));
              TACCR0 = 2900;
        

              End_Data();
              rx_mode();
              State_Data = 3;
              }
              
          //State_Data = 3: Finished data transfer process, leave function
          //                
        
             case 3:
              {
              Delay_mS();
              Delay_mS();
              State_Data = 0;
              Go = 0;
              break;
              }
             break;
        }

  };
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// MAIN VOID /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////









// IN NEED OF DOING
// KEYPAD CODE WITH INTERRUPTS FOR CHOOSING RECEIVER AND CHOOSING DATA VALUE

int main( void )
{	

DWDTCTL = WDTPW + WDTHOLD;//Stop the watchdog

__delay_cycles(1000000);

pinConfig();					//Configure Input/Output Pins
spiConfig();					//Configure SPI USCI controller
TimerConfig();
ISR_Timer_OFF();

data_acc();				        //Data Mode (start)
rx_mode();                                      //Recieve mode (start)
        

//////////////////////////State Machine Infinite Loop//////////////////////////
	
while(1)
 {
  
  switch(state)
  {
			
	  //State 0: A Buffer stage where the user can wait to press
	  //		 a button till moving to next stage
	
	  case 0:	
		{
		if(Button_Check_ON)			     //Waiting on first button press
		{
			P1OUT ^= BIT0; 			     //Toggle red LED
				
		  while(1)				     ///////////////
		  {					     //	
			if(Button_Check_ON)		     //
			{				     //
			state = 0; 			     //
			}				     //
			else    	                     // A while loop that prevents
			{           	                     // holding button from effecting
			state = 1;  		             // State machine
			Delay_mS()			     //
			Delay_mS()		             //
			break;          		     //
			}                   	             //
		  }                       	             ///////////////
		}
		break;
		}
		
	  //State 1: The State were we send the control word to ST7540, then moves
	  //		 onward to next state (an infinite loop)
		
	case 1:
	   {	
		ST7540_setup(0x12,0x85,0xDF);
		state = 2;					//Toggle State 3, an infinite loop (skips 
		break;						//state 2)
	   }

	  //State 2: UNUSED CURRENTLY

          //State 3: A temp while loop, here is where we wait for the interrupt
          //         thekeypad to start sending data.
	   
	case 2:
	  {
            P1OUT ^= BIT0; 			     //Toggle red LED
           __delay_cycles(1000000);
          Data_Transmit(0x95,0x09);
          while(1);
          {
          }
	   
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

