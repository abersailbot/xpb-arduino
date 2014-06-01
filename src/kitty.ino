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
        Serial.print("{\"");
        Serial.print(name);
        Serial.print("\": ");
        Serial.print(_int);
        Serial.println("}");
    }
}

void log_float(char* name, float _float) {
    if (DEBUG) {
        Serial.print("{\"");
        Serial.print(name);
        Serial.print("\": ");
        Serial.print(_float);
        Serial.println("}");
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
    log_int("sail", amount);
}

int get_amount(char* line) {
    int amount;
    amount = (int) strtol(line+1, NULL, 10);
    return amount;
}

float read_compass() {
    log_float("compass", cmps10.bearing());
}

void loop() {
    read_line(current_line);
    switch (current_line[0]){
        case 'c':
            read_compass();
            break;
        case 'r':
            move_rudder(get_amount(current_line));
            break;
        case 's':
            move_sail(get_amount(current_line));
            break;
    }
}
