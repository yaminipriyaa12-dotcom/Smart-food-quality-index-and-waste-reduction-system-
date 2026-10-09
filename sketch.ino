#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT22

#define MQ2_PIN A0

#define MEAT_BTN 3
#define MILK_BTN 4
#define FRUITS_BTN 5
#define VEG_BTN 6

#define GREEN_LED 8
#define YELLOW_LED 9
#define RED_LED 10
#define BUZZER 11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

  Serial.begin(9600);

  dht.begin();

  lcd.init();
  lcd.backlight();

  pinMode(MEAT_BTN, INPUT_PULLUP);
  pinMode(MILK_BTN, INPUT_PULLUP);
  pinMode(FRUITS_BTN, INPUT_PULLUP);
  pinMode(VEG_BTN, INPUT_PULLUP);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Initial screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FOOD DETECTOR");

  lcd.setCursor(0, 1);
  lcd.print("SELECT FOOD");
}


void loop() {

  // ==========================================
  // WAIT FOR A BUTTON PRESS
  // ==========================================

  if (digitalRead(MEAT_BTN) == LOW) {

    detectFood("MEAT");

  }

  else if (digitalRead(MILK_BTN) == LOW) {

    detectFood("MILK");

  }

  else if (digitalRead(FRUITS_BTN) == LOW) {

    detectFood("FRUITS");

  }

  else if (digitalRead(VEG_BTN) == LOW) {

    detectFood("VEGETABLES");

  }

  // Nothing happens if no button is pressed
}


// ==========================================
// FOOD DETECTION FUNCTION
// ==========================================

void detectFood(String foodType) {

  // Wait for button release
  delay(300);

  // ==========================================
  // READ SENSORS ONLY AFTER BUTTON PRESS
  // ==========================================

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int gasValue = analogRead(MQ2_PIN);


  // ==========================================
  // CHECK DHT SENSOR
  // ==========================================

  if (isnan(temperature) || isnan(humidity)) {

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("DHT SENSOR");
    
    lcd.setCursor(0, 1);
    lcd.print("ERROR");

    delay(2000);

    showHomeScreen();

    return;
  }


  // ==========================================
  // CALCULATE FQI
  // ==========================================

  float tempPenalty = 0;
  float humidityPenalty = 0;
  float gasPenalty = 0;


  // Temperature penalty

  if (temperature > 30) {

    tempPenalty = 30;

  }

  else if (temperature > 25) {

    tempPenalty = 15;

  }


  // Humidity penalty

  if (humidity > 80) {

    humidityPenalty = 25;

  }

  else if (humidity > 65) {

    humidityPenalty = 10;

  }


  // Gas penalty

  if (gasValue > 600) {

    gasPenalty = 35;

  }

  else if (gasValue > 400) {

    gasPenalty = 20;

  }

  else if (gasValue > 250) {

    gasPenalty = 10;

  }


  // FQI

  float fqi = 100 -
              (tempPenalty +
               humidityPenalty +
               gasPenalty);


  if (fqi < 0) {

    fqi = 0;

  }


  // ==========================================
  // DETERMINE STATUS
  // ==========================================

  String status;


  if (fqi >= 70) {

    status = "GOOD";

    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

  }

  else if (fqi >= 40) {

    status = "WARNING";

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

  }

  else {

    status = "SPOILED";

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);

    digitalWrite(BUZZER, HIGH);

    delay(500);

    digitalWrite(BUZZER, LOW);
  }


  // ==========================================
  // LCD PAGE 1
  // FOOD + TEMPERATURE
  // ==========================================

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(foodType);

  lcd.setCursor(0, 1);
  lcd.print("Temp:");
  lcd.print(temperature, 1);
  lcd.print("C");

  delay(2000);


  // ==========================================
  // LCD PAGE 2
  // HUMIDITY + GAS
  // ==========================================

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Hum:");
  lcd.print(humidity, 1);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Gas:");
  lcd.print(gasValue);

  delay(2000);


  // ==========================================
  // LCD PAGE 3
  // FQI + STATUS
  // ==========================================

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("FQI:");
  lcd.print(fqi, 0);

  lcd.setCursor(0, 1);
  lcd.print(status);

  delay(3000);


  // ==========================================
  // SERIAL MONITOR
  // ==========================================

  Serial.println("----------------------");

  Serial.print("Food: ");
  Serial.println(foodType);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Gas: ");
  Serial.println(gasValue);

  Serial.print("FQI: ");
  Serial.println(fqi);

  Serial.print("Status: ");
  Serial.println(status);

  Serial.println("----------------------");


  // ==========================================
  // RETURN TO WAITING SCREEN
  // ==========================================

  showHomeScreen();
}


// ==========================================
// HOME SCREEN
// ==========================================

void showHomeScreen() {

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("FOOD DETECTOR");

  lcd.setCursor(0, 1);
  lcd.print("SELECT FOOD");
}