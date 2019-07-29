//#define DDR_BU_THERM    P1DIR
//#define DDR_CD_PD       P1DIR

#define DDR_RXTX        P2DIR
#define DDR_REG_DATA    P2DIR

//#define PORT_BU_THERM   PORT1
//#define PORT_CD_PD      PORT1

#define PORT_RXTX       PORT2
#define PORT_REG_DATA   PORT2

//#define PIN_CD_PD       PIN?

//#define BU_THERM        ?
//#define CD_PD           ?

#define REG_DATA        P2_0
#define RXTX            P2_1

#define reg_acc()       PORT_REG_DATA   |=  (1<<REG_DATA);
#define data_acc()      PORT_REG_DATA   &=  ~(1<<REG_DATA);

#define tx_mode()       PORT_RXTX   &=  ~(1<<RXTX);
#define rx_mode()       PORT_RXTX   |=  (1<<RXTX);