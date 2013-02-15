#include <LiquidCrystal.h>

// Data wire is plugged into pin 2 on the Arduino
#define TEMP_1 A0
#define TEMP_2 A1

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
#define SERIESRESISTOR_0 1000000
#define SERIESRESISTOR_1 10000
// resistance at 25 degrees C

#define THERMISTORNOMINAL_0_low 1050000
#define THERMISTORNOMINAL_0 800000
#define THERMISTORNOMINAL_1 55000

// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25 
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950

int samples1[NUMSAMPLES];
int samples2[NUMSAMPLES];

float roundHalf(float value) {
  return ((int)((2 * value) + 0.5)) / 2.0;  
}

float toF(float value) {
  return (9.0/5.0) * value + 32;
}

float convert_temp(int samples[], float resistence_25_c, float series_resistor) {
  uint8_t i;
  float average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
    average += samples[i];
  }
  average /= NUMSAMPLES;

  average = (1023 / average) - 1;
  average = series_resistor / average;

  float steinhart = average / resistence_25_c;
  steinhart = log(steinhart);
  steinhart /= BCOEFFICIENT;
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); 
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;

  //return roundHalf(toF(steinhart));
  return toF(steinhart);
  //return average;
  //return steinhart;
}

void loop(void)
{ 
  uint8_t i;
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
    samples1[i] = analogRead(TEMP_1);
    samples2[i] = analogRead(TEMP_2);
    delay(10);
  }

  lcd.clear();
  lcd.print("1:");
  lcd.print(convert_temp(samples1, THERMISTORNOMINAL_0, SERIESRESISTOR_0), 1);
  lcd.write(degrees);
  lcd.print("F");

  lcd.setCursor(0, 1);

  //lcd.print("2:");
  //lcd.print(convert_temp(samples2, THERMISTORNOMINAL_1, SERIESRESISTOR_1), 1);
  //lcd.write(degrees);
  //lcd.print("F");
  
  delay(1000);
}


