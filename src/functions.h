
/*
  Date and time functions
*/
String getCurrentTime() {
  String extraZeroH, extraZeroM, extraZeroS;
  if (hour() < 10) extraZeroH = '0';
  if (minute() < 10) extraZeroM = '0';
  if (second() < 10) extraZeroS = '0';
  return String(extraZeroH + hour()) + ':' + extraZeroM + minute() + ':' + extraZeroS + second();
}

String getCurrentDate() {
  return(String(day())) + "-" + monthShortStr(month()) + "-" + year();
}

void printTimeDate() {
  terminal.println("-----------------------");
  terminal.println( getCurrentDate() + String(" | ") + getCurrentTime() );
}

void sendUptime ()
{
  uptime::calculateUptime();
  String uptime_format = (String)(uptime::getDays()) + "d:" + (String)(uptime::getHours()  ) + "h:" + (String)(uptime::getMinutes()) + "m" ;
  Blynk.virtualWrite(vPIN_UPTIME,  uptime_format);
}
BLYNK_CONNECTED() {
  rtc.begin();
  setSyncInterval(10 * 60);
  Blynk.virtualWrite(vPIN_GATE_STATE, analogRead(HALL_SENSOR) > MIN_THRESHOLD ? 1 : 0);
}

/*
  printOutput() - easy print to terminal with date function
*/
void printOutput(String a) {
  printTimeDate();
  terminal.println(a);
  terminal.flush();
}




/*
  formatTime() - Format millis in to MM:SS
*/
String formatTime(long milliSeconds) {
  long hours = 0, mins = 0, secs = 0;
  String returned, secs_o, mins_o, hours_o;
  secs = milliSeconds / 1000;
  mins = secs / 60;
  hours = mins / 60;
  secs = secs - (mins * 60);
  mins = mins - (hours * 60);
  if (hours) returned += hours + String("h ");
  if (secs < 10 && (mins || hours)) secs_o = "0";
  if (mins < 10 && hours) mins_o = "0";
  if (mins || hours) returned += mins_o + mins + String("m ");

  returned += secs_o + secs + String("s");
  return returned;
}

// Notify
void sendNotification() {
  if (notifyDelay && !notificationSent) {
    Blynk.notify(String("Внимание ! Ворота открыты уже: \n") + formatTime(GateSwitchMillisHeld) + "\n _____________\n\n\nВорота были открыты: \n" + String(GateLastOpened));
    printOutput(String("Notified # Gate Held: ") + formatTime(GateSwitchMillisHeld));
    notificationSent = 1;
    timerNotify = timer.setTimeout(((notifyDelay * 1000) - 1000), []() {
      notificationSent = 0;
      printOutput("Notified # Reset Flag");
    });
    //Blynk.sms("Alert! " + GateLastOpened + String(" // Front Gate has been left open for ") + formatTime(GateSwitchMillisHeld);
    //Blynk.notify("NOTIFY: Alert, Front Gate has been left open!");
  }
}
