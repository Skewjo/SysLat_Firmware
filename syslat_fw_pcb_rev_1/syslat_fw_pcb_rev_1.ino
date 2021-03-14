#include <Time.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 6, 4, 0, 1, 9, 5);      //syslat custom pcb version

int timeoutCounter = 0;

int index = 0;
const byte IOCpin = 7;
bool button_flag = false;
bool timer3_flag = false;

const int shortDelay = 10;
const int midDelay = 10;
const int longDelay = 250;//Sometimes when I lower the value below to 250, it works beautifully and I get like... 2 reads/second I think - Other times it makes the average system latency appear to go up from ~15-30 to ~50-60...
// 3-12-21: Somehow I got this (the longDelay variable above) down to 100, but it somehow affected (it lowered it!?!?) my average system latency, which was verrrryyy strange. Now it's got me questioning whether I know anything at all.
float f_timerTotal = 0;


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

  //set timer3 interrupt at 1Hz
  TCCR3A = 0;// set entire TCCR1A register to 0
  TCCR3B = 0;// same for TCCR1B
  TCNT3  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR3A = 7812;   //7812 should be every 1/2 second; // = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR3B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR3B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK3 |= (1 << OCIE1A);
  
  attachInterrupt(digitalPinToInterrupt(IOCpin), IOC, RISING);
  sei();//allow interrupts
}



//-----------------------------------------------------------BEGIN MAIN-----------------------------------------------------------//
void loop() {
  while (timeoutCounter < 3) {
    if(timer3_flag){
      int i = timeTheFlash();
      
      lcd.setCursor(0, 0);
  
      if (timerTotal <= 1) {
        timeoutCounter++;
        lcd.setCursor(0, 0);
        lcd.print("Too Fast");
      }
      else if (timerTotal < 500000){
        f_timerTotal = static_cast<float>(timerTotal)/1000;
        lcd.clear();
        lcd.print(f_timerTotal);
        timeoutCounter = 0;
      }
      if (timerTotal >= 500000) {
        timeoutCounter++;
        lcd.clear();
        lcd.print("Timeout");
        delay(10);
        //Serial.flush();
      }
  
      sendB();
      sendData(f_timerTotal);
      button_flag = false;
      
//  if(timer3_flag){
//      lcd.clear();
//      lcd.write("2 Hz int");
      timer3_flag = false;
    }
  }
  
  if(timeoutCounter >= 3){
    if(!Serial){
      lcd.clear();
      lcd.write("Syslat"); delay(longDelay);
      lcd.clear();
      lcd.write("disconn"); delay(longDelay);
    }
    else{
      lcd.clear();
      lcd.write("Timeout3");
      delay(longDelay);
    }
  }
  
  //Reset
  lcd.clear();
  timeoutCounter = 0;
}

int timeTheFlash(){
  int i = 0;
  Serial.write("A");
  
  timerBegin = micros();
  while((!button_flag)&&(i < 5000)){
    _delay_us(100);
    i++;
  }
  timerEnd = micros();
  timerTotal = timerEnd - timerBegin;
  
  return i;
}

void sendB(){ //possibly move this first flush to directly after "timerTotal" calculation so that we can possibly get rid of the first delay?
    //Serial.flush();
    //delay(midDelay);
    Serial.write("B");
    //delay(longDelay);
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
    //delay(midDelay);
    timerTotal = timerTotal % factor;
  }
  //delay(midDelay);
  Serial.write("C");
  //delay(longDelay);
  Serial.flush();
}

void IOC()
{
  button_flag = true; // Interrupt On Change ISR - as soon as pin goes high, this flag is set
}

ISR(TIMER3_COMPA_vect){               //timer3 interrupt 
  //Serial.print("1Hz\r\n");      //it works!
  timer3_flag = true;
}
