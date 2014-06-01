#include <CMPS10.h>
#include <Servo.h>
#include <Wire.h>

#define DEBUG 1

Servo rudder;
Servo sail;
CMPS10 cmps10; // compass

char current_line[6]; // allocate some space for the string

void log(char* message) {
    if (DEBUG) {
        Serial.println(message);
    }
}

void log_int(char* name, int _int) {
    if (DEBUG) {
        char line[strlen(name) + 15];
        sprintf(line, "{\"%s\": %d}", name, _int);
        log(line);
    }
}

void setup() {
    Serial.begin(9600);

    rudder.attach(10, 1060, 1920);
    sail.attach(9, 1050, 1930);

    // set default positions
    rudder.writeMicroseconds(1500);
    sail.writeMicroseconds(1500);

    // initialize the I2C bus
    Wire.begin();
    log("{\"started\": true}");
}

void read_line(char* line) {
    char c;
    int index;
    for (index = 0; index < 5; index++) {
        // wait until there is a character waiting on the Serial line
        while (Serial.available() == 0);
        // read a character
        c = Serial.read();
        if (c == '\n') {
            break;
        } else {
            line[index] = c;
        }
    }
    // terminate the string
    line[index] = '\0';
}

void move_rudder(int amount) {
    amount = constrain(amount, 1060, 1920);
    rudder.writeMicroseconds(amount);
    log_int("rudder", amount);
}

void move_sail(int amount) {
    amount = constrain(amount, 1050, 1930);
    sail.writeMicroseconds(amount);
}

int getAmount() {
    int amount;
    amount = (int) strtol(current_line+1, NULL, 10);
    return amount;
}

float readCompass() {
    Serial.println(cmps10.bearing());
}

void loop() {
    read_line(current_line);
    switch (current_line[0]){
        case 'c':
            if (DEBUG) {
                Serial.print("c");
            }
            readCompass();
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
