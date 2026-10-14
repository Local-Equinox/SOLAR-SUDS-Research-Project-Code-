#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Set I2C address to 0x27

const int irSensorPin = 2;
const int relayPin = 3;

void setup() {
  pinMode(irSensorPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Keep active-low relay OFF by default
  
  lcd.init();
  lcd.backlight();
  displayReadyMessage();
}

void loop() {
  int sensorState = digitalRead(irSensorPin);

  // If hand detected (Active-LOW)
  if (sensorState == LOW) {
    lcd.clear();
    lcd.print("Dispensing Soap");
    
    digitalWrite(relayPin, LOW); // Turn pump ON
    
    unsigned long startTime = millis();
    int highCount = 0; // Counter for continuous HIGH readings (hand removed)
    
    // Dispense loop (Max 1.5 seconds)
    while (millis() - startTime < 1500) {
      if (digitalRead(irSensorPin) == HIGH) {
        highCount++;
      } else {
        highCount = 0; // Reset counter if hand is still detected (LOW)
      }
      
      // Require 15 consecutive HIGH checks (~150ms) to confirm true hand removal
      if (highCount >= 15) {
        break;
      }
      
      delay(10); // Sample every 10ms
    }
    
    digitalWrite(relayPin, HIGH); // Turn pump OFF
    
    // Anti-Abuse Cooldown Loop
    for (int i = 3; i > 0; i--) {
      lcd.clear();
      lcd.print("Please Wait...");
      lcd.setCursor(0, 1);
      lcd.print("Lockout: ");
      lcd.print(i);
      lcd.print("s");
      delay(1000);
    }
    
    displayReadyMessage();
  }
}

void displayReadyMessage() {
  lcd.clear();
  lcd.print("SOLAR-SUD READY");
  lcd.setCursor(0, 1);
  lcd.print("Place Hand Below");
}