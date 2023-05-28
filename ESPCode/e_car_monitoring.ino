/*********
  DPTP System
  2020-01-15
  SAE J1772
  E-Car Type2 Charger Cable Power Monitor
  ESP32 WEMOS LOLIN32
*********/
  // Auto státuszai
  // Status A   (Nincs kapcsolat) +12v
  // Status B   (Jármű észlelése) +9v
  // Status C   (Töltés)          +6v
  // Status D   (ventillátor)     +3v
  // Status E   (Nincs energia)   +0v
  // Status F   (Hiba)            -12v
  
  // Százalékos számolás egysége 1,5625
  // 6A beállítása 6*1,5625 == 10%

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid     = "DPTP System ESP";
const char* password = "12345678";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";

hw_timer_t * timer = NULL;

// Mérendő feszültség
volatile double INmV = 13000.0;
volatile double INmVD = 4096.0;
volatile double AllCharger = 0;
volatile double AllmA = 0;
volatile int TimeDif = 5; // 0.5% kitőltéssel kompenzálunk

volatile int Counter;
volatile uint64_t TCFel;
volatile uint64_t TCLe;

volatile int AdValueH = 0;
volatile int AdValueL = 0;

volatile int szam = 0;
volatile int freki = 0;
volatile int duty = 0;
volatile double Amper = 0;
volatile double Watt = 0;
volatile int Sec = 0;
volatile int mp = 0;
volatile int perc = 0;
volatile int ora = 0;
volatile int mSCount = 0;
volatile int AdJel = 0;
String Lang = "HU";
String AdatHalmaz;
volatile int AdatHalmazCount = 10; //10mp

/*
 * 3.3v   ->  4096  ->  +13v
 * 1.65v  ->  2048  ->  0v
 * 0v     ->  0     ->  -13v
 */
/*int PilotSignal(int AdVH, int AdVL)
{
  // Megvizsgáljuk, hogy a mért jel pozitív vagy negatív
  if(AdVH > (INmVD/2))  // Ha pozitív
  {
    // Pozitív kifejtés 2048-4096
  }
  else if(AdVH < (INmVD/2))  // Ha negatív
  {
    // Negatív kifejtés 2048-0
  }
  else
  {
    // Ha nulla
  }
}*/

void IRAM_ATTR isr() {
  //digitalWrite(5, !digitalRead(5));
  if(digitalRead(36)==HIGH){
    TCFel = timerRead(timer);
    timerWrite(timer, 0);
    AdValueH += analogRead(32);
  }if(digitalRead(36)==LOW){
    TCLe = timerRead(timer);
    AdValueL += analogRead(32);
  }
  mSCount++;
}

/*void IRAM_ATTR isr_falling() {
  //digitalWrite(5, !digitalRead(5));
  TCLe = timerRead(timer);
  AdValueL = analogRead(32);
}*/

void onTimer(){
  if(Counter==10)
  {
    //Serial.println("Le járt az idő! Timer leállítva!");
    //endTimer();
    Serial.println("Timer időzítéses frissítés...");
    mSCount=2000;
    Counter=0;
  }
  else
  {
    Counter++;
  }
}

void startTimer() {
  timer = timerBegin(1, 80, true); // timer_id = 0; divider=80; countUp = true;
  timerAttachInterrupt(timer, &onTimer, true); // edge = true
  timerAlarmWrite(timer, 1000000, true); //1000 ms
  timerAlarmEnable(timer);
}

void endTimer() {
  timerEnd(timer);
  timer = NULL;
}

