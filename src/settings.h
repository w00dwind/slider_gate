
//
#define DEBUG_MODE 1  // if enable - debug token
#define LOCATION 1     // 1 =  Chistova.st ; 2 = Svyatoozerskaya; 3 = Shatura
//

/**************************************************************

                            Pins

 **************************************************************/
 // ESP pins
 #define PIN_MAGNET D7   // Physical  pin to a3144 sensor
 #define PIN_RELAY1 D6    // Physical  pin to Relay for full open
 #define PIN_RELAY2 D5   // Physical  pin to Relay short open

// Virtual pins
#define vPIN_TERMINAL               V21
#define vPIN_CUR_DATE               V8
#define vPIN_LCD                    V3

#define vPIN_UPTIME                 V5     // uptime pin
//#define BL_RSSI V6      // signal strength pin
#define vPIN_RELAY1                 V1    //  full open gate
#define vPIN_RELAY2                 V2   // short open gate


#define vPIN_TEST_BTN               V20

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



int timer1, timer2, timer3;
BlynkTimer timer;
int today = 6;

String GateLastOpened;
int           DoorBellButtonCur, GateSwitchCurrent, notificationSent;
long          GateSwitchMillisHeld, GateSwitchSecsHeld, notifyDelay;
unsigned long GateSwitchFirstTime;       // how long since the button was first pressed
byte          GateSwitchPrev = LOW;

int GateDailyCounter, tableIndex1;

int           tableIndex2 = 0;
