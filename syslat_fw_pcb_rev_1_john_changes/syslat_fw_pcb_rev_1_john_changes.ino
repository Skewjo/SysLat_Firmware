 /*
  REV_1: 9/17/20 - update port for PC prog update
  11/30/20 - working with small LCD, code needs cleanup
  12/29/20 - update for custom PCB, pin changes for LCD
  1/1/21  -  some cleanup work
  1/5/21  -  changed startup LCD display
*/
#include <Time.h>
#include <LiquidCrystal.h>

// LiquidCrystal(rs, rw, enable, d4, d5, d6, d7) //Arduino Uno Old LCD pinout?
// LiquidCrystal lcd(4, 6, 8, 10, 11, 12, 13);   //adafruit itsybitsy prototype version
   LiquidCrystal lcd(8, 6, 4, 0, 1, 9, 5);      //syslat custom pcb version

int timeoutCounter = 0;

int index = 0;
const byte IOCpin = 7;
bool button_flag = false;


const int shortDelay = 10;
const int midDelay = 100;
const int longDelay = 500;//Sometimes when I lower the value below to 250, it works beautifully and I get like... 2 reads/second I think - Other times it makes the average system latency appear to go up from ~15-30 to ~50-60...
const int timerTimeout = 500000;

unsigned long timerBegin, timerEnd, timerTotal;

void setup() {
  lcd.begin(8, 1);
  lcd.setCursor(0, 0);
  Serial.begin(9600); // start serial port at 9600 bps and wait for port to open:
  
  while(!Serial){
    lcd.print("Waiting for SysLat");
    delay(350);
    for(int s = 0; s < 18; s++){//'waiting for syslat' is 18 characters
      lcd.scrollDisplayLeft(); 
      delay(500);
      if(Serial){
        break;
      }
    }
    lcd.clear();
    lcd.setCursor(0,0);
  }
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IOCpin, INPUT);
 
  cli(); //disallow interupts while we set one up...
  attachInterrupt(digitalPinToInterrupt(IOCpin), IOC, RISING);
  sei();//allow interrupts
}



//-----------------------------------------------------------BEGIN MAIN-----------------------------------------------------------//
void loop() {
  while (timeoutCounter < 3) {
    int i = timeTheFlash();
    
    lcd.setCursor(0, 0);

    if (timerTotal <= 1) {
      timeoutCounter++;
      lcd.setCursor(0, 0);
      lcd.print("Too Fast");
    }
    //else if (timerTotal < timerTimeout){
      lcd.clear();
      lcd.print(timerTotal);
      timeoutCounter = 0;
    //}
    if (timerTotal >= timerTimeout) {
      timeoutCounter++;
      lcd.setCursor(0, 0);
      lcd.print("Timeout");
      delay(10);
      Serial.flush();
    }
    
    sendB();
    sendData(timerBegin, timerEnd, timerTotal);
    button_flag = false;
  }
  
  if(timeoutCounter >= 3){
    lcd.clear();
    lcd.write("Timeout");
    delay(longDelay);
  }
  
  //Reset
  lcd.clear();
  timeoutCounter = 0;
}

int timeTheFlash(){
  Serial.write("A");
  
  timerBegin = micros();
  int i = 0;
  while((!button_flag)&&(i < timerTimeout)){
    _delay_us(100);
    i++;
  }
  timerEnd = micros();
  timerTotal = timerEnd - timerBegin;
  timerTotal /= 1000;

  return i;
}

void sendB(){ //possibly move this first flush to directly after "timerTotal" calculation so that we can possibly get rid of the first delay?
    Serial.flush();
    delay(midDelay);
    Serial.write("B");
    delay(longDelay);
    Serial.flush();
}

void sendData(unsigned long timerBegin, unsigned long timerEnd, unsigned long timerTotal){
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

void IOC()
{
  button_flag = true; // Interrupt On Change ISR - as soon as pin goes high, this flag is set
}
