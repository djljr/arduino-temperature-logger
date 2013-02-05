#include <LiquidCrystal.h>

// Data wire is plugged into pin 2 on the Arduino
#define TEMP_1 A0

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

byte degrees_char[8] = {
  0b01100,
  0b10010,
  0b10010,
  0b01100,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

uint8_t degrees = 0;

void setup(void)
{
  lcd.createChar(degrees, degrees_char);
  lcd.begin(16, 2);
  lcd.print("Starting Up...");
  // start serial port
  Serial.begin(9600);
  // connect AREF to 3.3V and use that as VCC, less noisy!
  analogReference(EXTERNAL);
  // Start up the library
  lcd.setCursor(0,1);
  lcd.print("Started");
  delay(2500);
}

// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// the value of the 'other' resistor
#define SERIESRESISTOR 10000
// resistance at 25 degrees C
#define THERMISTORNOMINAL 55000 
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25 
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950

int samples[NUMSAMPLES];

void loop(void)
{ 
  uint8_t i;
  float average;
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
    samples[i] = analogRead(TEMP_1);
    delay(10);
  }
  
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
    average += samples[i];
  }
  average /= NUMSAMPLES;
  
  lcd.clear();
  lcd.print("A0:");
  lcd.print((int)average);

  lcd.setCursor(0, 1);
  lcd.print("2:");
  
  average = (1023 / average) - 1;
  average = SERIESRESISTOR / average;
  
  float steinhart = average / THERMISTORNOMINAL;
  steinhart = log(steinhart);
  steinhart /= BCOEFFICIENT;
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); 
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;
  
  lcd.print(steinhart);
  lcd.write(degrees);
  lcd.print("C");
  
  delay(1000);
}

