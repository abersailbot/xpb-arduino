#include <EEPROM.h>
#include <Servo.h>
#include <Wire.h>

#include "compass.h"

#define DEBUG 1

Servo rudder;
Servo sail;
int windSensorPin = A2;
int offset; // wind sensor offset
char current_line[6];  // allocate some space for the request string


void setup() {
    Serial.begin(115200);

    pinMode(windSensorPin, INPUT);

    offset = get_stored_offset();

    rudder.attach(10, 1060, 1920);
    sail.attach(9, 1100, 2100);
    // set default positions
    rudder.writeMicroseconds(1500);
    sail.writeMicroseconds(1500);
    // initialize the I2C bus
    Wire.begin();
    // tell the client we've started
    Serial.println("{\"started\": true}");
}

int get_stored_offset(void) {
    byte low_byte = EEPROM.read(0);
    byte high_byte = EEPROM.read(1);
    int stored_offset = ((low_byte << 0) & 0xFF) + ((high_byte << 8) & 0xFF00);
    return stored_offset;
}

void store_offset(int new_offset) {
    byte low_byte = ((new_offset >> 0) & 0xFF);
    byte high_byte = ((new_offset >> 8) & 0xFF);
    EEPROM.write(0, low_byte);
    EEPROM.write(1, high_byte);
}

void log_json_int(char* key, int value) {
    // write a single json key value pair
    if (DEBUG) {
        Serial.print("{\"");
        Serial.print(key);
        Serial.print("\": ");
        Serial.print(value);
        Serial.println("}");
    }
}

void log_json_float(char* key, float value) {
    // write a single json key value pair
    if (DEBUG) {
        Serial.print("{\"");
        Serial.print(key);
        Serial.print("\": ");
        Serial.print(value);
        Serial.println("}");
    }
}

void read_line(char* line) {
    // read characters from serial into line until a newline character
    char c;
    int index;
    for (index = 0; index < 5; index++) {
        // wait until there is a character
        while (Serial.available() == 0);
        // read a character
        c = Serial.read();
        if (c == '\n' || c== '\r') {
            line[index]='\0';
            break;
        }
        else {
            line[index] = c;
        }
    }
    // terminate the string
    line[index+1] = '\0';
}

void set_rudder(int amount) {
    // set the rudder to amount
    amount = constrain(amount, 1060, 1920);
    rudder.writeMicroseconds(amount);
    log_json_int("rudder", amount);
}

void set_sail(int amount) {
    // set the sail to amount
    amount = constrain(amount, 1100, 2100);
    sail.writeMicroseconds(amount);
    log_json_int("sail", amount);
}

int get_amount(char* line) {
    // return the number in a string such as "r1200" as an int
    int amount;
    amount = (int) strtol(line + 1, NULL, 10);
    return amount;
}

float read_compass() {
    // read from the compass and output it
    if (Compass.poll_data()) { // Send error if false(?)
        log_json_float("compass", Compass.get_bearing());

    } else {

    }
}

int mod(int angle) {
    if (angle > 359) {
        return mod(angle - 359);
    }
    else if (angle < 0) {
        return mod(359 + angle);
    }
    else {
        return angle;
    }
}

int read_wind_sensor() {
    int pulse_length = 0;
    pulse_length = pulseIn(windSensorPin, HIGH, 2000);

    // 29 is the magic number where pulse time of 1036 = 359
    int magic = 29;

    int wind_angle = ((pulse_length * 10) / magic);
    return wind_angle;
}

void loop() {
    if(DEBUG == 1){
        Serial.println("Running Debug in loop");
    }  
    read_line(current_line);
    if(DEBUG == 1){
        Serial.println(current_line);
    }

    int wind_angle;

    switch (current_line[0]) {
        case 'c':
            read_compass();
            break;
        case 'w':
            wind_angle = read_wind_sensor() - offset;  // compensate for offset
            wind_angle = mod(wind_angle);  // wrap around
            log_json_int("wind", wind_angle);
            break;
        case 'r':
            set_rudder(get_amount(current_line));
            break;
        case 's':
            set_sail(get_amount(current_line));
            break;
        case 'o':
            store_offset(read_wind_sensor());
            offset = get_stored_offset();
            break;
    }
}
