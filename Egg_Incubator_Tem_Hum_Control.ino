#include <LiquidCrystal.h>  // Include the LiquidCrystal library
#include <TimerOne.h>       // Include the TimerOne library
#include <DHT.h>            // Include the DHT library

// Create a LiquidCrystal object with pin numbers for RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(11, 10, 6, 5, 4, 3);

// Define pin numbers for buttons
#define UP_BUTTON_PIN 15
#define DOWN_BUTTON_PIN 16
#define SET_BUTTON_PIN 17

// Define pin numbers for components
#define Heater 2
#define Cooler 8
#define Fan 12
#define Buzzer 7
#define Turning 9
#define Humidifier 14

// Define macros for component control
#define Heater_ON digitalWrite(Heater, HIGH);          // Turn on Heater
#define Heater_OFF digitalWrite(Heater, LOW);          // Turn off Heater
#define Cooler_ON digitalWrite(Cooler, HIGH);          // Turn on Cooler
#define Cooler_OFF digitalWrite(Cooler, LOW);          // Turn off Cooler
#define Fan_ON digitalWrite(Fan, HIGH);                // Turn on Fan
#define Fan_OFF digitalWrite(Fan, LOW);                // Turn off Fan
#define Turning_ON digitalWrite(Turning, HIGH);        // Turn on Turning
#define Turning_OFF digitalWrite(Turning, LOW);        // Turn off Turning
#define Buzzer_ON digitalWrite(Buzzer, HIGH);          // Turn on Buzzer
#define Buzzer_OFF digitalWrite(Buzzer, LOW);          // Turn off Buzzer
#define Humidifier_ON digitalWrite(Humidifier, HIGH);  // Turn on Humidifier
#define Humidifier_OFF digitalWrite(Humidifier, LOW);  // Turn off Humidifier

#define DHTPIN 13          // Define pin number for DHT sensor
#define DHTTYPE DHT11      // Define DHT sensor type
DHT dht(DHTPIN, DHTTYPE);  // Create a DHT object

float Temperature = 0;           // Variable to store temperature
float Humidity = 0;              // Variable to store humidity
float Setpoint = 0;              // Variable for temperature setpoint
float HumiditySetpoint = 0;      // Variable for humidity setpoint
unsigned int timer_counter = 0;  // Timer counter
unsigned int counter1 = 0;       // Counter

void setup() {
  pinMode(Heater, OUTPUT);      // Set Heater pin as output
  pinMode(Cooler, OUTPUT);      // Set Cooler pin as output
  pinMode(Fan, OUTPUT);         // Set Fan pin as output
  pinMode(Buzzer, OUTPUT);      // Set Buzzer pin as output
  pinMode(Turning, OUTPUT);     // Set Turning pin as output
  pinMode(Humidifier, OUTPUT);  // Set Humidifier pin as output

  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);    // Set UP_BUTTON_PIN as input with pull-up resistor
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);  // Set DOWN_BUTTON_PIN as input with pull-up resistor
  pinMode(SET_BUTTON_PIN, INPUT_PULLUP);   // Set SET_BUTTON_PIN as input with pull-up resistor

  lcd.begin(16, 2);                  // Initialize LCD with 16 columns and 2 rows
  lcd.setCursor(0, 0);               // Set cursor position to the first column of the first row
  lcd.print("Egg Incubator");        // Display message on LCD
  Buzzer_ON;                         // Turn on Buzzer
  delay(100);                        // Delay
  Buzzer_OFF;                        // Turn off Buzzer
  dht.begin();                       // Initialize DHT sensor
  delay(2000);                       // Delay
  lcd.clear();                       // Clear LCD
  Timer1.initialize(100000);         // Initialize Timer1 with 100000 microseconds
  Timer1.attachInterrupt(timerIsr);  // Attach interrupt to Timer1
  lcd.clear();                       // Clear LCD
}

void loop() {
  Read_temp();                               // Read temperature
  Read_setpoint();                           // Read temperature setpoint
  PrintTemp();                               // Print temperature and setpoint
  Read_DHT11();                              // Read humidity from DHT sensor
  Temperature_control();                     // Control temperature
  Humidity_control();                        // Control humidity
  if (digitalRead(SET_BUTTON_PIN) == LOW) {  // If SET button is pressed
    SetTemperature();                        // Set temperature setpoint
    SetHumidity();                           // Set humidity setpoint
  }
}

void timerIsr() {
  timer_counter++;                                       // Increment timer counter
  if (timer_counter > 16200 && timer_counter < 16500) {  // Check timer counter range
    Turning_ON;                                          // Turn on Turning
  } else {
    Turning_OFF;  // Turn off Turning
  }

  if (timer_counter > 16500) {  // Check timer counter value
    timer_counter = 0;          // Reset timer counter
  }
}

