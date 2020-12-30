 /*
  REV_1: 9/17/20 - update port for PC prog update
  11/30/20 - working with small LCD, code needs cleanup
  12/29/20 - update for custom PCB, pin changes for LCD
*/
#include <Time.h>
#include <LiquidCrystal.h>

// LiquidCrystal(rs, rw, enable, d4, d5, d6, d7)
// LiquidCrystal lcd(4, 6, 8, 10, 11, 12, 13);   //this works! (adafruit itsybitsy prototype version)
   LiquidCrystal lcd(8, 6, 4, 0, 1, 9, 5);      //syslat custom pcb version

int timeoutCounter = 0;
int numberOfReadings = 0;
const int totalReadings = 10;

const int buf_size = 32;
char buffer[buf_size];
char msg[] = "light";
int index = 0;
//int pin_val = 0;
const byte IOCpin = 7;
bool button_flag = false;

int whiteSensorValue = 0;

const int shortDelay = 10;
const int midDelay = 100;
//Sometimes when I lower the value below to 250, it works beautifully and I get like... 2 reads/second I think.  Other times it makes the average system latency appear to go up from ~15-30 to ~50-60...
const int longDelay = 500;
const int millisTimeout = 500;

unsigned long ser_cnt = 0;
const long timeout = 5000;

//long arduinoClockBegin[totalReadings];
//long arduinoClockEnd[totalReadings];

unsigned long millisBegin, millisEnd, millisTotal;

unsigned long millisTest1;

void setup() {

  lcd.begin(8, 1);
  lcd.setCursor(0, 0);
  lcd.print("start");
  lcd.setCursor(0, 0);

  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600); //115200
  while ((!Serial)&&(ser_cnt < timeout)) {
    ser_cnt++; // wait for serial port to connect. Needed for native USB port only
  }
  ser_cnt = 0;
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(5, INPUT);
  pinMode(IOCpin, INPUT);

  cli();//stop interrupts

//set timer3 interrupt at 1Hz
//  TCCR3A = 0;// set entire TCCR1A register to 0
//  TCCR3B = 0;// same for TCCR1B
//  TCNT3  = 0;//initialize counter value to 0
//  // set compare match register for 1hz increments
//  OCR3A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
//  // turn on CTC mode
//  TCCR3B |= (1 << WGM12);
//  // Set CS10 and CS12 bits for 1024 prescaler
//  TCCR3B |= (1 << CS12) | (1 << CS10);  
//  // enable timer compare interrupt
//  TIMSK3 |= (1 << OCIE1A);

  attachInterrupt(digitalPinToInterrupt(IOCpin), IOC, RISING);
  
  sei();//allow interrupts

  
}



//********----------********----------********----------********----------*****------BEGIN MAIN----********----------********----------

