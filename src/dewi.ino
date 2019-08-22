#include <EEPROM.h>
#include <Servo.h>
#include <Wire.h>

#include "compass.h"

#define DEBUG 1

#define MIN_SAIL_PWM 1100
#define MAX_SAIL_PWM 2000
#define MIN_RUDDER_PWM 1060
#define MAX_RUDDER_PWM 1920

#define COMPASS_OFFSET 77

Servo rudder;
Servo sail;
int windSensorPin = A2;
int offset; // wind sensor offset
char current_line[10];  // allocate some space for the request string


void setup() {
    Serial.begin(115200);

    pinMode(windSensorPin, INPUT);

    offset = get_stored_offset();

    rudder.attach(10, MIN_RUDDER_PWM, MAX_RUDDER_PWM);
    sail.attach(9, MIN_SAIL_PWM, MAX_SAIL_PWM);
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
    amount = constrain(amount, MIN_RUDDER_PWM, MAX_RUDDER_PWM);
    rudder.writeMicroseconds(amount);
    log_json_int("rudder", amount);
}

void set_sail(int amount) {
    // set the sail to amount
    amount = constrain(amount, MIN_SAIL_PWM, MAX_SAIL_PWM);
    sail.writeMicroseconds(amount);
    log_json_int("sail", amount);
}

int get_amount(char* line) {
    // return the number in a string such as "r1200" as an int
    int amount;
    amount = (int) strtol(line + 1, NULL, 10);
    return amount;
}

float read_compass_bearing() {
    // read from the compass and output it
    if (Compass.poll_data()) { 
        return Compass.get_bearing();
    } else {
      // Send error if false(?)
        return -1;
    }
}

float read_compass_pitch(){
    // read from the compass
    if (Compass.poll_data()){
        return Compass.get_pitch();
    } else {
        return -1;
    }
}

float read_compass_roll(){
    // read from the compass
    if (Compass.poll_data()){
        return Compass.get_roll();
    } else {
        return -1;
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
    read_line(current_line);

    int wind_angle;
    float dispatchf;

    switch (current_line[0]) {
        case 'c':
            dispatchf = read_compass_bearing() - COMPASS_OFFSET;  // compensate for offset
            dispatchf = mod(dispatchf);  // wrap around
            log_json_float("compass", dispatchf);
            break;
        case 'p':
            dispatchf = read_compass_pitch();
            dispatchf = mod(dispatchf);  // wrap around
            log_json_float("pitch", dispatchf);
            break;
        case '&': // r was already taken
            dispatchf = read_compass_roll();
            dispatchf = mod(dispatchf);  // wrap around
            log_json_float("roll", dispatchf);
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
