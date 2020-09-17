//TODO: FIGURE OUT A WAY TO GET RID OF ALL THE STINKING FLUSHES AND DELAYS

#include <Time.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int whiteSensorPin = A2;
const int totalReadings = 10;

const int shortDelay = 10;
const int midDelay = 100;
const int longDelay = 500;


int whiteSensorValue = 0;
int whiteSensorAccept = 900;
int whiteCalibrate = 0;
int timeoutCounter = 0;
int millisTimeout = 500;
int numberOfReadings = 0;

//TABLES
//From StackOverflow: If your array has static storage allocation, it is default initialized to zero. 
//However, if the array has automatic storage allocation, then you can simply initialize all its elements to zero using an array initializer list which contains a zero. 
//I think this means we'll need to check the compiler and/or the board we end up using to ensure that it uses static storage allocation...not sure
long arduinoClockBegin[totalReadings];
long arduinoClockEnd[totalReadings];






unsigned long millisBegin, millisEnd, millisTotal;
unsigned long millisTest1;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB
    }
    lcd.begin(16, 2);
    //randomSeed();
}

void loop() {
    
    //calibration loop
    while (whiteCalibrate < 2) {
        int i = 0;
        lcd.setCursor(0, 0);
        lcd.print("Calibrating");

        whiteSensorValue = 0;
        millisBegin = millis();
        Serial.write("A");
        millisTest1 = millis();
        while (whiteSensorValue < whiteSensorAccept && i < millisTimeout) {
            delay(1);
            whiteSensorValue = analogRead(whiteSensorPin);
            lcd.setCursor(12, 1);
            lcd.print(whiteSensorValue);
            i++;
        }
        millisEnd = millis();
        millisTotal = millisEnd - millisBegin;

        if (millisTotal <= 3) {
            delay(midDelay);
            whiteCalibrate = 0;
            //I believe if the accept value is incremented by an odd/prime number, it will make it less likely to get hung.
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
        Serial.flush();
        delay(midDelay);
        Serial.write("B");
        delay(longDelay);
        Serial.flush();
    }
    
    //After calibration, send over the current clock time for data synchronization
    int startMillis = millis();
    Serial.write("C");
    delay(shortDelay);
    Serial.flush();
    Serial.write(startMillis);
    delay(shortDelay);
    Serial.flush();
    
    
    //work loop
    while (timeoutCounter < 3 && numberOfReadings < totalReadings) {
        int i = 0;
        whiteSensorValue = 0;

        millisBegin = millis();
        Serial.write("A");
        while (whiteSensorValue < whiteSensorAccept && i < millisTimeout) {
            delay(1);
            whiteSensorValue = analogRead(whiteSensorPin);
            i++;
        }
        millisEnd = millis();
        millisTotal = millisEnd - millisBegin;

        //Serial.print(millisTotal);
        //Serial.write(millisTotal);
        
        //Add data to tables
        arduinoClockBegin[numberOfReadings] = millisBegin;
        arduinoClockEnd[numberOfReadings] = millisEnd;
        

        
        
        lcd.setCursor(0, 0);

        if (i < millisTimeout) {
            lcd.clear();
            lcd.print(String(millisBegin) + String(" : ") + String(millisEnd));
            lcd.setCursor(0, 1);
            lcd.print(millisTotal);
            timeoutCounter = 0;
        }
        
        //The following statement assumes that you will not have a sub-2 millisecond input lag, but some systems may currently be capable...
        if (millisTotal <= 1) {
            timeoutCounter++;
            lcd.setCursor(0, 0);
            lcd.print("Too Fast    ");
        }
        else if (i >= millisTimeout) {
            timeoutCounter++;
            lcd.setCursor(0, 0);
            lcd.print("Timeout     ");
        }
        
        //possibly move this first flush to directly after "millisTotal" calculation so that we can possibly get rid of the first delay?
        Serial.flush();
        delay(midDelay);
        Serial.write("B");
        delay(longDelay);
        Serial.flush();
        numberOfReadings++;
    }
    
    //Write back ending clock time
    int endMillis = millis();
    Serial.write("D");
    delay(shortDelay);
    Serial.flush();
    Serial.write(endMillis);
    delay(shortDelay);
    Serial.flush();
    
    for(int i = 0; i <= numberOfReadings; i++){
        Serial.print(arduinoClockBegin[i]);
        Serial.print(" ");
        Serial.println(arduinoClockEnd[i]);
        delay(shortDelay);
        Serial.flush();
    }

    //Write out an 'E' to signify that data transfer is complete
    Serial.write("E");
    delay(shortDelay);
    Serial.flush();
    
    if(timeoutCounter >= 3){
        lcd.clear();
        lcd.write("Timeout    ");
        delay(longDelay);
    }
    
    //Reset
    lcd.clear();
    timeoutCounter = 0;
    whiteCalibrate = 0;
    numberOfReadings = 0;
    
    
}
