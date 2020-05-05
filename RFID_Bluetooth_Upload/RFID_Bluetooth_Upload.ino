#include <SPI.h> // SPI library
#include <MFRC522.h> // RFID library (https://github.com/miguelbalboa/rfid)
#include <ESP8266WiFi.h>
constexpr uint8_t RST_PIN = 5; // (D1) RST in module
constexpr uint8_t SS_PIN = 16; // (D0) SDA in module

const char* ssid     = "SSID";
const char* password = "PASSWORD";
String cardid = "";
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

WiFiServer server(80); // Set web server port number to 80

void setup() {
  delay(3000);
  Serial.begin(9600);   // Initiate a serial communication
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  
  SPI.begin();      // Initiate SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println("------------------------------------\n");
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients
  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<meta http-equiv='refresh' content='1'>");
  // CSS to style the on/off buttons 
  // Feel free to change the background-color and font-size attributes to fit your preferences
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}</style></head>");

  // Web Page Heading
  client.println("<body><h1>ESP8266 Web Server</h1>");
  client.println("<p>Card ID : </p>");
  
  if (mfrc522.PICC_IsNewCardPresent()) { // Look for new cards
    if (mfrc522.PICC_ReadCardSerial()) { // Select one of the cards
      Serial.println("ID Card Get");
      Serial.print("Card ID : ");
      String content = "";

      for (byte i = 0; i < mfrc522.uid.size; i++) {
        // Dump a byte array as hex values to Serial
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);

        // Dump a byte array as hex values to String
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX)); //
      }

      content.toUpperCase(); // Make string become UPPERCASE
      mfrc522.PICC_HaltA();
      if (content.substring(1) == "YOUR CARD ID") { // Change UID of the card that you want to give access, for example A0 B1 C2 D3
        // Do something with access granted
        Serial.println();
        Serial.println("---Access Granted---");
      }

      else {
        // Do something with access denied
        Serial.println();
        Serial.println("---Access Denied---");
      }
      cardid = content.substring(1);
      Serial.println("");
    }
  }
  client.println("<p>"+ cardid + "</p>");
}
