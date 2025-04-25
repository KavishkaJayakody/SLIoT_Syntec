#include <Arduino.h>
#include "config.h"
#include "gps.h"
#include "gprs.h"
#include "printer.h"
#include "keypad.h"

// Create instances
GPS gps(&Serial2, GPS_RX_PIN, GPS_TX_PIN);
GPRS gprs(&Serial1, GPRS_RX_PIN, GPRS_TX_PIN);
Keypad keypad(KEYPAD_ROW_PINS, KEYPAD_COL_PINS);
Printer printer;

void handleKeypress(char key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    
    switch(key) {
        case 'A': // Send current location via SMS
            {
                float lat = gps.getLatitude();
                float lon = gps.getLongitude();
                char message[SMS_MESSAGE_BUFFER_SIZE];
                snprintf(message, sizeof(message), "Location: %.6f, %.6f", lat, lon);
                gprs.sendSMS(DEFAULT_SMS_NUMBER, message);
            }
            break;
            
        case 'B': // Reconnect TCP
            gprs.connectTCP(TCP_SERVER, TCP_PORT);
            break;
            
        case 'C': // Print current location to Serial
            Serial.printf("Lat: %.6f, Lon: %.6f\n", 
                        gps.getLatitude(), 
                        gps.getLongitude());
            break;
            
        case 'D': // Disconnect TCP
            gprs.disconnectTCP();
            break;
    }
}

void setup() {
    //Serial.begin(DEBUG_BAUD_RATE);  // Debug serial
    

    printer.begin();

    printer.printBill("ND-2314", "Syntech Transit(Pvt) Ltd", "0012", "2025/03/21", "15:23", "10000000001", "Moratuwa - Nittambuwa", "Katubedda", "Kadawatha", 1, 2, 260.00, "0703482664", "0332297800");
  
    printer.printBill("ND-2314", "Syntech Transit(Pvt) Ltd", "0012", "2025/03/21", "15:23", "10000000001", "Moratuwa - Nittambuwa", "Katubedda", "Kadawatha", 1, 2, 260.00, "0703482664", "0332297800");
    // Initialize GPRS
    if (gprs.begin()) {
        Serial.println("GPRS initialized successfully");
        
        // Initialize GPRS data connection
        if (gprs.initGPRS(GPRS_APN)) {
            Serial.println("GPRS data connection initialized");
            
            // Connect to TCP server
            if (gprs.connectTCP(TCP_SERVER, TCP_PORT)) {
                Serial.println("Connected to TCP server");
            } else {
                Serial.println("Failed to connect to TCP server");
            }
        }
    } else {
        Serial.println("GPRS initialization failed");
    }
}

void loop() {
    // Update GPS data
    gps.update();
    
    // Update GPRS (check for incoming messages/status)
    gprs.update();
    
    // Check for keypad input
    char key = keypad.getKey();
    if (key != '\0') {
        handleKeypress(key);
    }
    
    // Send data every interval
    static unsigned long lastSendTime = 0;
    if (millis() - lastSendTime > TCP_UPDATE_INTERVAL) {
        float lat = gps.getLatitude();
        float lon = gps.getLongitude();
        
        // Create JSON formatted data
        char tcpMessage[TCP_MESSAGE_BUFFER_SIZE];
        snprintf(tcpMessage, sizeof(tcpMessage), 
                "{\"device_id\":\"%s\",\"lat\":%.6f,\"lon\":%.6f,\"timestamp\":%lu}",
                DEVICE_ID, lat, lon, millis());
        
        // Send TCP data
        if (gprs.sendTCPData(tcpMessage)) {
            Serial.println("TCP data sent successfully");
        } else {
            Serial.println("Failed to send TCP data");
            // Try to reconnect
            gprs.connectTCP(TCP_SERVER, TCP_PORT);
        }
        
        lastSendTime = millis();
    }
}
