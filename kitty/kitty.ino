#include <CMPS10.h>
#include <Servo.h>
#include <Wire.h>

Servo myRudderServo; // create servo object to control a servo 
Servo mySailServo; // a maximum of eight servo objects can be created
CMPS10 cmps10; // compass

char inData[6]; // Allocate some space for the string

int DEBUG = 0;

void setup() {
    Serial.begin(9600); //Begin at 9600
    //Use .attach for setting up connection to the servo
    myRudderServo.attach(10, 1060, 1920); // Attach, with the output limited
    // between 1000 and 2000 ms
    mySailServo.attach(9, 1050, 1930); // Same, but between 1000 and 1850 ms
    myRudderServo.writeMicroseconds(1500);
    mySailServo.writeMicroseconds(1500);
    Wire.begin(); // Initialize the I2C bus for the compass
    if (DEBUG) {
        Serial.write("Power On\n");
    }
}

void getData() {
    char inChar; // Where to store the character read
    int index; // Index into array; where to store the character
    for(index = 0; index < 5; index++) {
        while (Serial.available() == 0); //Null statment to wait until there is a character waiting on the Serial line
        inChar = Serial.read(); // Read a character
        if(inChar != '\n') {
            inData[index] = inChar; // Store it
        } 
        else {
            break;
        }
    }
    inData[index] = '\0'; // Null terminate the string
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
    return cmps10.bearing(); // Print the sensor readings to the serial port.
}

void loop() {
    getData();
    switch(inData[0]){
        case 'c':
            if (DEBUG) {
                Serial.print("c");
            }
            Serial.println(readCompass()); //Compass Read
            break;
        case 'r':
            if (DEBUG) {
                Serial.println("r");
            }
            setServo('R', getAmount()); // Rudder Set
            break;
        case 's':
            if (DEBUG) {
                Serial.println("s");
            }
            setServo('S', getAmount()); // Sail Set
            break;
    }
}
