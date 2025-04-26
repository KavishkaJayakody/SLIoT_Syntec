#include <Arduino.h>
#include "Config.h"
#include "GPS.h"
#include "Key_Receiver.h"
#include "Bus_Ticket_Printer.h"
#include "Bus_Data.h"
#include "Bus_Display.h"
#include <Ticker.h>

// Define hardware serial ports
HardwareSerial gpsSerial(GPS_SERIAL_NUM);

// Create instances
GPS gps(&gpsSerial, GPS_RX_PIN, GPS_TX_PIN);
KeyReceiver keyReceiver;
BusTicketPrinter ticketPrinter;
BusDisplay display;
BusData bus;

// Ticker objects
Ticker gpsTicker;
Ticker timeTicker;
Ticker sendTicker;
Ticker displayTicker;

void updateGPS() {
    gps.update();  // Parse new GPS data
    bus.latitude = gps.getLatitude();
    bus.longitude = gps.getLongitude();
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
        
        // Update the time on the display as well
        display.setTime(h, m, s);
    } else {
        Serial.println("Invalid time format in bus.time");
    }
}

// LVGL update function for the ticker
void updateDisplay() {
    display.update();  // Call lv_timer_handler() inside
}

void setup() {
    // Initialize bus data
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
    
    // Initialize components
    keyReceiver.begin();
    ticketPrinter.begin();
    display.begin();
    
    // Set the location on display
    display.setLocation(bus.fromHalt.c_str());
    
    // Parse initial time and set display
    int h, m, s;
    if (sscanf(bus.time.c_str(), "%d:%d:%d", &h, &m, &s) == 3) {
        display.setTime(h, m, s);
    }
    
    // Set date on display (assuming a date format of YYYY/MM/DD)
    int y, mo, d;
    if (sscanf(bus.date.c_str(), "%d/%d/%d", &y, &mo, &d) == 3) {
        // Calculate the day of week (simple algorithm, may not be accurate for all dates)
        // This is just for demonstration - in a real app, use a proper calendar library
        int weekday = (d + mo + y + (y/4)) % 7;
        display.setDate(y, mo-1, d, weekday); // month-1 because our array is 0-indexed
    }
    
    // Set up tickers
    gpsTicker.attach(1.0, updateGPS);     // Update GPS every second
    timeTicker.attach(1.0, updateTime);   // Update time every second
    displayTicker.attach(0.02, updateDisplay); // Update LVGL at 50Hz
    
    sendTicker.attach(2.0, []() {
        // Send GPS data to server
        char tcpMessage[TCP_MESSAGE_BUFFER_SIZE];
        snprintf(tcpMessage, sizeof(tcpMessage), 
                "{\"id\":\"%s\",\"lat\":%.6f,\"lon\":%.6f,\"tim\":%s,\"txt\":\"%s, \"halt\":\"%s\",\"pass\":%d,\"dest\":\"%s\"}",
                DEVICE_ID, bus.latitude, bus.longitude, bus.time.c_str(), keyReceiver.current_text.c_str(),
                bus.getCurrentHaltName(), bus.passengerCount, bus.getDestinationHaltName());
        Serial.println(tcpMessage);
    });

    // Set ticket printer data
    ticketPrinter.setTicketData(
        bus.registrationNumber,
        bus.companyName,
        bus.routeNumber,
        bus.date,
        bus.time,
        bus.referenceNumber,
        bus.routeName,
        bus.fromHalt,
        bus.toHalt,
        bus.isFullTicket,
        bus.ticketCount,
        bus.unitPrice,
        bus.driverPhone,
        bus.hotline
    );

    // Print welcome message
    Serial.println("\nBus Ticketing System Ready");
    Serial.println("Key Functions:");
    Serial.println("1-9: Select number of passengers");
    Serial.println("A: Select destination halt");
    Serial.println("B: View halt statistics");
    Serial.println("C: Print ticket");
    Serial.println("UP/DOWN: Navigate between halts");
    Serial.println("Current Halt: " + String(bus.getCurrentHaltName()));
}

void loop() {
    keyReceiver.check_Key_and_Execute();
    
    // Note: No need to call lv_timer_handler() here
    // because it's being handled by the displayTicker
    
    // No need for delay in the main loop
}