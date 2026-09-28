#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <SPI.h>
#include <MFRC522.h>
#include <time.h>

//  WiFi podaci
#define WIFI_SSID "Ajla"
#define WIFI_PASSWORD "18715000"

//  Firebase podaci
#define FIREBASE_HOST "smart-mailbox-c4b1f-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "AIzaSyAe9qOZlkHtTR1KwYqigotvi7GkAMbaMTA"

//  PINOVI
#define SS_PIN D8     // SDA RFID čitača
#define RST_PIN D3    // RST RFID čitača
#define PIR_SENSOR D2 // PIR senzor (GPIO14)
#define LED_PIN D4    // Zelena LED dioda (GPIO5)
#define BUZZER_PIN D1 // Buzzer (GPIO4)

//  RFID čitač
MFRC522 rfid(SS_PIN, RST_PIN);

//  Firebase objekat
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

//  Pohranjeni RFID ID-ovi
const char* OWNER_ID = "4374D7FD";  
const char* POSTMAN_ID = "431387FA"; 

void setup() {
    Serial.begin(115200);
    SPI.begin();
    rfid.PCD_Init();
    
    pinMode(PIR_SENSOR, INPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    digitalWrite(PIR_SENSOR, LOW);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    
    Serial.println("Sistem spreman! PIR senzor aktivan.");

    //  Povezivanje na WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Povezivanje na WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("Povezano!");

    //  Povezivanje na Firebase
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Serial.println("Firebase povezan!");

    //  Povezivanje na NTP server za tačno vrijeme
    configTime(0, 0, "pool.ntp.org");
    Serial.println("Sinhronizacija vremena...");
    delay(2000);
}

void loop() {
    //  Ako PIR senzor detektuje pokret
    if (digitalRead(PIR_SENSOR) == HIGH) {
        Serial.println("Pokret detektovan! Čekam RFID skeniranje...");
        
        unsigned long startTime = millis();
        bool cardScanned = false;

        //  Čekamo 5 sekundi da se skenira kartica
        while (millis() - startTime < 5000) {
            if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
                cardScanned = true;
                handleRFID();
                break;
            }
            delay(100);
        }

        //  Ako nije skenirana kartica → aktivira se alarm
        if (!cardScanned) {
            Serial.println("Nije skenirana kartica! Aktiviram alarm.");
            activateAlarm();
            sendToFirebase("Neovlašten pristup!", "Opasnost");
        }

        delay(5000); //  Dodano: Čekaj 5 sekundi prije nove PIR detekcije
    }
}

//  Funkcija za obradu RFID kartice
void handleRFID() {
    Serial.println("RFID kartica detektovana!");

    //  Čitanje RFID-a
    String tagID = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
        tagID += String(rfid.uid.uidByte[i], HEX);
    }
    tagID.toUpperCase();
    Serial.print("Očitani RFID ID: ");
    Serial.println(tagID);

    String message, eventType;
    
    if (tagID == OWNER_ID) {
        message = "Vlasnik u blizini";
        eventType = "Vlasnik prepoznat";
        Serial.println("Vlasnik prepoznat!");
        activateLedGreen();
    } else if (tagID == POSTMAN_ID) {
        message = "Poštar u blizini";
        eventType = "Poštar prepoznat";
        Serial.println("Poštar prepoznat!");
        activateLedGreen();
    } else {
        message = "Nepoznata kartica";
        eventType = "Opasnost";
        Serial.println("Nepoznata kartica! Aktiviram alarm.");
        activateAlarm();
    }

    //  Slanje podataka u Firebase
    sendToFirebase(message, eventType);

    //  Reset RFID čitača
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

// Funkcija za aktivaciju alarma
void activateAlarm() {
    tone(BUZZER_PIN, 1000);
    delay(3000);
    noTone(BUZZER_PIN);
}

// Funkcija za aktivaciju zelene LED
void activateLedGreen() {
    digitalWrite(LED_PIN, HIGH);
    delay(2000);
    digitalWrite(LED_PIN, LOW);
}



// //  Funkcija za slanje podataka u Firebase
// void sendToFirebase(String message, String eventType) {
//     String formattedTime = getFormattedTime();

//     if (formattedTime == "Neuspješno učitavanje vremena") {
//         Serial.println("Greška: Vrijeme nije sinhronizovano, podaci neće biti poslani!");
//         return;
//     }

//     String path = "/events/event" + String(millis());  
//     FirebaseJson json;
//     json.set("message", message);
//     json.set("type", eventType);
//     json.set("time", formattedTime);

//     Serial.println("Šaljem podatke u Firebase...");
//     if (Firebase.pushJSON(fbdo, path, json)) {
//         Serial.println(" Događaj uspješno poslan u Firebase!");
//     } else {
//         Serial.print("Greška pri slanju: ");
//         Serial.println(fbdo.errorReason());
//     }
// }
// Funkcija za slanje podataka u Firebase
// void sendToFirebase(String message, String eventType) {
//     String formattedTime = getFormattedTime();

//     if (formattedTime == "Neuspješno učitavanje vremena") {
//         Serial.println("Greška: Vrijeme nije sinhronizovano, podaci neće biti poslani!");
//         return;
//     }

//     String path = "/events/event" + String(millis());  // Jedinstveni identifikator događaja
//     FirebaseJson json;
//     json.set("message", message);
//     json.set("type", eventType);
//     json.set("time", formattedTime);
//     json.set("timestamp", millis());  // Dodaj timestamp u milisekundama za tačno sortiranje

//     Serial.println(" Šaljem podatke u Firebase...");
//     if (Firebase.pushJSON(fbdo, path, json)) {
//         Serial.println(" Događaj uspješno poslan u Firebase!");
//     } else {
//         Serial.print(" Greška pri slanju: ");
//         Serial.println(fbdo.errorReason());
//     }
// }

//  Funkcija za dobijanje UNIX vremena (epoch time)
unsigned long getUnixTimestamp() {
    time_t now = time(nullptr);
    return now; // Vraća timestamp u sekundama
}

//  Funkcija za slanje podataka u Firebase
void sendToFirebase(String message, String eventType) {
    String formattedTime = getFormattedTime();

    if (formattedTime == "Neuspješno učitavanje vremena") {
        Serial.println(" Greška: Vrijeme nije sinhronizovano, podaci neće biti poslani!");
        return;
    }

    unsigned long timestamp = getUnixTimestamp();  //  Dobijanje ispravnog UNIX timestamp-a

    String path = "/events/event" + String(timestamp);  // Koristi timestamp kao ID
    FirebaseJson json;
    json.set("message", message);
    json.set("type", eventType);
    json.set("time", formattedTime);
    json.set("timestamp", timestamp);  //  Sada je timestamp ispravan

    Serial.println("Šaljem podatke u Firebase...");
    if (Firebase.pushJSON(fbdo, path, json)) {
        Serial.println("Događaj uspješno poslan u Firebase!");
    } else {
        Serial.print("Greška pri slanju: ");
        Serial.println(fbdo.errorReason());
    }
}


String getFormattedTime() {
    time_t now = time(nullptr);
    struct tm * timeinfo = localtime(&now);
    
    //  Dodaj UTC+1 (ili UTC+2 ako je ljetno računanje vremena)
    timeinfo->tm_hour += 1;  

    if (timeinfo->tm_year < (2016 - 1900)) {
        return "Neuspješno učitavanje vremena";
    }

    char buffer[30];
    strftime(buffer, sizeof(buffer), "%H:%M %d.%m.%Y.", timeinfo);
    return String(buffer);
}  






