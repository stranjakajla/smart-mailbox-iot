# Smart Mailbox – IoT Project

A smart mailbox built with an **ESP8266** microcontroller, an **RFID reader**, a **PIR motion sensor** and **Firebase**. It recognizes the owner and the postman by their RFID cards, raises an alarm on unauthorized access and logs every event to a real-time web dashboard.

Built as a project for the course **Design and Development of IoT Projects** at the Faculty of Information Technologies, Džemal Bijedić University of Mostar.

![Web dashboard](docs/web-dashboard.png)

## How it works

1. **Presence detection:** the PIR sensor detects a person near the mailbox and the system waits for an RFID card.
2. **Authentication:** the RFID reader scans the card and compares its ID with the stored IDs.
   - **Owner** or **postman** recognized: the green LED turns on and the event is logged.
   - **Unknown card**, or no card scanned within 5 seconds: the buzzer sounds an alarm and the event is logged as a danger.
3. **Cloud logging:** each event is sent to Firebase Realtime Database with its type, message and time (synchronized via an NTP server).
4. **Monitoring:** the web dashboard shows the latest notification and the full event log in real time, with filters by event type (owner, postman, danger).

### State diagram

![FSM diagram](docs/fsm_diagram.jpg)

## Hardware

| Component | Role |
|---|---|
| ESP8266 NodeMCU | Microcontroller, Wi-Fi connection |
| RFID-RC522 reader + cards | Owner and postman authentication |
| PIR sensor HC-SR501 | Motion detection |
| Green LED | Successful authentication signal |
| Buzzer | Alarm for unauthorized access |

### Wiring

| Component | NodeMCU pin |
|---|---|
| RFID SDA (SS) | D8 |
| RFID RST | D3 |
| RFID SCK / MOSI / MISO | D5 / D7 / D6 (hardware SPI) |
| PIR sensor OUT | D2 |
| LED | D4 |
| Buzzer | D1 |

## Software

- **Firmware:** C++ in Arduino IDE (`ESP8266WiFi`, `FirebaseESP8266`, `MFRC522`, `SPI`)
- **Cloud:** Firebase Realtime Database
- **Web app:** HTML, CSS, Bootstrap, JavaScript, Firebase JS SDK

## Project structure

```
smart-mailbox-iot/
├── arduino/smart_mailbox/smart_mailbox.ino   # ESP8266 firmware
├── web/
│   ├── index.html                            # Web dashboard
│   └── smartmailbox.js                       # Firebase connection and event display
└── docs/
    ├── Smart-Mailbox-documentation.docx      # Project documentation (Bosnian)
    ├── fsm_diagram.jpg                       # Finite state machine diagram
    └── web-dashboard.png                     # Dashboard screenshot
```

## Setup

1. Create a Firebase project with a **Realtime Database**.
2. In `arduino/smart_mailbox/smart_mailbox.ino`, enter your Wi-Fi name and password, your Firebase host and your API key. Set `OWNER_ID` and `POSTMAN_ID` to the IDs of your own RFID cards (the IDs are printed to the Serial Monitor when a card is scanned).
3. In Arduino IDE, install the ESP8266 board package and the `Firebase ESP8266 Client` and `MFRC522` libraries, then upload the code to the NodeMCU.
4. In `web/smartmailbox.js`, enter your Firebase web config.
5. Open `web/index.html` in a browser.

## Author

Ajla Stranjak
