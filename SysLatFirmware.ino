#include <Time.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int whiteSensorPin = A2;
int whiteSensorValue = 0;
int whiteSensorAccept = 900;
int whiteCalibrate = 0;
int timeoutCounter = 0;
int millisTimeout = 500;

unsigned long millisBegin, millisEnd, millisTotal;
unsigned long millisTest1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16,2);
  //randomSeed();
}

void loop() {
  while(whiteCalibrate < 2){
    int i = 0;
    lcd.setCursor(0,0);
    lcd.print("Calibrating"); 

    whiteSensorValue = 0;
    millisBegin = millis();
    Serial.write("A");
    millisTest1 = millis();
    while(whiteSensorValue < whiteSensorAccept && i < millisTimeout){
      delay(1);
      whiteSensorValue = analogRead(whiteSensorPin);
      lcd.setCursor(12,1);
      lcd.print(whiteSensorValue);
      i++;
    }
    millisEnd = millis();
    millisTotal = millisEnd-millisBegin;
    
    if(millisTotal <= 5){
      delay(100);
      whiteCalibrate = 0;
      whiteSensorAccept = whiteSensorAccept + 3;
      lcd.setCursor(0,1);
      lcd.print(whiteSensorAccept);
    }
    else if(millisTotal >= millisTimeout){
      whiteCalibrate = 0;
      timeoutCounter++;
      whiteSensorAccept = whiteSensorAccept - 25;
      lcd.setCursor(0,1);
      lcd.print(whiteSensorAccept);
    }
    else{
      whiteCalibrate++;
    }
    Serial.flush();
    delay(100);
    Serial.write("B");
    delay(1000);
    Serial.flush();
  }

  
  
  while(timeoutCounter < 3){
    int i = 0;
    whiteSensorValue = 0;
    
    millisBegin = millis();
    Serial.write("A");
    while(whiteSensorValue < whiteSensorAccept && i < millisTimeout){
      delay(1);
      whiteSensorValue = analogRead(whiteSensorPin);
      i++;
    }
    millisEnd = millis();
    millisTotal = millisEnd-millisBegin;
    
    //Serial.print(millisTotal);
    //Serial.write(millisTotal);
    lcd.setCursor(0, 0);
    
    if(i < millisTimeout){
      lcd.clear();
      lcd.print(String(millisBegin) + String(" : ") + String(millisEnd));
      lcd.setCursor(0,1);
      lcd.print(millisTotal);
      timeoutCounter = 0;
    }
    
    if(millisTotal <= 1){
      timeoutCounter++;
      lcd.setCursor(0,0);
      lcd.print("Too Fast  ");
    }
    else if(i >= millisTimeout){
      timeoutCounter++;
      lcd.setCursor(0,0);
      lcd.print("Timeout   ");
    }
    Serial.flush();
    delay(100);
    Serial.write("B");
    delay(500);
    Serial.flush();
    
  }
  if(timeoutCounter >= 3){
    lcd.clear();
    timeoutCounter = 0;
    whiteCalibrate = 0;
  }
}
