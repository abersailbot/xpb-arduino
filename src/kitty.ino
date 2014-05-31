#include <CMPS10.h>
#include <Servo.h>
#include <Wire.h>

Servo myRudderServo;
Servo mySailServo;
CMPS10 cmps10; // compass

char inData[6]; // allocate some space for the string

int DEBUG = 1;

void setup() {
    Serial.begin(9600);

    myRudderServo.attach(10, 1060, 1920);
    mySailServo.attach(9, 1050, 1930);

    // set default positions
    myRudderServo.writeMicroseconds(1500);
    mySailServo.writeMicroseconds(1500);

    Wire.begin(); // initialize the I2C bus

    if (DEBUG) {
        Serial.write("Power On\n");
    }
}

void getData() {
    char inChar;
    int index;
    for(index = 0; index < 5; index++) {
        // wait until there is a character waiting on the Serial line
        while (Serial.available() == 0);
        inChar = Serial.read(); // read a character
        if(inChar != '\n') {
            inData[index] = inChar;
        } 
        else {
            break;
        }
    }
    inData[index] = '\0'; // terminate the string
}

void setServo(char servoChar, int turnAmount){
    if('R' == servoChar) {
        turnAmount = constrain(turnAmount, 1060, 1920);
        myRudderServo.writeMicroseconds(turnAmount);
    }
    else {
        turnAmount = constrain(turnAmount, 1050, 1930);
        mySailServo.writeMicroseconds(turnAmount);
    }
}

int getAmount() {
    int turnAmount;
    turnAmount = (int) strtol(inData+1, NULL, 10);
    Serial.println(turnAmount);
    return turnAmount;
}

float readCompass() {
    return cmps10.bearing();
}

void loop() {
    getData();
    switch(inData[0]){
        case 'c':
            if (DEBUG) {
                Serial.print("c");
            }
            Serial.println(readCompass());
            break;
        case 'r':
            if (DEBUG) {
                Serial.println("r");
            }
            setServo('R', getAmount());
            break;
        case 's':
            if (DEBUG) {
                Serial.println("s");
            }
            setServo('S', getAmount());
            break;
    }
}
