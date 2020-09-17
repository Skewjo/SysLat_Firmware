//TODO: FIGURE OUT A WAY TO GET RID OF ALL THE STINKING FLUSHES AND DELAYS

#include <Time.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int whiteSensorPin = A2;

const int shortDelay = 10;
const int midDelay = 100;
//Sometimes when I lower the value below to 250, it works beautifully and I get like... 2 reads/second I think.  Other times it makes the average system latency appear to go up from ~15-30 to ~50-60...
const int longDelay = 500;
const int millisTimeout = 500;

int whiteSensorValue = 0;
int whiteSensorAccept = 900;
int whiteCalibrate = 0;
int timeoutCounter = 0;

unsigned long millisBegin, millisEnd, millisTotal;


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
    
    //work loop
    while (timeoutCounter < 3) {
        int i = timeTheFlash();
        
        lcd.setCursor(0, 0);

        if (i < millisTimeout) {
            lcd.clear();
            lcd.print(String(millisBegin) + String(" : ") + String(millisEnd));
            lcd.setCursor(0, 1);
            lcd.print(millisTotal);
            timeoutCounter = 0;
        }
        
        //The following statement assumes that you will not have a sub-2 millisecond input lag, but some systems may currently be capable...maybe? IDK.
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
        
        sendB();
        sendData(millisBegin, millisEnd, millisTotal);
    }
    
    if(timeoutCounter >= 3){
        lcd.clear();
        lcd.write("Timeout    ");
        delay(longDelay);
    }
    
    reset();    
}


int timeTheFlash(){
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

void reset(){
    lcd.clear();
    timeoutCounter = 0;
    whiteCalibrate = 0;
}
