
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <BlynkSimpleEsp8266.h>
#include "uptime.h"
#include "string.h"
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

#define DEBUG 1 // Set up debug mode

// AUTH

#if (DEBUG == 1)
char auth[] = "JFCPeotBJkYlD6LLH8onX3jzscoDFM43"; // Токен для отладки
char esp_hostname[] = "esp_gate_debug";

#else
char auth[] = "5TXdW24ynSCtm8BaQ2Rqcbni3Tt_C_g3"; // Боевой токен
char esp_hostname[] = "esp_gate";
#endif

char ssid[] = "Route"; //Название WiFi-сети
char pass[] = "query123"; //Пароль




//Widgets
BlynkTimer timer; // timer for sending data to blynk
WidgetRTC rtc;  // Start rtc widget
WidgetTerminal terminal(V21);
WidgetLCD lcd(V3);

char currentTime[9];
char currentDate[11];



BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}

// timer for opened gate //

#define PERIOD 5000
unsigned long openTimer = 0;


// ESP pins
#define PIN_MAGNET D7   // Physical  pin to a3144 sensor
#define PIN_RELAY1 D6    // Physical  pin to Relay for full open
#define PIN_RELAY2 D5   // Physical  pin to Relay short open


#define garageSensorSW digitalRead(PIN_MAGNET) // read hall sensor

// virtual pins for BLYNK
#define BL_UPTIME V5     // uptime pin
#define BL_RSSI V6      // signal strength pin
#define BL_RELAY1 V1    //  full open gate
#define BL_RELAY2 V2   // short open gate



// relay

BLYNK_WRITE(BL_RELAY1) //функция, отслеживающая изменение виртуального пина
{
  int pinValue1 = param.asInt(); //переменная текущего состояния виртуального пина
  if (pinValue1 == 1) {
    digitalWrite(PIN_RELAY1, LOW);
    delay (500);
    digitalWrite(PIN_RELAY1, HIGH);
    Blynk.virtualWrite(BL_RELAY1, 0);
    // Terminal logging
    sprintf(currentDate, "%02d/%02d/%04d",  day(), month(), year());
    sprintf(currentTime, "%02d:%02d:%02d", hour(), minute(), second());
    terminal.print("[" + String(currentDate) + " | ");
    terminal.print(String(currentTime) + "]");
    terminal.println("Button " + String(BL_RELAY1) + " was pushed");
    terminal.println();
    terminal.println();
    terminal.flush();

  }
  else
  {
    digitalWrite(PIN_RELAY1, HIGH);
  }
}

BLYNK_WRITE(BL_RELAY2) //функция, отслеживающая изменение виртуального пина
{
  int pinValue2 = param.asInt(); //переменная текущего состояния виртуального пина
  if (pinValue2 == 1) {
    digitalWrite(PIN_RELAY2, LOW);
    delay (500);
    digitalWrite(PIN_RELAY2, HIGH);
    Blynk.virtualWrite(BL_RELAY2, 0);
  }
  else
  {
    digitalWrite(PIN_RELAY2, HIGH);
  }
}

void garageMagSensor ()
{

  lcd.clear();
  //Serial.print("Hall sensor state: ");Serial.println(garageSensorSW); // отображение статуса датчика хола в serial порт, для отладки.


  if (garageSensorSW == !LOW)
  {
    lcd.print(2, 0, "Ворота открыты"); // LCD print, column 2, row 0.


      if (millis() - openTimer >= PERIOD)
      {

        unsigned long sec = openTimer / 1000 ;
        //unsigned long minutes = openTimer / 1000 / 60;
        String notifyMessage=String("Ворота открыты ")+sec+String(" секунд") ;

        lcd.print(4, 1, sec);
        lcd.print(8,1, "сек");

        openTimer += PERIOD;

        switch(sec)
        {
          case 180:
          case 600:
          Blynk.notify(notifyMessage);
          break;
        }
      }
  }

  else
  {
    lcd.print(2, 0, "Ворота закрыты"); // LCD print, column 2, row 0
    openTimer = 0;
  }
}







void sendUptime ()
{
  uptime::calculateUptime();
  String uptime_format = (String)(uptime::getDays()) + "d:" + (String)(uptime::getHours()  ) + "h:" + (String)(uptime::getMinutes()) + "m" ;
  Blynk.virtualWrite(BL_UPTIME,  uptime_format);
  //String currentTime = String(hour()) + ":" + minute() + ":" + second();
}


void clockDisplay()
{
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details

  //String currentDate = String(day()) + " " + month() + " " + year();
  // Send time to the App
  sprintf(currentTime, "%02d:%02d:%02d", hour(), minute(), second());
  Blynk.virtualWrite(V0, currentTime);
}

// test terminal button state
BLYNK_WRITE(V20)
{

  int pinValue20 = param.asInt();
  if(pinValue20 == 1) {
    sprintf(currentDate, "%02d/%02d/%04d",  day(), month(), year());
    sprintf(currentTime, "%02d:%02d:%02d", hour(), minute(), second());
    terminal.print("[" + String(currentDate) + " | ");
    terminal.print(myTime + "]");
    terminal.println("Test Button was pushed");
    terminal.println();
    terminal.println();
  }
  terminal.flush();
}

void setup() //основная функция, выполняется один раз при подаче питания на микроконтроллер
{

  Serial.begin(115200); //открываем серийный порт



  WiFi.mode(WIFI_STA); // режим клиента
  WiFi.hostname(esp_hostname); //
  WiFi.begin(ssid, pass);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }


  setSyncInterval(10 * 60); // Sync interval in seconds (10 minutes)

    // Display digital clock every 10 seconds
  timer.setInterval(10000L, clockDisplay);


// !!!!
  ArduinoOTA.onError([](ota_error_t error) {
    ESP.restart();
  });
  ArduinoOTA.setHostname(esp_hostname); // !!!!
  ArduinoOTA.begin();

  Blynk.begin(auth, ssid, pass);

  //Terminal
  terminal.clear();

  // This will print Blynk Software version to the Terminal Widget when
  // your hardware gets connected to Blynk Server
  terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  terminal.println(F("-------------"));
  terminal.flush();

  // Pin mode, bootstate
  pinMode(PIN_RELAY1, OUTPUT); //объявляем D4 "выходным" пином
  pinMode(PIN_RELAY2, OUTPUT);
  pinMode(PIN_MAGNET, INPUT_PULLUP);
  digitalWrite(PIN_MAGNET, LOW);
  digitalWrite(PIN_RELAY1, HIGH);
  digitalWrite(PIN_RELAY2, HIGH);


// Setup a function with timer
  timer.setInterval(10000L, garageMagSensor);
  timer.setInterval(30000L, sendUptime);
  timer.setInterval(30000L, clockDisplay);

  Blynk.virtualWrite(BL_RSSI, WiFi.RSSI());
}



void loop() //основная функция, которая выполняется постоянно по кругу
{
  timer.run();
  Blynk.run(); // lets start magic
  ArduinoOTA.handle();
}
