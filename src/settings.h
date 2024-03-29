
//
#define DEBUG_MODE 0  // if enable - debug token; hall sensor value print to terminal
#define LOCATION 1     // 1 =  Shatura ; 2 = Svyatoozerskaya; 3 = Chistova

//
int MIN_THRESHOLD = 525;
int MAX_THRESHOLD = 560;

bool f_opened_notyfy;
/**************************************************************

                            Pins

 **************************************************************/
 // ESP pins
 #define HALL_SENSOR                A0   // Physical  pin to hall sensor

 #define PIN_RELAY1                 D5    // Physical  pin to Relay for full open
 #define PIN_RELAY2                 D6   // Physical  pin to Relay short open

// Virtual pins
#define vPIN_TERMINAL               V21
#define vPIN_CUR_DATE               V8
#define vPIN_LCD                    V3

#define vPIN_UPTIME                 V5     // uptime pin
//#define BL_RSSI V6      // signal strength pin
#define vPIN_RELAY1                 V1    //  full open gate
#define vPIN_RELAY2                 V2   // short open gate


// #define vPIN_TEST_BTN               V20

#define vPIN_GATE_TABLE_CLR         V13
#define vPIN_GATE_TABLE             V23
#define vPIN_GATE_LAST              V11
#define vPIN_GATE_HELD              V14
#define vPIN_GATE_COUNTER           V12

#define vPIN_TABLE_COUNTER          V16
#define vPIN_NOTIFY_DELAY           V17

/**************************************************************

                            Globals

 **************************************************************/



WidgetRTC rtc;
WidgetLCD lcd(vPIN_LCD);
WidgetTerminal terminal(vPIN_TERMINAL);

// Blynk LED
WidgetLED led1(V19);
#define vPIN_LED        V19
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_RED       "#D3435C"


int timer1, timer2, timer3;
BlynkTimer timer;
int today = 0;

String        GateLastOpened;
int           GateSwitchCurrent, notificationSent;
long          GateSwitchMillisHeld, GateSwitchSecsHeld, notifyDelay;
unsigned long GateSwitchFirstTime;       // how long since the button was first pressed
int           GateSwitchPrev;

int           GateDailyCounter, tableIndex1;

int           tableIndex2 = 0;
