#ifndef KEY_RECEIVER_H
#define KEY_RECEIVER_H

#include <Arduino.h>
//#include "driver/gpio.h"  // Needed for gpio_install_isr_service
#include "Bus_Data.h"
#include "Bus_Ticket_Printer.h"

// Forward declarations
extern BusData bus;
extern BusTicketPrinter ticketPrinter;

enum Key{
    NONE,   KEY_A       ,KEY_B      ,KEY_C      ,KEY_D      ,KEY_E  ,
            KEY_F       ,KEY_G      ,KEY_1      ,KEY_2      ,KEY_3  ,
            KEY_H       ,KEY_I      ,KEY_4      ,KEY_5      ,KEY_6 ,
            KEY_J       ,KEY_UP     ,KEY_7      ,KEY_8      ,KEY_9 ,
            KEY_LEFT    ,KEY_DOWN   ,KEY_RIGHT  ,KEY_0      ,KEY_ENTER
};

enum class SystemState {
    IDLE,
    SELECTING_PASSENGERS,
    SELECTING_DESTINATION,
    PRINTING_TICKET,
    SHOWING_STATS
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
    SystemState currentState = SystemState::IDLE;
    String tempInput = "";

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
        if (!dataReady) return;
        
        dataReady = false;
        int value = 0;
        for (int i = 0; i < NUM_DATA_PINS; i++) {
            value |= (digitalRead(DATA_PINS[i]) << i);
        }
        Serial.print("Received Key: ");
        Serial.println(value);

        switch (currentState) {
            case SystemState::IDLE:
                handleIdleState(value);
                break;
                
            case SystemState::SELECTING_DESTINATION:
                handleDestinationSelection(value);
                break;
                
            case SystemState::SELECTING_PASSENGERS:
                handlePassengerSelection(value);
                break;
                
            case SystemState::PRINTING_TICKET:
                handleTicketPrinting(value);
                break;
                
            case SystemState::SHOWING_STATS:
                handleStatsDisplay(value);
                break;
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

    void handleIdleState(int value) {
        switch (value) {
            case KEY_UP:
                bus.goToNextHalt();
                Serial.println("Next Halt: " + String(bus.getCurrentHaltName()));
                break;
                
            case KEY_DOWN:
                bus.goToPreviousHalt();
                Serial.println("Previous Halt: " + String(bus.getCurrentHaltName()));
                break;
                
            case KEY_A:
                Serial.println("Enter destination halt number (1-9):");
                currentState = SystemState::SELECTING_DESTINATION;
                tempInput = "";
                break;
                
            case KEY_B:
                bus.printHaltStats();
                currentState = SystemState::SHOWING_STATS;
                break;
                
            case KEY_C:
                if (bus.passengerCount > 0) {
                    currentState = SystemState::PRINTING_TICKET;
                    printTicket();
                } else {
                    Serial.println("No passengers selected");
                }
                break;
                
            default:

            
                String key = getKeyName(value);
                if (key.toInt() >= 1 && key.toInt() <=9) {
                    bus.issueTicket(key.toInt());
                    Serial.printf("Issued %d tickets\n", key.toInt());
                    
                }

                break;
        }
    }

    void handleDestinationSelection(int value) {
        if (value == KEY_ENTER) {
            int destIndex = tempInput.toInt() - 1;
            if (destIndex >= 0 && destIndex < BusData::NUM_HALTS) {
                bus.setDestinationHalt(destIndex);
                Serial.printf("Destination set to: %s\n", bus.getDestinationHaltName());
            }
            currentState = SystemState::IDLE;
        } else {
            String key = getKeyName(value);
            if (key.toInt() >= 1 && key.toInt() <=9) {
            tempInput += key;
            Serial.println("Destination: " + tempInput);
        }
        }
    }

    void handlePassengerSelection(int value) {
        if (value == KEY_ENTER) {
            int count = tempInput.toInt();
            if (count > 0) {
                bus.issueTicket(count);
                Serial.printf("Issued %d tickets\n", count);
            }
            currentState = SystemState::IDLE;
        } else {
            String key = getKeyName(value);
            if (key.toInt() >= 1 && key.toInt() <=9) {
            tempInput += key;
            Serial.println("Passengers: " + tempInput);
        }
        }
    }

    void handleTicketPrinting(int value) {
        if (value == KEY_ENTER) {
            currentState = SystemState::IDLE;
        }
    }

    void handleStatsDisplay(int value) {
        if (value == KEY_ENTER) {
            currentState = SystemState::IDLE;
        }
    }

    void printTicket() {
        ticketPrinter.setTicketData(
            bus.registrationNumber,
            bus.companyName,
            bus.routeNumber,
            bus.date,
            bus.time,
            bus.referenceNumber,
            bus.routeName,
            bus.getCurrentHaltName(),
            bus.getDestinationHaltName(),
            bus.isFullTicket,
            bus.passengerCount,
            bus.unitPrice * bus.passengerCount,
            bus.driverPhone,
            bus.hotline
        );
        ticketPrinter.printTicket();
        Serial.println("Ticket printed successfully");
    }

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
