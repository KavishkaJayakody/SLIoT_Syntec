#ifndef KEY_RECEIVER_H
#define KEY_RECEIVER_H

#include <Arduino.h>
//#include "driver/gpio.h"  // Needed for gpio_install_isr_service
#include "Bus_Data.h"

enum Key{
    NONE,   KEY_A       ,KEY_B      ,KEY_C      ,KEY_D      ,KEY_E  ,
            KEY_F       ,KEY_G      ,KEY_1      ,KEY_2      ,KEY_3  ,
            KEY_H       ,KEY_I      ,KEY_4      ,KEY_5      ,KEY_6 ,
            KEY_J       ,KEY_UP     ,KEY_7      ,KEY_8      ,KEY_9 ,
            KEY_LEFT    ,KEY_DOWN   ,KEY_RIGHT  ,KEY_0      ,KEY_ENTER
};


class KeyReceiver {
public:
    static const int NUM_DATA_PINS = 5;
    const int DATA_PINS[NUM_DATA_PINS] = {21, 34, 35, 33, 32};  // Match with Pro Mini's A0–A4
    const int INTERRUPT_PIN = 19;  // Connected to Pro Mini's SIGNAL_PIN

    volatile bool dataReady = false;
    String current_text = "";
    String last_text = "";
    String last_key = "";


    KeyReceiver() {
        for (int i = 0; i < NUM_DATA_PINS; i++) {
            pinMode(DATA_PINS[i], INPUT_PULLDOWN);
        }
        pinMode(INTERRUPT_PIN, INPUT_PULLDOWN);

      instance = this;
    }

    void begin() {
        Serial.begin(115200);
        Serial.println("ESP32 Receiver Ready");

        // Ensure ISR service is installed
        if (gpio_install_isr_service(ESP_INTR_FLAG_LOWMED) != ESP_OK) {
            Serial.println("GPIO ISR service already installed or failed.");
        }

        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), isrWrapper, RISING);
    }

    String waitForInputUntil(int endKey = KEY_ENTER) {
        String inputText = "";
    
        //Serial.println("Waiting for input...");
    
        while (true) {
            //checkAndPrintKey();  // Optional: still prints to Serial
    
            if (dataReady) {
                dataReady = false;
    
                int value = 0;
                for (int i = 0; i < NUM_DATA_PINS; i++) {
                    value |= (digitalRead(DATA_PINS[i]) << i);
                }
    
                if (value == endKey) {
                    //Serial.println("End key received.");
                    last_text = current_text;
                    current_text = "";
                    break;
                } else {
                    String c = getKeyName(value);  // Optional: simple ASCII mapping
                    inputText += c;
                    current_text = inputText;  // Update current_text with the new input
                    Serial.print("Appended char: ");
                    Serial.println(c);
                }
            }
    
            delay(10);  // Small delay to avoid hogging CPU
        }
    
        return inputText;
    }

    void checkAndPrintKey() {
        if (dataReady) {
            dataReady = false;

            int value = 0;
            for (int i = 0; i < NUM_DATA_PINS; i++) {
                value |= (digitalRead(DATA_PINS[i]) << i);
            }

            Serial.print("Received Key: ");
            Serial.println(value);
        }
    }


    void check_Key_and_Execute() {
        if (dataReady) {
            dataReady = false;

            int value = 0;
            for (int i = 0; i < NUM_DATA_PINS; i++) {
                value |= (digitalRead(DATA_PINS[i]) << i);
            }
            Serial.print("Received Key: ");
            Serial.println(value);
            switch (value){
                case(KEY_UP):
                    bus.goToNextHalt();
                    Serial.println("Next Halt: ");
                    break;
            
                case(KEY_DOWN):
                    bus.goToPreviousHalt();
                    Serial.println("Previous Halt: ");
                    break;
            }
            

        
        }
        
    }
    int check_Key() {
        if (dataReady) {
            dataReady = false;

            int value = 0;
            for (int i = 0; i < NUM_DATA_PINS; i++) {
                value |= (digitalRead(DATA_PINS[i]) << i);
            }

            //Serial.print("Received Key: ");
            Serial.println(value);
            return value;
        }
        else {
            return NONE;  // Return NONE if no key is pressed
        }
        
    }
    const char* getKeyName(int value) {
        switch (value) {
            case KEY_A: return "A";
            case KEY_B: return "B";
            case KEY_C: return "C";
            case KEY_D: return "D";
            case KEY_E: return "E";
            case KEY_F: return "F";
            case KEY_G: return "G";
            case KEY_H: return "H";
            case KEY_UP: return "^";
            case KEY_DOWN: return "-";
            case KEY_LEFT: return "<";
            case KEY_RIGHT: return ">";
            case KEY_0: return "0";
            case KEY_1: return "1";
            case KEY_2: return "2";
            case KEY_3: return "3";
            case KEY_4: return "4";
            case KEY_5: return "5";
            case KEY_6: return "6";
            case KEY_7: return "7";
            case KEY_8: return "8";
            case KEY_9: return "9";
            case KEY_ENTER: return "/";
            default: return "!";
        }
    }
    

private:
    static KeyReceiver* instance;

    static void isrWrapper() {
        if (instance) {
            instance->onInterrupt();
        }
    }

    void onInterrupt() {
        dataReady = true;
    }
};

// Define the static member
extern KeyReceiver* keyReceiverInstance;
KeyReceiver* KeyReceiver::instance = nullptr;

#endif // KEY_RECEIVER_H
