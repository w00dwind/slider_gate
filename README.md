# Slider gate opener


* A little project for manage slider gates in private house.
* Based on nodeMCU board and Blynk IoT platform.
# screenshot
![alt text](/img/blynk_screen.jpg)
## Logic:
In this project i connect nodemcu with relay module to logic board of FAAC 741 drive. For short open gate you have to close 2 and 8 contacts on board, for full open gate - 1 and 8. (just close it for 1 seconds or less).
Also FAAC logic board uses two magnets and hall sensor to know when drive have to stop at closed point and open point. I connected hall effect sensor (KY-024) to nodemcu for know when the gate is open and when close.
Also:
* Journal of open/close
* Reminder when gates open more than n minutes (can be disable in blynk app)

You can use blynk HTTPs REST API to manage virtual pins in blynk apps. You just need to have public IP address (if you use blynk local server. If you prefer blynk cloud platform, please check documentation on their website)
For example:
to push the button "V1" on blynk local server:
https://YOUR_DOMAIN_WITH_BLYNK_LOCAL_SERVER.com/YOUR_TOKEN/PIN?value=1

You can add this GET request in ios shortcuts and now can ask Siri to open or close gate.

## Components:
* NodeMCU V3 (or any other arduino like boards with WiFi)
* two channel relay module (For most cases one channel is enough, but some)
* hall effect sensor module KY-024


# FAAC board scheme
![alt text](/img/faac_board.png)