void loop() {
    
  // if we get a valid byte, echo it
    
  /*
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
        */
        

// /\/\/\/\  Above is serial echo function

//calibration loop
   /*
    while (whiteCalibrate < 2) {
        lcd.setCursor(0, 0);
        lcd.print("Calibrating");

        int i = timeTheFlash();

        lcd.setCursor(12, 1);
        lcd.print(whiteSensorValue);
        
        if (millisTotal <= 3) {
            delay(midDelay);
            whiteCalibrate = 0;
            //I believe if the accept value is incremented by an odd/prime number, it will make it less likely to get hung and it will be more accurate
            whiteSensorAccept = whiteSensorAccept + 3;
            lcd.setCursor(0, 1);
            lcd.print(whiteSensorAccept);
        }
        else if (millisTotal >= millisTimeout) {
            whiteCalibrate = 0;
            timeoutCounter++;
            whiteSensorAccept = whiteSensorAccept - 25;
            lcd.setCursor(0, 1);
            lcd.print(whiteSensorAccept);
        }
        else {
            whiteCalibrate++;
        }

        sendB();
        sendData(millisBegin, millisEnd, millisTotal);
    }
    */
/*
  DON'T NEED CALIBRATION LOOP
*/

    
    
    //------------------------------------------------------------------------------BEGIN WORK LOOP ------------------------------------------------------
    while (timeoutCounter < 3) {

        int i = timeTheFlash();
       
        lcd.setCursor(0, 0);

        if (i < millisTimeout) {
            lcd.clear();
//            lcd.print(String(millisBegin) + String(" : ") + String(millisEnd));
            lcd.setCursor(0, 0);
            lcd.print(millisTotal);
            timeoutCounter = 0;
        }

        //The following statement assumes that you will not have a sub-2 millisecond input lag, but some systems may currently be capable...
        if (millisTotal <= 1) {
            timeoutCounter++;
            lcd.setCursor(0, 0);
            lcd.print("Too Fast");
           // Serial.print("Too Fast\r\n");    //comment this when running with program
        }
        else if (i >= millisTimeout) {
            timeoutCounter++;
            lcd.setCursor(0, 0);
            lcd.print("Timeout");
        //  Serial.print("Timeout\r\n");    //comment this when running with program
          delay(10);  Serial.flush();
        }
        
        //Add data to tables
       // arduinoClockBegin[numberOfReadings] = millisBegin;
       // arduinoClockEnd[numberOfReadings] = millisEnd;

       sendB();
       sendData(millisBegin, millisEnd, millisTotal);  

       pinCheck();
    }
    
    if(timeoutCounter >= 3){
        lcd.clear();
        lcd.write("Timeout");
   //   Serial.println("Timeout");
        delay(longDelay);
    }
    
    //Reset
    lcd.clear();
    timeoutCounter = 0;
//    whiteCalibrate = 0;
    numberOfReadings = 0;

}
//********----------********----------********----------********----------*****------END OF MAIN----********----------********----------


int timeTheFlash(){
    int i = 0;
    //whiteSensorValue = 0;
    millisBegin = millis();
    Serial.write("A");
    //while (whiteSensorValue < whiteSensorAccept && i < millisTimeout) {
    while((!button_flag)&&(i < millisTimeout)){
        delay(1);
        //delayMicroseconds(1000);
        //whiteSensorValue = analogRead(whiteSensorPin);
        i++;
    }
    millisEnd = millis();
    millisTotal = millisEnd - millisBegin;

    //button_flag = false;    //reset the buton flag bc I removed the interrupt

    return i;
}

void sendB(){
    //possibly move this first flush to directly after "millisTotal" calculation so that we can possibly get rid of the first delay?
    Serial.flush();
    delay(midDelay);
    Serial.write("B");
    delay(longDelay);
    Serial.flush();
}

void sendData(unsigned long millisBegin, unsigned long millisEnd, unsigned long millisTotal){
    //Serial.print(millisBegin);
    //Serial.print(" ");
    //Serial.print(millisEnd);
    //Serial.print(" ");
    //Serial.print(millisTotal);

    int num,temp,factor=1;

    temp=millisTotal;

    while(temp){
      temp=temp/10;
      factor = factor*10;
    }

    while(factor>1){
      factor = factor/10;
      Serial.print(millisTotal/factor);
      Serial.flush();
      delay(midDelay);
      millisTotal = millisTotal % factor;
    }
    delay(midDelay);
    Serial.write("C");
    delay(longDelay);
    Serial.flush();
}


// Interrupt On Change ISR
// as soon as pin goes high, this flag is set
void IOC()
{
  button_flag = true;
}

//ISR(TIMER3_COMPA_vect){               //timer3 interrupt 
//  //Serial.print("1Hz\r\n");      //it works!
//}

void pinCheck()
  //instead of pinCheck, let's use the pin interrupt and check the flag
  {
    if(button_flag == true){
//      digitalWrite(LED_BUILTIN, HIGH); 
//      delay(50);    
//      digitalWrite(LED_BUILTIN, LOW);
      //Serial.println(msg);
      button_flag = false;    //reset the flag
    }
  //  pin_val = digitalRead(5);
  //  if(pin_val == LOW){
  //    digitalWrite(LED_BUILTIN, HIGH); 
  //    delay(100);    
  //    digitalWrite(LED_BUILTIN, LOW);
  //    Serial.println(msg);
  //  }
  //  else{
  //    digitalWrite(LED_BUILTIN, LOW);
  //    delay(100);
  //  }
}
