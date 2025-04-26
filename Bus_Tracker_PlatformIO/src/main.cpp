#include <Arduino.h>
#include "Config.h"
#include "GPS.h"
//#include "GPRS.h"
#include "Key_Receiver.h"
#include "Bus_Ticket_Printer.h"
#include "Bus_Data.h"
#include "Bus_Data.h"
#include <Ticker.h>

// Define hardware serial ports
HardwareSerial gpsSerial(GPS_SERIAL_NUM);
//HardwareSerial gprsSerial(GPRS_SERIAL_NUM);

// Create instances
GPS gps(&gpsSerial, GPS_RX_PIN, GPS_TX_PIN);
//GPRS gprs(&gprsSerial, GPRS_RX_PIN, GPRS_TX_PIN);
KeyReceiver keyReceiver;
BusTicketPrinter ticketPrinter;
BusData bus;

Ticker gpsTicker;
Ticker timeTicker;
Ticker sendTicker;



void updateGPS() {
    gps.update();  // Parse new GPS data
    bus.latitude = gps.getLatitude();
    bus.longitude = gps.getLongitude();
    //Serial.printf("GPS Updated -> Lat: %.6f, Lon: %.6f\n", bus.latitude, bus.longitude);
}

void updateTime() {
    int h, m, s;
    if (sscanf(bus.time.c_str(), "%d:%d:%d", &h, &m, &s) == 3) {
        s += 1;
        if (s >= 60) {
            m += s / 60;
            s %= 60;
        }
        if (m >= 60) {
            h += m / 60;
            m %= 60;
        }
        if (h >= 24) {
            h %= 24;
        }

        char timeBuffer[9];
        snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", h, m, s);
        bus.time = String(timeBuffer);

        //Serial.printf("Time Updated -> %s %s\n", bus.date.c_str(), bus.time.c_str());
    } else {
        Serial.println("Invalid time format in bus.time");
    }
}


void setup() {
    bus.registrationNumber = "ND-2314";
    bus.companyName = "Syntech Transit (Pvt) Ltd";
    bus.routeNumber = "0012";
    bus.referenceNumber = "10000000001";
  
    bus.routeName = "Moratuwa - Nittambuwa";
    bus.fromHalt = "Katubedda";
    bus.toHalt = "Kadawatha";
    bus.isFullTicket = true;
  
    bus.date = "2025/03/21";
    bus.time = "09:00:00";
  
    bus.ticketCount = 2;
    bus.unitPrice = 130.00;
  
    bus.driverPhone = "0703482664";
    bus.hotline = "0332297800";

    // Initialize debug serial
    Serial.begin(115200);
    
    // Initialize GPS serial
    gpsSerial.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    
    // Initialize GPRS serial
    //gprsSerial.begin(GPRS_BAUD_RATE, SERIAL_8N1, GPRS_RX_PIN, GPRS_TX_PIN);

    keyReceiver.begin();

    gpsTicker.attach(1.0, updateGPS);   // every 2 seconds
    timeTicker.attach(1.0, updateTime); // every 5 seconds
    sendTicker.attach(2.0, []() {
        // Send GPS data to server
        char tcpMessage[TCP_MESSAGE_BUFFER_SIZE];
        snprintf(tcpMessage, sizeof(tcpMessage), 
                "{\"id\":\"%s\",\"lat\":%.6f,\"lon\":%.6f,\"tim\":%s,\"txt\":\"%s, \"halt\":\"%s\",\"pass\":%d,\"dest\":\"%s\"}",
                DEVICE_ID, bus.latitude, bus.longitude, bus.time, keyReceiver.current_text.c_str(),
                bus.getCurrentHaltName(), bus.passengerCount, bus.getDestinationHaltName());
        Serial.println(tcpMessage);
    });

    ticketPrinter.begin();

    // Print welcome message
    Serial.println("\nBus Ticketing System Ready");
    Serial.println("Key Functions:");
    Serial.println("1-9: Select number of passengers");
    Serial.println("A: Select destination halt");
    Serial.println("B: View halt statistics");
    Serial.println("C: Print ticket");
    Serial.println("UP/DOWN: Navigate between halts");
    Serial.println("Current Halt: " + String(bus.getCurrentHaltName()));

    ticketPrinter.setTicketData(
        "ND-2314", "Syntech Transit(Pvt) Ltd", "0012", "2025/03/21", "15:23",
        "10000000001", "Moratuwa - Nittambuwa", "Katubedda", "Kadawatha",
        true, 2, 260.00, "0703482664", "0332297800"
    );
    

    // Serial.println("Ticket printing Test.");
    // ticketPrinter.printTicket();
    // Serial.println("Ticket printed successfully.");
    // delay(2000); // Wait for the printer to finish printing

    
}

void loop() {
    //keyReceiver.waitForInputUntil(KEY_ENTER); // Wait for ENTER key
    keyReceiver.check_Key_and_Execute();
    //bus.goToNextHalt();
    //delay(10000);

}


