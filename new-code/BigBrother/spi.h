#define DDR_SPI     P1DIR
#define PORT_SPI    PORT1
#define PIN_SPI     PORT1 // ????

//#define DDR_SS      P1DIR
//#define PORT_SS     PORT1

//#define SS_PIN      PB1
//#define SS          PB2
#define MISO        P1_1
#define MOSI        P1_2
#define SCK         P1_4

#define ss_clear()  PORT_SS &=  ~(1<<SS_PIN);
#define ss_set()    PORT_SS |=  (1<<SS_PIN);