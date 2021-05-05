#include <Time.h>
#include <LiquidCrystal.h>

//const byte IOCpin = 7;
const byte analogIn = 10;

//bool button_flag = false;
int analogValue = 0;
int aValueAccept = 0;

int millisTimeout = 0;
int microsTimeout = 0;
int timeoutCounter = 0;
int index = 0;
char calibCount = 0;

//sensor check rate is set by the following delays:
//SendB --> midDelay + longDelay
//SendData --> midDelay x 2 + longDelay

const int shortDelay = 10;
const int midDelay = 100;   //current value = 10
const int longDelay = 100; 
float f_timerTotal = 0;


unsigned long timerBegin, timerEnd, timerTotal;

LiquidCrystal lcd(8, 6, 4, 0, 1, 9, 5);      //syslat custom pcb version 
  /*    // if we get a valid byte, echo it
        if (Serial.available() > 0) {
          digitalWrite(LED_BUILTIN, HIGH); 
          delay(100);    
          digitalWrite(LED_BUILTIN, LOW);
          
          // get incoming bytes:
          Serial.readBytesUntil('\0', buffer, buf_size);
      
          Serial.print(buffer);
          for(int i = 0; i < buf_size; i++){
            if(buffer[i] == '\0'){
              index = i;    //copy the location of the null char
              i = buf_size;   //force exit from the for loop
            }
          }
          //below loop writes over existing chars with null characters
          for(int i = 0; i < index; i++){
            buffer[i] = '\0';
          }
        }
        */// serial echo function

//calibration loop
   /*
    while (calibCount < 2) {
        lcd.setCursor(0, 0);
        lcd.print("Calib");
        int i = timeTheFlash();
        lcd.setCursor(5, 1);
        lcd.print(analogValue);
        
        if (millisTotal <= 3) {
            delay(midDelay);
            calibCount = 0;
            //I believe if the accept value is incremented by an odd/prime number, it will make it less likely to get hung and it will be more accurate
            aValueAccept = aValueAccept + 3; 
            lcd.setCursor(0, 1);
            lcd.print(aValueAccept);
        }
        else if (millisTotal >= millisTimeout) {
            whiteCalibrate = 0;
            timeoutCounter++;
            whiteSensorAccept = whiteSensorAccept - 25;
            lcd.setCursor(0, 1);
            lcd.print(whiteSensorAccept);
        }
        else {
            calibCount++;
        }
        sendB();
        sendData(millisBegin, millisEnd, millisTotal);
    }
    */
/*
  DON'T (NOT!) NEED CALIBRATION LOOP 
*/

void setup() {
  
  lcd.begin(8, 1);
  lcd.setCursor(0, 0);
  Serial.begin(9600); // start serial port at 9600 bps and wait for port to open:
  
//  while(!Serial){
//    lcd.print("Waiting for SysLat");
//    delay(350);
//    for(int s = 0; s < 18; s++){//'waiting for syslat' is 18 characters
//      lcd.scrollDisplayLeft(); 
//      delay(500);
//      if(Serial){
//        break;
//      }
//    }
//    lcd.clear();
//    lcd.setCursor(0,0);
//  }
  
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(IOCpin, INPUT);
  pinMode(analogIn, INPUT);
  cli(); //disallow interupts while we set one up...
  //attachInterrupt(digitalPinToInterrupt(IOCpin), IOC, RISING);
  sei();//allow interrupts
}



//-----------------------------------------------------------BEGIN MAIN-----------------------------------------------------------//
void loop() {
//  while (timeoutCounter < 3) {
//    timeTheFlash();
//    
//    lcd.setCursor(0, 0);
//
//    if (timerTotal <= 1) {
//      timeoutCounter++;
//      lcd.setCursor(0, 0);
//      lcd.print("Too Fast");
//    }
//    else if (timerTotal < 500000){
//      f_timerTotal = static_cast<float>(timerTotal)/1000;
//      lcd.clear();
//      lcd.print(f_timerTotal);
//      timeoutCounter = 0;
//    }
//    if (timerTotal >= 500000) {
//      timeoutCounter++;
//      lcd.setCursor(0, 0);
//      lcd.print("Timeout");
//      delay(10);
//    }
//    
//    sendB();
//    sendData(f_timerTotal);
//    //button_flag = false;
//  }
  
//  if(timeoutCounter >= 3){
//    lcd.clear();
//    lcd.write("Timeout");
//    //delay(longDelay);
//    delay(50);
//  }

  analogValue = analogRead(analogIn);
  lcd.print(analogValue);
  delay(500);
  
  //Reset
  lcd.clear();
  timeoutCounter = 0;
}

void timeTheFlash(){
  int i = 0;
  analogValue = 0;
  Serial.write("A");
  timerBegin = micros();
  //while((!button_flag)&&(i < 5000)){
  while (analogValue < aValueAccept && i < microsTimeout) {
    _delay_us(1000);
    analogValue = analogRead(analogIn);
    i++;
  }
  timerEnd = micros();
  timerTotal = timerEnd - timerBegin;
  //lcd.clear();
  //lcd.write(analogValue);
}

void sendB(){ //possibly move this first flush to directly after "timerTotal" calculation so that we can possibly get rid of the first delay?
    Serial.flush();
    delay(midDelay);
    Serial.write("B");
    delay(longDelay);
    Serial.flush();
}

void sendData(unsigned long timerTotal){ 
  int num,temp,factor=1;
  temp=timerTotal;

  while(temp){
    temp=temp/10;
    factor = factor*10;
  }
  while(factor>1){
    factor = factor/10;
    Serial.print(timerTotal/factor);
    Serial.flush();
    delay(midDelay);
    timerTotal = timerTotal % factor;
  }
  delay(midDelay);
  Serial.write("C");
  delay(longDelay);
  Serial.flush();
}

//void IOC()
//{
//  button_flag = true; // Interrupt On Change ISR - as soon as pin goes high, this flag is set
//}