void setup() {
  Serial.begin(115200);
  //pinMode(39, INPUT);        // Freki lelfutó él figyelése
  pinMode(36, INPUT);        // Freki felfutó él figyelése
  pinMode(32, INPUT);        // Analog
  pinMode(5, OUTPUT);        // LED
  digitalWrite(5, HIGH);     // LED magasba

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
  
  //attachInterrupt(digitalPinToInterrupt(USER_BT1), bt1IntHandler, FALLING);
  attachInterrupt(36, isr, CHANGE);
  //attachInterrupt(39, isr_falling, FALLING);

  /*
  analogReadResolution(12);             // Sets the sample bits and read resolution, default is 12-bit (0 - 4095), range is 9 - 12 bits
  analogSetWidth(12);                   // Sets the sample bits and read resolution, default is 12-bit (0 - 4095), range is 9 - 12 bits
                                        //  9-bit gives an ADC range of 0-511
                                        // 10-bit gives an ADC range of 0-1023
                                        // 11-bit gives an ADC range of 0-2047
                                        // 12-bit gives an ADC range of 0-4095
  analogSetCycles(8);                   // Set number of cycles per sample, default is 8 and provides an optimal result, range is 1 - 255
  analogSetSamples(1);                  // Set number of samples in the range, default is 1, it has an effect on sensitivity has been multiplied
  analogSetClockDiv(1);                 // Set the divider for the ADC clock, default is 1, range is 1 - 255
  analogSetAttenuation(ADC_11db);       // Sets the input attenuation for ALL ADC inputs, default is ADC_11db, range is ADC_0db, ADC_2_5db, ADC_6db, ADC_11db
  analogSetPinAttenuation(VP,ADC_11db); // Sets the input attenuation, default is ADC_11db, range is ADC_0db, ADC_2_5db, ADC_6db, ADC_11db
                                        // ADC_0db provides no attenuation so IN/OUT = 1 / 1 an input of 3 volts remains at 3 volts before ADC measurement
                                        // ADC_2_5db provides an attenuation so that IN/OUT = 1 / 1.34 an input of 3 volts is reduced to 2.238 volts before ADC measurement
                                        // ADC_6db provides an attenuation so that IN/OUT = 1 / 2 an input of 3 volts is reduced to 1.500 volts before ADC measurement
                                        // ADC_11db provides an attenuation so that IN/OUT = 1 / 3.6 an input of 3 volts is reduced to 0.833 volts before ADC measurement
  adcAttachPin(VP);                     // Attach a pin to ADC (also clears any other analog mode that could be on), returns TRUE/FALSE result 
  adcStart(VP);                         // Starts an ADC conversion on attached pin's bus
  adcBusy(VP);                          // Check if conversion on the pin's ADC bus is currently running, returns TRUE/FALSE result 
  adcEnd(VP);                           // Get the result of the conversion (will wait if it have not finished), returns 16-bit integer result
  */
  adcAttachPin(32);
  startTimer();
}

