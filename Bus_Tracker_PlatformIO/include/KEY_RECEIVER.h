#ifndef KEY_RECEIVER_H
#define KEY_RECEIVER_H

#include <Arduino.h>
#include "Bus_Data.h"
#include "Bus_Ticket_Printer.h"

// External objects defined in main.cpp
extern BusData bus;
extern BusTicketPrinter ticketPrinter;

// 5-bit keypad codes
enum Key {
    NONE,
    KEY_ESC,   // Escape / cancel
    KEY_F1,    // Function 1
    KEY_CHG,   // Change
    KEY_STBY,  // Standby
    KEY_ON,    // On
    KEY_PRT,   // Print ticket
    KEY_F2,    // Function 2 / show stats
    KEY_1, KEY_2, KEY_3,
    KEY_RT,    // Select route
    KEY_F3,    // Function 3
    KEY_4, KEY_5, KEY_6,
    KEY_F4,    // Function 4
    KEY_UP,    // Next halt
    KEY_7, KEY_8, KEY_9,
    KEY_LEFT,  // Navigate left
    KEY_DOWN,  // Previous halt
    KEY_RIGHT, // Navigate right
    KEY_0,
    KEY_ENTER // Confirm / Enter
};

enum class SystemState {
    IDLE,
    SELECTING_DESTINATION,
    PRINTING_TICKET,
    SHOWING_STATS
};

class KeyReceiver {
public:
    static const int NUM_DATA_PINS = 5;
    const int DATA_PINS[NUM_DATA_PINS] = {21, 34, 35, 33, 32};
    const int INTERRUPT_PIN       = 19;

    volatile bool dataReady      = false;
    SystemState currentState     = SystemState::IDLE;
    String tempInput             = "";

    KeyReceiver() {
        for (int i = 0; i < NUM_DATA_PINS; i++) {
            pinMode(DATA_PINS[i], INPUT_PULLDOWN);
        }
        pinMode(INTERRUPT_PIN, INPUT_PULLDOWN);
        instance = this;
    }

    void begin() {
        // Install ISR service once
        if (gpio_install_isr_service(ESP_INTR_FLAG_LOWMED) != ESP_OK) {
            Serial.println("GPIO ISR service already installed or failed.");
        }
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN),
                        isrWrapper, RISING);
    }

    void check_Key_and_Execute() {
        if (!dataReady) return;
        dataReady = false;

        int code = 0;
        for (int i = 0; i < NUM_DATA_PINS; i++) {
            code |= (digitalRead(DATA_PINS[i]) << i);
        }
        // Print the enum name instead of numeric value
        Serial.print("Received Key: ");
        Serial.println(getKeyName(code));

        switch (currentState) {
            case SystemState::IDLE:
                handleIdleState(code);
                break;

            case SystemState::SELECTING_DESTINATION:
                handleDestinationSelection(code);
                break;

            case SystemState::PRINTING_TICKET:
                if (code == KEY_ENTER) currentState = SystemState::IDLE;
                break;

            case SystemState::SHOWING_STATS:
                if (code == KEY_ENTER) currentState = SystemState::IDLE;
                break;
        }
    }

    const char* getKeyName(int code) {
        switch (code) {
            case KEY_ESC:   return "KEY_ESC";
            case KEY_F1:    return "KEY_F1";
            case KEY_CHG:   return "KEY_CHG";
            case KEY_STBY:  return "KEY_STBY";
            case KEY_ON:    return "KEY_ON";
            case KEY_PRT:   return "KEY_PRT";
            case KEY_F2:    return "KEY_F2";
            case KEY_1:     return "KEY_1";
            case KEY_2:     return "KEY_2";
            case KEY_3:     return "KEY_3";
            case KEY_RT:    return "KEY_RT";
            case KEY_F3:    return "KEY_F3";
            case KEY_4:     return "KEY_4";
            case KEY_5:     return "KEY_5";
            case KEY_6:     return "KEY_6";
            case KEY_F4:    return "KEY_F4";
            case KEY_UP:    return "KEY_UP";
            case KEY_7:     return "KEY_7";
            case KEY_8:     return "KEY_8";
            case KEY_9:     return "KEY_9";
            case KEY_LEFT:  return "KEY_LEFT";
            case KEY_DOWN:  return "KEY_DOWN";
            case KEY_RIGHT: return "KEY_RIGHT";
            case KEY_0:     return "KEY_0";
            case KEY_ENTER: return "KEY_ENTER";
            default:        return "NONE";
        }
    }

private:
    static KeyReceiver* instance;

    // ISR wrapper now in flash (no IRAM_ATTR) to avoid relocation errors
    static void isrWrapper() {
        if (instance) instance->onInterrupt();
    }

    void onInterrupt() {
        dataReady = true;
    }

    void handleIdleState(int code) {
        switch (code) {
            case KEY_UP:
                bus.goToNextHalt();
                Serial.println("Next Halt: " + String(bus.getCurrentHaltName()));
                break;

            case KEY_DOWN:
                bus.goToPreviousHalt();
                Serial.println("Previous Halt: " + String(bus.getCurrentHaltName()));
                break;

            case KEY_RT:
                Serial.println("Enter destination halt number (1-9):");
                currentState = SystemState::SELECTING_DESTINATION;
                tempInput = "";
                break;

            case KEY_F2:
                bus.printHaltStats();
                currentState = SystemState::SHOWING_STATS;
                break;

            case KEY_PRT:
                if (bus.passengerCount > 0) {
                    printTicket();
                    currentState = SystemState::IDLE;
                } else {
                    Serial.println("No passengers selected");
                }
                break;

            case KEY_ESC:
                currentState = SystemState::IDLE;
                Serial.println("Cancelled, back to idle");
                break;

            default:
                // Direct ticket issue on numeric keys 1–9
                if (code >= KEY_1 && code <= KEY_9) {
                    int cnt = code - KEY_0;
                    bus.issueTicket(cnt);
                    Serial.printf("Issued %d tickets\n", cnt);
                }
                break;
        }
    }

    void handleDestinationSelection(int code) {
        if (code == KEY_ENTER) {
            int idx = tempInput.toInt() - 1;
            if (idx >= 0 && idx < BusData::NUM_HALTS) {
                bus.setDestinationHalt(idx);
                Serial.println("Destination: " + String(bus.getDestinationHaltName()));
            }
            currentState = SystemState::IDLE;
        }
        else if (code >= KEY_1 && code <= KEY_9) {
            tempInput += getKeyName(code);
            Serial.println("Selecting: " + tempInput);
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
        Serial.println("Ticket printed");
    }
};

// Define static instance
KeyReceiver* KeyReceiver::instance = nullptr;

#endif // KEY_RECEIVER_H
