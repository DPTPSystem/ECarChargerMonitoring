# E-Car Charger Monitoring
* E-Car Type2 Charger Cable Power Monitor
* DPTP System 2023-05-28.
* Projekt keletkezése: 2020-01-15.
* ESP32 WEMOS LOLIN32 - Arduino

![DPTP System E-Car Charger Monitoring](https://github.com/DPTPSystem/ECarChargerMonitoring/blob/master/images/type2_charger_check_4.jpg "DPTP System E-Car Charger Monitoring")

# Figyelmeztetés
A 220v nagyon veszéjes ezért csak akkor kezdj bele ilyen tesztelésbe vagy (lehetőleg még akkor sem) utánépítésbe, ha tudod mit csinálsz. Ez a 
a projekt mondjuk nem méri az AC vezetéket, de a pilot jelhez feltétlen szükséges a GND vagy is AC esetében a nulla. Ha valami hibásan van bekötve
vagy csak benézed, nagyon hamar megcsaphat az áram. Fontos, hogy inkább csak nézegesd és ne kezdj el mókolni. Semmilyen felelőséget nem vállalok
az estleges károkért, egészségúgyi problémákért, amelyet okoz vagy okozhat az utánépítés és felhasználás.

# Miért
Sajnos az elektromos autózásaim kapcsán gyakran merült fel bennem, hogy valóban annyival tölt e az adott normál vagy villám töltő, amennyi azon
fel van tüntetve vagy amennyit a saját mérője mutat. Vannak olyan Type2-es oszlopok, melyeken semmi nem jelzi az aktuális töltési sebességet, csak
fel van írva az oszlopra, hogy pl. 22kWh-os.

# Célok
A projekt célja, hogy megfigyeléseket/méréseket végezzek, az utcai elektromos autó töltökön, amelyek Type2-es csatlakozó felülettel vannak ellátva.

# Hardver
A hardver nagyon egyszerű, összeségében egy egyszerű kiegészítő adapter, amely a feszültségszintet hivatott az ESP32-nek optimális tartományra
illeszteni. Összesen egy feszültség osztás találató a nyákon, amely megoldja a megfelelő analog szintet, amelyet képesek vagyunk mérni az ESP-el.
A hardver méri a PWM jelet és a PWM aplitudóját. 

# Web
Azért is választottam az ESP32-őt és az arduinót ehhez a projekthez mert elég jól használható, hogy ha valamilyen webszerver vagy internetes 
hállózathoz akarunk illeszteni valamit. Az elérhető könyvtárak és a kézenfekvő és könnyű beállítások iszonyat energiát takarítanak meg, főként
ha az ember csak tesztelés céljából akar valamit elkészíteni. Nyilván végleges hardver esetében nem használnék arduinot vagy előre elkészített
modult. A projektben a mintavételezett adatokat webes felületen tudjuk nyomonkövetni és képesek vagyunk menteni az adatokat, amelyeket folyamatosan
ment a rendszerer. A tárolás csak ideiglenes, tehát ha nincs elmentve az adatok elvesznek.

![DPTP System E-Car Charger Monitoring](https://github.com/DPTPSystem/ECarChargerMonitoring/blob/master/images/type2_charger_check_11.jpg "DPTP System E-Car Charger Monitoring")

# Nyáktervek
A nyáktervek kétoldalú áramköri rajzolattal készült SMD kivitelben és 0.8mm-es lemezvastagsággal.

![DPTP System E-Car Charger Monitoring](https://github.com/DPTPSystem/ECarChargerMonitoring/blob/master/images/pcb_1.PNG "DPTP System E-Car Charger Monitoring")

![DPTP System E-Car Charger Monitoring](https://github.com/DPTPSystem/ECarChargerMonitoring/blob/master/images/pcb_2.PNG "DPTP System E-Car Charger Monitoring")

# PCB

![DPTP System E-Car Charger Monitoring](https://github.com/DPTPSystem/ECarChargerMonitoring/blob/master/images/t_1.jpg "DPTP System E-Car Charger Monitoring")

![DPTP System E-Car Charger Monitoring](https://github.com/DPTPSystem/ECarChargerMonitoring/blob/master/images/t_2.jpg "DPTP System E-Car Charger Monitoring")

![DPTP System E-Car Charger Monitoring](https://github.com/DPTPSystem/ECarChargerMonitoring/blob/master/images/t_3.jpg "DPTP System E-Car Charger Monitoring")

![DPTP System E-Car Charger Monitoring](https://github.com/DPTPSystem/ECarChargerMonitoring/blob/master/images/t_4.jpg "DPTP System E-Car Charger Monitoring")

![DPTP System E-Car Charger Monitoring](https://github.com/DPTPSystem/ECarChargerMonitoring/blob/master/images/t_5.jpg "DPTP System E-Car Charger Monitoring")

![DPTP System E-Car Charger Monitoring](https://github.com/DPTPSystem/ECarChargerMonitoring/blob/master/images/t_6.jpg "DPTP System E-Car Charger Monitoring")

# Működés
Az eszküzhöz wifin keresztül lehet csatlakozni, ezt követően a fentebb is látható felületet fogjuk kapni, amelyet a böngészőben kell megnyitni.
A program automatikusan észlelni fogja a csatlakozást és kiírja a vételezett adatokat.
```
  // Auto státuszai
  // Status A   (Nincs kapcsolat) +12v
  // Status B   (Jármű észlelése) +9v
  // Status C   (Töltés)          +6v
  // Status D   (ventillátor)     +3v
  // Status E   (Nincs energia)   +0v
  // Status F   (Hiba)            -12v
```

# Program
Nem fontos, de jobb ha saját adatokat adsz meg a következők esetében.
```
// Replace with your network credentials
const char* ssid     = "DPTP System ESP";
const char* password = "12345678";
```
A programon belül mást ne változtass meg csak abban az esetben, ha tudod mit csinálsz.

# Tesztelés

![DPTP System E-Car Charger Monitoring](https://github.com/DPTPSystem/ECarChargerMonitoring/blob/master/images/type2_charger_check_14.jpg "DPTP System E-Car Charger Monitoring")

![DPTP System E-Car Charger Monitoring](https://github.com/DPTPSystem/ECarChargerMonitoring/blob/master/images/type2_charger_check_17.jpg "DPTP System E-Car Charger Monitoring")

# Kapcsolodó videós tartalmak
DPTP System - E Car Type2 Charger Cable Power Monitor

[![DPTP System - E-Car Charger Monitoring](https://img.youtube.com/vi/ucOh74im8FI/0.jpg)](https://www.youtube.com/watch?v=ucOh74im8FI)