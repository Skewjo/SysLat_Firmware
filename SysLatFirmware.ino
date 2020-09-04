#include <Time.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int redSensorPin = A2;
int redSensorValue = 0;
int redSensorAccept = 900;
int redCalibrate = 0;
int timeoutCounter = 0;

unsigned long millisBegin, millisEnd, millisTotal;
unsigned long millisTest1;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  lcd.begin(16,2);
  //randomSeed();
}

void loop() {
  while(redCalibrate < 1){ // || blueCalibrate == 0 || greenCalibrate ==0
    lcd.setCursor(0,0);
    lcd.print("Calibrating"); 
    int i = 0;
    
    millisBegin = millis();
    Serial.write("A");
    millisTest1 = millis();
    while(redSensorValue < redSensorAccept && i < 500){
      delay(1);//can delay take a double?
      redSensorValue = analogRead(redSensorPin);
      lcd.setCursor(12,1);
      lcd.print(redSensorValue);
      i++;
    }
    millisEnd = millis();
    millisTotal = millisEnd-millisBegin;
    
    if(millisTotal <= 3){
      redCalibrate = 0;
      redSensorAccept = redSensorAccept + 5;
      lcd.setCursor(0,0);
      lcd.print("Too Bright ");
      lcd.setCursor(0,1);
      lcd.print(redSensorAccept);
      delay(500);
    }
    else if(millisTotal >= 500){
      redCalibrate = 0;
      timeoutCounter++;
      redSensorAccept = redSensorAccept - 25;
      lcd.setCursor(0,0);
      lcd.print("Timeout");
      lcd.setCursor(0,1);
      lcd.print(redSensorAccept);
    }
    /*
    else if(redSensorAccept < 500){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("No signal detected");
      lcd.setCursor(0,1);
      lcd.print("Restarting calibration");
      delay(600);
    }*/
    else{
      redCalibrate++;
    }
    Serial.flush();
    delay(100);
    Serial.write("B");
    delay(500);
    Serial.flush();
  }
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Beginning Test"); 
  
  while(timeoutCounter < 3){
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    int i = 0;
    
    millisBegin = millis();
    Serial.write("A");
    redSensorValue = 0;
    millisTest1 = millis();
    while(redSensorValue < redSensorAccept && i < 5000){
      delay(0.1);
      redSensorValue = analogRead(redSensorPin);
      i++;
    }
    millisEnd = millis();
    millisTotal = millisEnd-millisBegin;
    //Serial.print(millisTotal);
    //Serial.write(millisTotal);
    lcd.clear();
    lcd.setCursor(0, 0);
    
    if(i < 5000){
      lcd.print(String(millisBegin) + String(" : ") + String(millisEnd));
      lcd.setCursor(0,1);
      lcd.print(millisTotal);
      lcd.setCursor(8, 1);
      lcd.print(millisTest1 - millisBegin);
    }
    
    if(millisTotal <= 3){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Too Bright");
      redSensorAccept -= 25;
      redCalibrate = 0;
    }
    else if(i >= 5000){
      timeoutCounter++;
      lcd.print("Timeout");
    }
    Serial.flush();
    
    delay(100);
    Serial.write("B");
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    Serial.flush();
    delay(500);
  }
  if(timeoutCounter >= 3){
    timeoutCounter = 0;
    redCalibrate = 0;
    //blueCalibrate = 0;
    //greenCalibrate = 0;
  }
}
