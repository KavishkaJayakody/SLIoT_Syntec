#ifndef KEYPAD_H
#define KEYPAD_H

#include <Arduino.h>

class Keypad {
private:
    static const uint8_t ROWS = 4;
    static const uint8_t COLS = 4;
    
    // Pin configurations
    uint8_t rowPins[ROWS];
    uint8_t colPins[COLS];
    
    // Keypad layout
    char keys[ROWS][COLS] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };
    
    // Debounce delay in milliseconds
    static const uint8_t DEBOUNCE_DELAY = 50;
    unsigned long lastDebounceTime = 0;

public:
    Keypad(const uint8_t* row_pins, const uint8_t* col_pins) {
        // Copy pin configurations
        memcpy(rowPins, row_pins, ROWS);
        memcpy(colPins, col_pins, COLS);
        
        // Initialize row pins as output
        for (uint8_t i = 0; i < ROWS; i++) {
            pinMode(rowPins[i], OUTPUT);
            digitalWrite(rowPins[i], HIGH);
        }
        
        // Initialize column pins as input with pullup
        for (uint8_t i = 0; i < COLS; i++) {
            pinMode(colPins[i], INPUT_PULLUP);
        }
    }
    
    char getKey() {
        // Check if enough time has passed since last key press
        if ((millis() - lastDebounceTime) < DEBOUNCE_DELAY) {
            return '\0';
        }
        
        // Scan the keypad
        for (uint8_t row = 0; row < ROWS; row++) {
            // Set current row to LOW
            digitalWrite(rowPins[row], LOW);
            
            // Check each column in the current row
            for (uint8_t col = 0; col < COLS; col++) {
                if (digitalRead(colPins[col]) == LOW) {
                    // Key is pressed
                    digitalWrite(rowPins[row], HIGH);
                    lastDebounceTime = millis();
                    return keys[row][col];
                }
            }
            
            // Reset row to HIGH
            digitalWrite(rowPins[row], HIGH);
        }
        
        return '\0'; // No key pressed
    }
    
    // Get pressed key with blocking (waits for key press)
    char waitForKey() {
        char key;
        do {
            key = getKey();
        } while (key == '\0');
        return key;
    }
    
    // Change the key mapping if needed
    void setKeyMap(const char newKeys[ROWS][COLS]) {
        memcpy(keys, newKeys, ROWS * COLS);
    }
};

#endif // KEYPAD_H
