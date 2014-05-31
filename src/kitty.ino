#include <CMPS10.h>
#include <Servo.h>
#include <Wire.h>

Servo rudder;
Servo sail;
CMPS10 cmps10; // compass

char inData[6]; // allocate some space for the string

int DEBUG = 1;

void setup() {
    Serial.begin(9600);

    rudder.attach(10, 1060, 1920);
    sail.attach(9, 1050, 1930);

    // set default positions
    rudder.writeMicroseconds(1500);
    sail.writeMicroseconds(1500);

    Wire.begin(); // initialize the I2C bus

    if (DEBUG) {
        Serial.write("Power On\n");
    }
}

void getData() {
    char inChar;
    int index;
    for (index = 0; index < 5; index++) {
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

void move_rudder(int amount) {
    amount = constrain(amount, 1060, 1920);
    rudder.writeMicroseconds(amount);
}

void move_sail(int amount) {
    amount = constrain(amount, 1050, 1930);
    sail.writeMicroseconds(amount);
}

int getAmount() {
    int amount;
    amount = (int) strtol(inData+1, NULL, 10);
    Serial.println(amount);
    return amount;
}

float readCompass() {
    return cmps10.bearing();
}

void loop() {
    getData();
    switch (inData[0]){
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
            move_rudder(getAmount());
            break;
        case 's':
            if (DEBUG) {
                Serial.println("s");
            }
            move_sail(getAmount());
            break;
    }
}
