/*

*/
#include <Time.h>

int timeoutCounter = 0;
int millisTimeout = 1000;  // was 500, doubled because I think reg arduino clock speed is 8 MHz
int numberOfReadings = 0;
const int totalReadings = 10;

const int buf_size = 32;
char buffer[buf_size];
char msg[] = "light";
int index = 0;
//int pin_val = 0;
const byte IOCpin = 7;
bool button_flag = false;

//TABLES
//From StackOverflow: If your array has static storage allocation, it is default initialized to zero. 
//However, if the array has automatic storage allocation, then you can simply initialize all its elements to zero using an array initializer list which contains a zero. 
//I think this means we'll need to check the compiler and/or the board we end up using to ensure that it uses static storage allocation...not sure
long arduinoClockBegin[totalReadings];
long arduinoClockEnd[totalReadings];

unsigned long millisBegin, millisEnd, millisTotal;
unsigned long millisTest1;


// the setup function runs once when you press reset or power the board
void setup() {

  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(5, INPUT);
  pinMode(IOCpin, INPUT);

  cli();//stop interrupts

//set timer3 interrupt at 1Hz
  TCCR3A = 0;// set entire TCCR1A register to 0
  TCCR3B = 0;// same for TCCR1B
  TCNT3  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR3A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR3B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR3B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK3 |= (1 << OCIE1A);

  attachInterrupt(digitalPinToInterrupt(IOCpin), IOC, RISING);
  //establishContact();

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
        

// /\/\/\/\Above is serial echo function

  /*
  
  DON'T NEED CALIBRATION LOOP
  
  */
  
  //After calibration, send over the current clock time for data synchronization
    int startMillis = millis();
    Serial.write("C");
    delay(10);
    Serial.flush();
    Serial.write(startMillis);
    delay(10);
    Serial.flush();
    
    //------------------------------------------------------------------------------BEGIN WORK LOOP ------------------------------------------------------
    while (timeoutCounter < 3 && numberOfReadings < totalReadings) {
        int i = 0;
       // whiteSensorValue = 0;

        millisBegin = millis();          //save clock count before sending, maybe should start after b/c of printf delay?
        Serial.write("A");
        //while (whiteSensorValue < whiteSensorAccept && i < millisTimeout) {
        while((!button_flag)&&(i < millisTimeout)){
           delay(1);   //keepin the delay for the millisTimeout
          //  whiteSensorValue = analogRead(whiteSensorPin);    //basically reading the analog pin every 1ms to see if voltage is higher than the calibrated threshold (whiteSensorAccept) 
           i++;                                              
        }       
       // button_flag = false;    //reset the flag
        pinCheck();
        millisEnd = millis();
        millisTotal = millisEnd - millisBegin;

        //The following statement assumes that you will not have a sub-2 millisecond input lag, but some systems may currently be capable...
//        if (millisTotal <= 1) {
//            timeoutCounter++;
//           // lcd.setCursor(0, 0);
//           // lcd.print("Too Fast    ");
//           Serial.print("Too Fast\r\n");    //comment this when running with program
//           delay(10);  Serial.flush();
//        }

        //Serial.print(millisTotal);
        Serial.write(millisTotal);
        
        //Add data to tables
        arduinoClockBegin[numberOfReadings] = millisBegin;
        arduinoClockEnd[numberOfReadings] = millisEnd;
        
        // Commenting all LCD code cause I don't have one yet
        //lcd.setCursor(0, 0);

        if (i < millisTimeout) {
//            lcd.clear();
//            lcd.print(String(millisBegin) + String(" : ") + String(millisEnd));
//            lcd.setCursor(0, 1);
//            lcd.print(millisTotal);
            timeoutCounter = 0;
        }
        else if (i >= millisTimeout) {
            timeoutCounter++;
          //  lcd.setCursor(0, 0);
          //  lcd.print("Timeout     ");
        //  Serial.print("Timeout\r\n");    //comment this when running with program
          delay(10);  Serial.flush();
        }
        
        
        //possibly move this first flush to directly after "millisTotal" calculation so that we can possibly get rid of the first delay?
        Serial.flush();   //I'd say yes, or don't use at all becaue the definition says "Waits for the transmission of outgoing serial data to complete."
        delay(100);     // and you haven't sent anything, unless you know more about this function than me
        Serial.write("B");
        delay(500);
        Serial.flush();
        numberOfReadings++;
    }

//------------------------------------------------------------------END OF WORK LOOP------------------------------------------------------------------


  //Write back ending clock time
    int endMillis = millis();
    Serial.write("D");
    delay(10);
    Serial.flush();
    Serial.write(endMillis);
    delay(10);
    Serial.flush();

    // Summary print
    for(int i = 0; i <= numberOfReadings; i++){
     //   Serial.print(arduinoClockBegin[i]);
     //   Serial.print(" ");        //is this a space delimiter?
     //   Serial.println(arduinoClockEnd[i]);
        delay(10);
        Serial.flush();
    }
    
    if(timeoutCounter >= 3){
      //  lcd.clear();
      //  lcd.write("Timeout    ");
   //   Serial.println("Timeout");
        delay(500);
    }
    
    //Reset
 //   lcd.clear();
    timeoutCounter = 0;
//    whiteCalibrate = 0;
    numberOfReadings = 0;

}
//********----------********----------********----------********----------*****------END OF MAIN----********----------********----------





// Interrupt On Change ISR
// as soon as pin goes high, this flag is set
void IOC()
{
  button_flag = true;
}

ISR(TIMER3_COMPA_vect){               //timer3 interrupt 
  //Serial.print("1Hz\r\n");      //it works!
}

void pinCheck()
  //instead of pinCheck, let's use the pin interrupt and check the flag
  {
    if(button_flag == true){
      digitalWrite(LED_BUILTIN, HIGH); 
      delay(100);    
      digitalWrite(LED_BUILTIN, LOW);
     // Serial.println(msg);
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
