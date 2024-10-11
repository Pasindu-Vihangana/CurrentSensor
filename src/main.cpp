#include <Arduino.h>
#include <LiquidCrystal.h>

#define SAMPLING_TIME 200
#define SAMPLE_COUNT 500
#define CALIBRATE_BUTTON 14

const int rs = 11, en = 9, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

float sensitivity = 1000.0 / 185.0; // 1000mA per 185mV
float Vref = 0.00;

static unsigned long lastUpdateTime = 0;

float get_Vref()
{
  long int sensorValue = 0;

  for (int i = 0; i < SAMPLE_COUNT; i++)
  {
    sensorValue += analogRead(A0);

    // wait 2 milliseconds before the next loop
    delay(2);
  }

  sensorValue = sensorValue / SAMPLE_COUNT;

  float unitValue = 5.0 / 1024.0 * 1000;
  float voltage = unitValue * sensorValue;

  return voltage;
}

float get_current()
{
  long int sensorValue = 0;

  for (int i = 0; i < SAMPLE_COUNT; i++)
  {
    sensorValue += analogRead(A0);

    // wait 2 milliseconds before the next loop
    delay(2);
  }

  sensorValue = sensorValue / SAMPLE_COUNT;

  // The on-board ADC is 10-bits
  // Different power supply will lead to different reference sources
  // example: 2^10 = 1024 -> 5V / 1024 ~= 4.88mV
  //          unitValue= 5.0 / 1024.0*1000 ;
  float unitValue = 5.0 / 1024.0 * 1000;
  float voltage = unitValue * sensorValue;

  // Calculate the corresponding current
  float current = (voltage - Vref) * sensitivity;

  // display voltage (mV)
  // This voltage is the pin voltage corresponding to the current
  voltage = unitValue * sensorValue - Vref;

  return current;
}

void calibrateSensor()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   CURRENT SENSOR   ");
  lcd.setCursor(0, 2);
  lcd.print("CALIBRATION RUNNING");

  Vref = get_Vref(); // Keep waiting
  delay(1000);       // Display message for 2 seconds

  // Clear the LCD and display calibration complete message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   CURRENT SENSOR   ");
  lcd.setCursor(0, 2);
  lcd.print("CALIBRATION COMPLETE");
  delay(2000); // Display message for 2 seconds

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   CURRENT SENSOR   ");
  lcd.setCursor(0, 2);
  lcd.print("resolution: 26.4mA");
  delay(2000); // Display message for 2 seconds
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(CALIBRATE_BUTTON, INPUT_PULLUP);
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("   CURRENT SENSOR   ");
  lcd.setCursor(0, 2);
  lcd.print("TURN OFF PWM &");
  lcd.setCursor(0, 3);
  lcd.print("PRESS CALIBRATE!");

  // Wait for calibration button press
  while (digitalRead(CALIBRATE_BUTTON) == HIGH)
  {
  }
  calibrateSensor();
}

void loop()
{
  unsigned long currentTime = millis();

  if (currentTime - lastUpdateTime >= SAMPLING_TIME)
  {
    // Read the value 500 times:
    float current = get_current();

    // Limit current between 0 and 5000 mA
    current = constrain(current, 0, 5000);

    // display current (mA)
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   CURRENT SENSOR   ");
    lcd.setCursor(0, 2);
    lcd.print("CURRENT: ");
    lcd.setCursor(9, 2);
    lcd.print(current, 1);
    lcd.print("mA");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

    lastUpdateTime = currentTime;
  }

  if (digitalRead(CALIBRATE_BUTTON) == LOW)
  {
    calibrateSensor();
  }
}
