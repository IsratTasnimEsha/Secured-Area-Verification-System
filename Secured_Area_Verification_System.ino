#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

// Keypad configuration
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 3, 4, 5, 6 };
byte colPins[COLS] = { 7, 8, 9, 10 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);

#define NEW_UID \
  { 0xDE, 0xAD, 0xBE, 0xEF }

MFRC522::MIFARE_Key key;

const int irPin = A2;

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo configuration
Servo servo;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  SPI.begin();
  mfrc522.PCD_Init();

  lcd.init();
  lcd.noBacklight();
  lcd.begin(16, 2);

  pinMode(irPin, INPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);

  servo.attach(2);
  servo.write(0);
}

void loop() {
  if (digitalRead(irPin) == LOW) {
    servo.write(80);
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Person");
    lcd.setCursor(0, 1);
    lcd.print("detected");
    digitalWrite(8, HIGH);
  }

  if (digitalRead(irPin) == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Put your card on");
    lcd.setCursor(0, 1);
    lcd.print("the RFID");

    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      delay(50);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Card detected");
      delay(2000);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter code:");

      String input = waitForKeypadInput();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Input: " + input);
      delay(2000);

      if (input.equals("3333")) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Thank you");
        servo.write(0);
        delay(1000);
        digitalWrite(7, HIGH);
        digitalWrite(7, LOW);
        lcd.noBacklight();
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Wrong Password");
        delay(2000);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter code:");

        String input = waitForKeypadInput();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Input: " + input);
        delay(2000);

        if (input.equals("3333")) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Thank you");
          servo.write(0);
          delay(1000);
          digitalWrite(7, HIGH);
          digitalWrite(7, LOW);
          lcd.noBacklight();
        }

        else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Entry blocked");
          delay(1000);
        }
      }

      byte newUid[] = NEW_UID;
      if (mfrc522.MIFARE_SetUid(newUid, (byte)4, true)) {
      }

      mfrc522.PICC_HaltA();
      mfrc522.PCD_Init();  // Reinitialize the RFID module
      if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        delay(50);
      }

      mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
    }
  }
}

String waitForKeypadInput() {
  String keypadInput = "";
  while (keypadInput.length() < 4) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        break;
      } else {
        keypadInput += key;
        lcd.setCursor(keypadInput.length(), 1);
        lcd.print(key);
      }
    }
  }
  return keypadInput;
}