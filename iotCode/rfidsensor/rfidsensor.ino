#include <SPI.h>
#include <MFRC522.h>

// Custom SPI pin assignments
#define SS_PIN   21
#define RST_PIN  22
#define SCK_PIN  25
#define MOSI_PIN 26
#define MISO_PIN 27

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

// Init array that will store new NUID 
byte nuidPICC[4];

byte unlockCard[4] = {0x65, 0x74, 0x4D, 0x05};

void setup() { 
  Serial.begin(115200);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();
  Serial.println("Scan a card");
}
 
void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  if (memcmp(rfid.uid.uidByte, unlockCard, 4) == 0) {
    Serial.println("Card Matches");
  }
  else {
    Serial.println("Crad does not match");
  }
  delay(1000);

/*
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (memcmp(rfid.uid.uidByte, nuidPICC, 4) != 0) {
    Serial.println(F("A new card has been detected."));
    memcpy(nuidPICC, rfid.uid.uidByte, 4);

    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  } //else {
    //Serial.println(F("Card read previously."));
  //}

  // Halt PICC
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  */
}

/*

//Helper routine to dump a byte array as hex values to Serial. 
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}


//Helper routine to dump a byte array as dec values to Serial.
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(' ');
    Serial.print(buffer[i], DEC);
  }
}
*/
