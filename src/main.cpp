#include <Arduino.h>
#include <WiFi.h>

#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>

#include "credencials.h"

#include <VFD.h>

const int latchPin_vfd = 2;
const int clockPin_vfd = 1;
const int dataPin_vfd = 44;

const int latchPin_enable = 7;
const int clockPin_enable = 8;
const int dataPin_enable = 9;

VFD vfd(dataPin_vfd, clockPin_vfd, latchPin_vfd, 6);
VFD clon(dataPin_enable, clockPin_enable, latchPin_enable, 1);


AsyncWebServer server(80);

String logBuffer = "";

void addTimestamp(String &msg) {
    unsigned long s = millis() / 1000;
    char ts[12];
    sprintf(ts, "[%02lu:%02lu:%02lu] ", s/3600, (s%3600)/60, s%60);
    msg = String(ts) + msg;
}

void WebLog(String msg) {
    addTimestamp(msg);
    Serial.println(msg);
    logBuffer += "<div class='log-line'>" + msg + "</div>\n";
    if(logBuffer.length() > 5000)
        logBuffer = logBuffer.substring(logBuffer.length() - 5000);
}


void commandParser(String cmd){
  cmd.trim();
    
    if (cmd == "reboot") {
        WebLog("Rebooting...");
        delay(500);
        ESP.restart();

    } else if (cmd.startsWith("scroll-thru ")) {
        String text = cmd.substring(12);
        WebLog("Scrolling: " + text);
        vfd.printScrolling(text, 500);

    } else if (cmd.startsWith("scroll-in ")) {
        String text = cmd.substring(10);
        WebLog("Scrolling in: " + text);
        vfd.printScrollIn(text, 500);

    } else if (cmd.startsWith("scroll-out ")) {
        String text = cmd.substring(11);
        WebLog("Scrolling-out: " + text);
        vfd.printScrollOut(500, SCROLL_LEFT, text);

    }
    
    else if (cmd.startsWith("display ")) {
        String text = cmd.substring(8);
        WebLog("Displaying: " + text);
        vfd.printDisplay(text);

    } else if (cmd == "ip") {
        WebLog("IP: " + WiFi.localIP().toString());

    } else if (cmd == "uptime") {
        WebLog("Uptime: " + String(millis()/1000) + "s");

    } else if (cmd == "off") {
        vfd.offAll();
        WebLog("Display off");

    } else if (cmd == "on") {
        vfd.onAll();
        WebLog("Display on");

    } else {
        WebLog("Unknown command: " + cmd);
    }
}



void setup() {
  Serial.begin(115200);
  
  WiFi.setHostname(HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASS);
  ArduinoOTA.begin();
  

  // in setup():
  server.on("/log", HTTP_GET, [](AsyncWebServerRequest* request){
      String page = F("<!DOCTYPE html><html><head><meta charset='utf-8'>"
      "<style>"
      "*{box-sizing:border-box;margin:0;padding:0}"
      "body{background:#111;font-family:monospace;display:flex;flex-direction:column;height:100vh}"
      ".bar{background:#1a1a1a;border-bottom:1px solid #333;padding:10px 14px;font-size:12px;color:#666;text-align:center}"
      "#log{flex:1;padding:12px 14px;overflow-y:auto}"
      ".log-line{font-size:13px;line-height:1.6;color:#0f0;word-break:break-all}"
      ".input-row{display:flex;border-top:1px solid #333;background:#1a1a1a}"
      "input{flex:1;background:transparent;border:none;outline:none;color:#0f0;font-family:monospace;font-size:13px;padding:10px 14px}"
      "input::placeholder{color:#444}"
      "button{background:transparent;border:none;border-left:1px solid #333;color:#0f0;font-family:monospace;font-size:13px;padding:10px 16px;cursor:pointer}"
      "button:hover{background:#222}"
      "</style>"
      "</head><body>"
      "<div class='bar'>iv-3-clock.local/log</div>"
      "<div id='log'></div>"
      "<div class='input-row'>"
      "<input type='text' id='msg' placeholder='send message to esp32...' onkeydown='if(event.key===\"Enter\")send()'/>"
      "<button onclick='send()'>send</button>"
      "</div>"
      "<script>"
      "function refreshLog(){"
      "  fetch('/logdata').then(r=>r.text()).then(html=>{"
      "    const l=document.getElementById('log');"
      "    const atBottom=l.scrollHeight-l.scrollTop<=l.clientHeight+50;"
      "    l.innerHTML=html;"
      "    if(atBottom) l.scrollTop=l.scrollHeight;"
      "  });"
      "}"
      "function send(){"
      "  const v=document.getElementById('msg').value.trim();"
      "  if(!v) return;"
      "  fetch('/msg?d='+encodeURIComponent(v)).then(()=>{"
      "    document.getElementById('msg').value='';"
      "    refreshLog();"
      "  });"
      "}"
      "refreshLog();"
      "setInterval(refreshLog, 2000);"
      "</script></body></html>");
      request->send(200, "text/html", page);
  });

  server.on("/logdata", HTTP_GET, [](AsyncWebServerRequest* request){
      request->send(200, "text/html", logBuffer);
  });

  server.on("/msg", HTTP_GET, [](AsyncWebServerRequest* request){
      if(request->hasParam("d")){
          String msg = request->getParam("d")->value();
          WebLog("> " + msg);
          commandParser(msg);
      }
      request->send(200, "text/plain", "ok");
  });

  server.begin();

  vfd.begin();
  clon.begin();
  clon.onAll();
  String localIP = WiFi.localIP().toString();

  // vfd.printDisplay("ABCDEF");     // dead code - immediately overwritten
  // String in = "DESIGNED BY ADRicomn"; // dead code - never used
  vfd.printScrolling(localIP, 250);
}


void loop() {
ArduinoOTA.handle();


  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    WebLog("Clock Heartbeat... System Uptime: " + String(millis()/1000) + "s");
      
  }

vfd.update();
}

//1- right bottom
//2- semicolon
//3- right top
//4- top
//5- middle
//6- left top
//7- left bottom
//8- bottom
//byte data =B11111111;
//byte data_z = B00000000;
//byte data_0 =B10110111; 
//byte data_9 =B10111101; 
//byte data_8 =B10111111;
//byte data_7 =B10110000;
//byte data_6 =B10011111;
//byte data_5 =B10011101;
//byte data_4 =B10101100;
//byte data_3 =B10111001;
//byte data_2 =B00111011;
//byte data_1 =B10100000;


// uint8_t customVfdTranslationTable[]{
//     'A', B11111111,
//     'B', B10011111,
//     'C', B11100111,
//     'D', B11111001,
//     'E', B01111110,
//     'F', B00011000
// };


  //  vfd.setCustomTranslationTable(customVfdTranslationTable); 


  //   byte pattern[6] = {
  //     B10110111,
  //     B00001000,
  //     B00010111,
  //     B10110001,
  //     B00001000,
  //     B10110111
  //   };
  //   vfd.printDisplayRaw(pattern);
  //   delay(5000);