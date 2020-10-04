//#define DEBUG_MODE 1  // if enable - debug token
//#define LOCATION 1     // 1 =  Chistova.st ; 2 = Svyatoozerskaya; 3 = Shatura


#if (LOCATION == 1) // Chistova st.
  char ssid[] = "iRoute";
  char pass[] = "0347a8TZ";
#elif (LOCATION == 2)   // Svyatoozerskaya st.
  char ssid[] = "Route";
  char pass[] = "query123";
#elif (LOCATION == 3)
  char ssid[] = "Tenda_Walter";
  char pass[] = "9104650730";
#endif
// Blynk token
#if (DEBUG_MODE == 1)
  char auth[] = "JFCPeotBJkYlD6LLH8onX3jzscoDFM43"; // Токен для отладки
  char esp_hostname[] = "esp_gate_debug";
#else
//char auth[] = "5TXdW24ynSCtm8BaQ2Rqcbni3Tt_C_g3"; // Боевой токен
char auth[] = "XXX"; // Токен для отладки
char esp_hostname[] = "esp_gate";
#endif