void Temperature_control() {
  // Temperature control logic
  // Adjust Heater, Cooler, Fan, and Buzzer based on temperature

  if (Temperature >= Setpoint + 0.5) {
    Heater_OFF;
    counter1++;
    if (counter1 > 20) {
      counter1 = 0;
      if (Fan == HIGH) {
        Fan_OFF;
      } else if (Fan == LOW) {
        Fan_ON;
      }
    }

    if (Temperature >= Setpoint + 1.5) {
      Cooler_ON;
    } else if (Temperature <= Setpoint + 0.5) {
      Cooler_OFF;
    }

    if (Temperature >= Setpoint + 2.0) {
      Buzzer_ON;
    } else {
      Buzzer_OFF;
    }
  } else if (Temperature <= Setpoint - 0.5) {
    Heater_ON;
    Fan_ON;
    Cooler_OFF;
    if (Temperature <= Setpoint - 1.0) {
      Buzzer_ON;
    } else {
      Buzzer_OFF;
    }
  }
}


void Humidity_control() {
  // Humidity control logic
  // Adjust Humidifier based on humidity
  if (Humidity < 50) {
    Humidifier_ON;
  } else if (Humidity >= 60) {
    Humidifier_OFF;
  }
}

void SetTemperature() {
  lcd.clear();                   // Clear LCD
  lcd.setCursor(0, 0);           // Set cursor to first column of first row
  lcd.print("Set Temperature");  // Display message
  delay(500);                    // Delay

  while (true) {
    lcd.setCursor(0, 1);   // Set cursor to first column of second row
    lcd.print("T: ");      // Display message
    lcd.print(Setpoint);   // Display temperature setpoint
    lcd.print((char)223);  // Display degree symbol
    lcd.print("C  ");      // Display Celsius

    int upButtonState = digitalRead(UP_BUTTON_PIN);      // Read state of UP button
    int downButtonState = digitalRead(DOWN_BUTTON_PIN);  // Read state of DOWN button

    if (upButtonState == LOW) {  // If UP button is pressed
      Setpoint++;                // Increase temperature setpoint
      delay(200);                // Delay
    }

    if (downButtonState == LOW) {  // If DOWN button is pressed
      Setpoint--;                  // Decrease temperature setpoint
      delay(200);                  // Delay
    }

    if (digitalRead(SET_BUTTON_PIN) == LOW) {  // If SET button is pressed
      break;                                   // Exit the loop
    }
  }
}

void SetHumidity() {
  lcd.clear();                // Clear LCD
  lcd.setCursor(0, 0);        // Set cursor to first column of first row
  lcd.print("Set Humidity");  // Display message
  delay(500);                 // Delay

  while (true) {
    lcd.setCursor(0, 1);          // Set cursor to first column of second row
    lcd.print("H: ");             // Display message
    lcd.print(HumiditySetpoint);  // Display humidity setpoint
    lcd.print("%  ");             // Display percentage

    int upButtonState = digitalRead(UP_BUTTON_PIN);      // Read state of UP button
    int downButtonState = digitalRead(DOWN_BUTTON_PIN);  // Read state of DOWN button

    if (upButtonState == LOW) {  // If UP button is pressed
      HumiditySetpoint++;        // Increase humidity setpoint
      delay(200);                // Delay
    }

    if (downButtonState == LOW) {  // If DOWN button is pressed
      HumiditySetpoint--;          // Decrease humidity setpoint
      delay(200);                  // Delay
    }

    if (digitalRead(SET_BUTTON_PIN) == LOW) {  // If SET button is pressed
      break;                                   // Exit the loop
    }
  }
}

void Read_temp() {
  // Read temperature from analog pin or sensor
  Temperature = 0;
  for (int i = 0; i < 30; i++) {
    Temperature += analogRead(A1) * 0.5;
    delay(10);
  }
  Temperature /= 30;
}

void Read_setpoint() {
  // Read temperature setpoint from analog pin or sensor
  Setpoint = 0;
  for (int i = 0; i < 30; i++) {
    Setpoint += analogRead(A0) / 25.6;
    delay(2);
  }
  Setpoint /= 30;
}

void PrintTemp() {
  // Print temperature and setpoint on LCD
  lcd.setCursor(9, 0);
  lcd.write("S:");
  lcd.print(Setpoint, 1);

  lcd.setCursor(0, 0);
  lcd.write("T:");
  lcd.print(Temperature - 1, 1);
  lcd.write((char)223);
  lcd.write("C");
}

void Read_DHT11() {
  // Read humidity from DHT sensor
  Humidity = dht.readHumidity();

  lcd.setCursor(0, 1);
  lcd.write("H:");
  lcd.print(Humidity);
  lcd.write("%");

  lcd.setCursor(9, 1);
  lcd.write("S:");
  lcd.print("86%");
}