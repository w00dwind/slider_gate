

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <BlynkSimpleEsp8266.h>
#include "uptime.h"
#include "string.h"
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include "settings.h"
#include "secret.h"
#include "functions.h"


BLYNK_WRITE(vPIN_GATE_COUNTER) {
  GateDailyCounter = param.asInt();
}
BLYNK_WRITE(vPIN_TABLE_COUNTER) {
  tableIndex1 = param.asInt();
}

BLYNK_WRITE(vPIN_GATE_TABLE_CLR) {
  if (param.asInt()) {
    Blynk.virtualWrite(vPIN_GATE_TABLE, "clr" );
    printOutput("Gate Table Cleared");
  }
}

BLYNK_WRITE(vPIN_NOTIFY_DELAY) {
  notifyDelay = param.asInt() * 60;
  (notifyDelay) ? printOutput(String("Notify Delay: ") + String(param.asInt()) + String(" min")) : printOutput(String("Notify Disabled "));
}

// relay

BLYNK_WRITE(vPIN_RELAY1) //функция, отслеживающая изменение виртуального пина
{
  int pinValue1 = param.asInt(); //переменная текущего состояния виртуального пина
  if (pinValue1 == 1) {
    digitalWrite(PIN_RELAY1, LOW);
    delay (500);
    digitalWrite(PIN_RELAY1, HIGH);
    Blynk.virtualWrite(vPIN_RELAY1, 0);
  }
  else
  {
    //digitalWrite(PIN_RELAY1, HIGH); // !! Check - is it necessary
  }
}

BLYNK_WRITE(vPIN_RELAY2) //функция, отслеживающая изменение виртуального пина
{
  int pinValue2 = param.asInt(); //переменная текущего состояния виртуального пина
  if (pinValue2 == 1) {
    digitalWrite(PIN_RELAY2, LOW);
    delay (500);
    digitalWrite(PIN_RELAY2, HIGH);
    Blynk.virtualWrite(vPIN_RELAY2, 0);
  }
  else
  {
    //digitalWrite(PIN_RELAY2, HIGH);
  }
}

void OpenedGate ()
{
  lcd.clear();
  int GateSwitchCurrent = digitalRead(PIN_MAGNET);
  //Serial.print("Hall sensor state: ");Serial.println(garageSensorSW); // отображение статуса датчика хола в serial порт, для отладки.
  if (GateSwitchCurrent == HIGH && GateSwitchPrev == LOW) {
    lcd.print(2, 0, "Ворота открыты"); // LCD print, column 2, row 0.
    GateSwitchFirstTime = millis();
    // LOG WHEN
    GateLastOpened = getCurrentDate() + String("   ") + getCurrentTime();
    Blynk.virtualWrite(vPIN_GATE_LAST, GateLastOpened);
    // INDICATOR COLOUR
    Blynk.setProperty(vPIN_GATE_HELD, "color", "#ED9D00");
    // DAILY COUNTER
    GateDailyCounter++;
    Blynk.virtualWrite(vPIN_GATE_COUNTER, GateDailyCounter);

    // START TIMER
    timer.enable(timer2);
    // TERMINAL
    printOutput("Gate Opened >>");
    }
    GateSwitchMillisHeld = (millis() - GateSwitchFirstTime);
    GateSwitchSecsHeld = GateSwitchMillisHeld / 1000;

    if (GateSwitchCurrent == LOW && GateSwitchPrev == HIGH) {
    // INDICATOR COLOUR
    Blynk.setProperty(vPIN_GATE_HELD, "color", "#23C48E");
    // STOP ACTIVE GATE TIMER
    timer.disable(timer2);
    //notificationSent = 0;
    // TABLE
    Blynk.virtualWrite(vPIN_GATE_TABLE, "add", tableIndex1, GateLastOpened, formatTime(GateSwitchMillisHeld) );
    Blynk.virtualWrite(vPIN_GATE_TABLE, "pick", tableIndex1 );

    tableIndex1++;
    //Table counter
    Blynk.virtualWrite(vPIN_TABLE_COUNTER, tableIndex1);
    // TERMINAL
    printOutput(String("Gate Closed << Time: ") + formatTime(GateSwitchMillisHeld));
    }
  GateSwitchPrev = GateSwitchCurrent;
}

void setup() {//основная функция, выполняется один раз при подаче питания на микроконтроллер

  Serial.begin(9600); //открываем серийный порт



  WiFi.mode(WIFI_STA); // режим клиента
  WiFi.hostname(esp_hostname); //
  //WiFi.begin(ssid, pass);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  ArduinoOTA.onError([](ota_error_t error) {
    ESP.restart();
  });
  ArduinoOTA.setHostname(esp_hostname); // !!!!
  ArduinoOTA.begin();

  Blynk.begin(auth, ssid, pass);



  // Pin mode, bootstate
  pinMode(PIN_RELAY1, OUTPUT); //объявляем D4 "выходным" пином
  pinMode(PIN_RELAY2, OUTPUT);
  pinMode(PIN_MAGNET, INPUT_PULLUP);
  //digitalWrite(PIN_MAGNET, LOW);  // CHECK IT
  digitalWrite(PIN_RELAY1, HIGH);
  digitalWrite(PIN_RELAY2, HIGH);


//  timer
  timer.setInterval(2000L, OpenedGate);
  timer.setInterval(30000L, sendUptime);
  //timer.setInterval(30000L, clockDisplay);
  timer1 = timer.setInterval(2000, []() {
    Blynk.virtualWrite(vPIN_CUR_DATE,  getCurrentDate() + String("  ") + getCurrentTime() );
    Blynk.setProperty(vPIN_CUR_DATE, "label", String("WIFI: ") + String(map(WiFi.RSSI(), -105, -40, 0, 100)) + String("% (") + WiFi.RSSI() + String("dB)") + String(" IP: ") + WiFi.localIP().toString());
  });
  timer2 = timer.setInterval(1000, []() {
    Blynk.virtualWrite(vPIN_GATE_HELD, formatTime(GateSwitchMillisHeld));
    if (GateSwitchSecsHeld > 0 && GateSwitchSecsHeld >= notifyDelay) sendNotification();
  });
  timer.disable(timer2);

  timer3 = timer.setInterval(1000, []() {
    if (year() != 1970) {
      today = day();
      Blynk.syncVirtual(vPIN_GATE_COUNTER);
      Blynk.syncVirtual(vPIN_TABLE_COUNTER);
      setSyncInterval(5 * 60);
      printOutput("Sync'd RTC - Interval: 5min");
      timer.disable(timer3);
      timer.setInterval(15 * 60 * 1000, []() {
        if (day() != today) {
          GateDailyCounter = 0;
          Blynk.virtualWrite(vPIN_GATE_COUNTER, 0);
          today = day();
        }
      });
    }
  });
}



void loop()
{
  timer.run();
  Blynk.run();
  ArduinoOTA.handle();
}