char uS[20];
char Temp[20];
String s1,s2,s3,s4,s5,s6;
void loop()
{
  
  if(mSCount>=2000)
  {
    AdValueH = AdValueH/(mSCount/2);
    AdValueL = AdValueL/(mSCount/2);
    AdJel = AdValueH-AdValueL;
    Serial.print("Freki: ");
    Serial.print(freki);
    Serial.println(" Hz");
    
    Serial.print("Duty: ");
    duty = (TCFel+TimeDif)/10;
    Amper = (TCFel*0.6)/10;
    Watt = Amper*230;
    AllCharger += Watt / 3600;
    AllmA = (((double)(Amper/3600.0)*AllCharger)/2);

    Serial.print(duty);
    Serial.println(" %");

    Serial.print("Felvett teljesítmény: ");
    Serial.print(AllCharger);
    Serial.println("");
    
    Serial.print("Felvett Áram: ");
    Serial.print(AllmA);
    Serial.println("");
  
    Serial.print("AdValueH: ");
    Serial.print(AdValueH);
    Serial.print(" ");

    Serial.print("AdValueL: ");
    Serial.print(AdValueL);
    Serial.print(" ");
    
    sprintf(Temp, "%i", (int)(AdJel*(INmV/INmVD)));
    Serial.print(Temp);
    Serial.println(" mV");

    if(AdatHalmazCount==0)
    {
      // Analog jel, kitőltés, áram, teljesítmény, össze teljesítmény, összes áram, feszültség
      AdatHalmaz += String(Sec)+","+String(AdJel)+","+String(duty)+","+String(Amper)+","+String(Watt)+","+String(AllCharger)+","+String(AllmA)+","+String(Temp)+"#";
      Serial.println(AdatHalmaz);
      AdatHalmazCount = 10;
    }

    AdatHalmazCount--;
    AdValueH = 0;
    AdValueL = 0;
    freki = TCLe;
    Sec++;
    mSCount=0;
  }
  
  WiFiClient client = server.available();   // Listen for incoming clients
 
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /HU/LED/on") >= 0 || header.indexOf("GET /EN/LED/on") >= 0) {
              Serial.println("GPIO LED on");
              output5State = "on";
              digitalWrite(5, LOW);
            } else if (header.indexOf("GET /HU/LED/off") >= 0 || header.indexOf("GET /EN/LED/off") >= 0) {
              Serial.println("GPIO LED off");
              output5State = "off";
              digitalWrite(5, HIGH);
            } else if (header.indexOf("GET /EN") >= 0) {
              Lang = "EN";
            } else if (header.indexOf("GET /HU") >= 0) {
              Lang = "HU";
            }
            if (header.indexOf("GET /HU/RS") >= 0 || header.indexOf("GET /EN/RS") >= 0) {
                AllCharger = 0;
                AllmA = 0;
                AdValueH = 0;
                AdValueL = 0;
                szam = 0;
                freki = 0;
                duty = 0;
                Amper = 0;
                Watt = 0;
                Sec = 0;
                mp = 0;
                perc = 0;
                ora = 0;
                AdJel = 0;
                AdatHalmaz = "";
            }

            ora = Sec/3600;
            perc = Sec%3600;
            mp = perc%60;
            perc = perc/60;
            sprintf(uS, "%llu", TCFel);

            s1 = "";
            s2 = "";
            s3 = "";
            s4 = "";
            s5 = "";
            s6 = "";
              // Auto státuszai
              // Status A   (Nincs kapcsolat) +12v
              // Status B   (Jármű észlelése) +9v
              // Status C   (Töltés)          +6v
              // Status D   (ventillátor)     +3v
              // Status E   (Nincs energia)   +0v
              // Status F   (Hiba)            -12v
            if(atoi(Temp)>10000) // 10v-nál nagyobb (készenlét)
            {
              s1 = "active";
            }else if(atoi(Temp)<10000 && atoi(Temp)>7500) // Ha 10v-nál kisebb és 7.5-nál nagyobb (jármű észelve)
            {
              s2 = "active";
            }else if(atoi(Temp)<7500 && atoi(Temp)>5000) // Ha 7v-nál kisebb és 5-nál nagyobb
            {
              s3 = "active";
            }else if(atoi(Temp)<5000 && atoi(Temp)>2000) // Ha 5v-nál kisebb és 2-nél nagyobb
            {
              s4 = "active";
            }else if(atoi(Temp)<2000) // Ha 1v-nál kisebb
            {
              s5 = "active";
            }else if(atoi(Temp)<=0) // Ha -12v
            {
              s6 = "active";
            }

            if (s1 == "active" || s2 == "active" || s5 == "active" || s6 == "active") {
                AllCharger = 0;
                AllmA = 0;
                AdValueH = 0;
                AdValueL = 0;
                szam = 0;
                freki = 0;
                duty = 0;
                Amper = 0;
                Watt = 0;
                AdJel = 0;
            }

            // Display the HTML web page
            client.print("<!DOCTYPE html>"
            "<html lang='hu'>"
             "<head>"
                "<title> DPTP System - E-Car Type2 Charger Cable Power Monitor </title>"
                "<meta http-equiv=refresh content=5;URL='//192.168.4.1/"); client.print(Lang); client.println("'>"
                "<meta charset='UTF-8'>"
                "<meta http-equiv='Content-Type' content='text/html; charset=ISO-8859-2' />"
                "<style type='text/css'>"
                  "#keret{position:relative;clear:both;margin:auto auto;border:0px solid red;width:98%;text-align:center;font-family: 'Courier New', Times, serif;font-size:30px;}"
                  "#keret .title{clear:both;border:1px dashed gray;width:auto;padding:5px 5px;margin:5px auto;}"
                  "#keret .jobbkeret{float:right;max-width:49%;width:100%;border:0px solid yellow;margin:5px auto;}"
                  "#keret .balkeret{float:left;max-width:49%;width:100%;border:0px solid blue;margin:5px auto;}"
                  "#keret .mezo{margin:0px auto;width:auto;border:0px solid gray;padding:20px 5px;}"
                  "#keret .fent{border-top:1px dashed gray;border-bottom:1px dashed gray;}"
                  "#keret .jobb{text-align:right;}"
                  "#keret .bal{text-align:left;}"
                  "#keret .szurke{ background-color:#f5f5f5;}"
                  "#state{clear:both;margin:0px auto;border:1px solid #e6e6e6;width:99%;text-align:center;border-radius: 10px;-webkit-border-radius: 10px;-moz-border-radius: 10px;padding:10px 0px;background-color:#f5f5f5;}"
                  "#state .s{clear:both;margin:0px auto;width:auto;border-bottom:1px dotted #e0e0e0;padding:10px 10px;background-color:transparent;color:#dadada;text-shadow:1px 1px 1px white;}"
                  "#state .active{background-color:#e0f0f0;color:gray;}"
                  "p{clear:both;margin:10px auto;}"
                  "a{margin:0px 5px;}"
                  "button:active{background-color: #33cc00;}"
                  "button{font-size:30px;margin:1px auto;border:1px dashed gray;padding:5px 10px;width:48%;height:150px;color:gray;text-shadow:1px 1px 1px white;-webkit-border-radius: 10px;-moz-border-radius: 10px;padding:10px 0px;cursor:pointer;}"
                  ".bactive{background-color:#e0f0f0;color:#d2d2d2;}"
                  ".disabled{color:#d2d2d2;}"
                  "@media(max-width:400px){#keret{ font-size:20px;}button{ font-size:20px; width:45%; height:100px;}}"
                  "textarea{display:none;}"
                "</style>"
                "<script>"
                "function download(filename, text) {"
                  "var dt = new Date();"
                  "var DateTime = (dt.getFullYear()) + '_' + (('0'+dt.getMonth()+1).slice(-2)) + '_' + (('0'+dt.getDate()).slice(-2)) + '_' + (('0'+dt.getHours()).slice(-2)) + '_' + (('0'+dt.getMinutes()).slice(-2));"
                  "var d = document.createElement('a');"
                  "d.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(text));"
                  "d.setAttribute('download', DateTime + '_' + filename);"
                  "d.click();"
                "}"
                "</script>"
              "</head>"
              "<body>"
            
                "<div id='keret'>");
                if (header.indexOf("GET /EN") >= 0) {
                client.println(
                  "<div class='title'>TYPE2 DATAS</div>"
                  "<div class='balkeret'>"
                    "<div class='mezo fent jobb szurke' style='border-top:0px;'>FREQUENCY</div>"
                    "<div class='mezo jobb'>DUTY</div>"
                    "<div class='mezo fent jobb szurke'>CURRENT</div>"
                    "<div class='mezo jobb szurke'>POWER RECEIVED</div>"
                    "<div class='mezo fent jobb'>VOLTAGE</div>"
                    "<div class='mezo jobb szurke'>SIGNAL</div>"
                    "<div class='mezo fent jobb'>CHARGER</div>"
                  "</div>");
                }
                else
                {
                  client.println(
                  "<div class='title'>ADATOK</div>"
                  "<div class='balkeret'>"
                    "<div class='mezo fent jobb szurke' style='border-top:0px;'>FREKVENCIA</div>"
                    "<div class='mezo jobb'>KITŐLTÉS</div>"
                    "<div class='mezo fent jobb szurke'>ÁRAMERŐSSÉG</div>"
                    "<div class='mezo jobb szurke'>VÉTELEZETT</div>"
                    "<div class='mezo fent jobb'>FESZŰLTSÉG</div>"
                    "<div class='mezo jobb szurke'>ANALOG JEL</div>"
                    "<div class='mezo fent jobb'>TÖLTÉS</div>"
                  "</div>");
                }
                  client.println(
                  "<div class='jobbkeret'>"
                    "<div class='mezo fent bal szurke' style='border-top:0px;'>"+String(freki)+" Hz</div>");
                    if (header.indexOf("GET /EN") >= 0) {
                      client.println("<div class='mezo bal'>"+String(duty)+" % (Time: "+uS+" uS)</div>");
                    }
                    else
                    {
                      client.println("<div class='mezo bal'>"+String(duty)+" % (Idő: "+uS+" uS)</div>");
                    }
                    client.println(
                    "<div class='mezo fent bal szurke'>"+String(Amper)+" Ah ("+String(Watt)+" Wh)</div>"
                    "<div class='mezo bal'>"+AllCharger+" W ("+AllmA+" A) ["+((AllCharger/1000)*45)+" Ft]</div>"
                    "<div class='mezo fent bal'>"+Temp+" mV</div>"
                    "<div class='mezo bal szurke'>"+AdJel+"</div>"
                    "<div class='mezo fent bal'>"+ora+":"+perc+":"+mp +"</div>"
                  "</div>"
                  "<div id='state'>");
                  if (header.indexOf("GET /EN") >= 0) {
                  client.println(
                    "<div class='s "+String(s1)+"'>READY BY</div>"
                    "<div class='s "+String(s2)+"'>VEHICLE DETECTION</div>"
                    "<div class='s "+String(s3)+"'>CHARGING</div>"
                    "<div class='s "+String(s4)+"'>VENTILATION</div>"
                    "<div class='s "+String(s5)+"'>NO POWER</div>"
                    "<div class='s "+String(s6)+"'>ERROR</div>");
                  }
                  else
                  {
                    client.println(
                    "<div class='s "+String(s1)+"'>KÉSZENLÉT</div>"
                    "<div class='s "+String(s2)+"'>JÁRMŰ ÉSZLELÉSE</div>"
                    "<div class='s "+String(s3)+"'>TÖLTÉS</div>"
                    "<div class='s "+String(s4)+"'>VENTILLÁTOR</div>"
                    "<div class='s "+String(s5)+"'>NINCS ENERGIA</div>"
                    "<div class='s "+String(s6)+"'>HIBA</div>");
                  }
                    client.println(
                  "</div>"
                  "<div class='title' style='font-size:12px;border:1px dashed gray;'>DPTP System - 2020 E-Car Type2 Charger Cable Power Monitor</div>"
                "</div>"
              "</body>"
            "</html>");

            // Gombok
            if (header.indexOf("GET /EN") >= 0) {
              client.println("<p>");
              client.println("<a href=\"/EN/RS\"><button>RESET</button></a>");
              //client.println("<a href=\"data:application/csv;charset=utf-8,"+AdatHalmaz+"\" download=\"chargerdatas.csv\"><button class='disabled'>SAVE</button></a>");
              client.println("<a href=\"javascript:download('chargerdatas.txt','"+AdatHalmaz+"');\"><button class='disabled'>SAVE</button></a>");
              client.println("</p>");
              client.println("<p>");
              client.println("<a><button class='bactive'>ENGLIS</button></a>");
              client.println("<a href=\"/HU\"><button>HUNGARY</button></a>");
              client.println("</p>");
            }
            else{
              client.println("<p>");
              client.println("<a href=\"/HU/RS\"><button>NULLÁZÁS</button></a>");
              //client.println("<a href=\"data:application/csv;charset=utf-8,"+AdatHalmaz+"\" download=\"toltoadatok.csv\"><button class='disabled'>MENTÉS</button></a>");
              client.println("<a href=\"javascript:download('toltoadatok.txt','"+AdatHalmaz+"');\"><button class='disabled'>MENTÉS</button></a>");
              client.println("</p>");
              client.println("<p>");
              client.println("<a href=\"/EN\"><button>ANGOL</button></a>");
              client.println("<a><button class='bactive'>MAGYAR</button></a>");
              client.println("</p>");
            }
            
            client.println("<textarea id='DataSave'>"+AdatHalmaz+"</textarea>");
            
            if (output5State=="off") {
              client.println("<p><a href=\"/"+Lang+"/LED/on\"><button style=\"background-color:green;\">LED ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/"+Lang+"/LED/off\"><button style=\"background-color:red;\">LED OFF</button></a></p>");
            } 
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
      szam++;
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
