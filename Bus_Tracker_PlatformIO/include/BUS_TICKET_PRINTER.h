#ifndef BUS_TICKET_PRINTER_H
#define BUS_TICKET_PRINTER_H

#include <Arduino.h>
#include "Adafruit_Thermal.h"
#include "config.h"

#define TX_PIN 18
#define RX_PIN 19

class BusTicketPrinter {
public:
    BusTicketPrinter()
        : serialPort(1), printer(&serialPort) {}

    void begin() {
        serialPort.begin(9600, SERIAL_8N1, PRINTER_RX_PIN, PRINTER_TX_PIN);
        printer.begin();
    }

    void setTicketData(
        const String& regNum,
        const String& comName,
        const String& routeNo,
        const String& date,
        const String& time,
        const String& refNo,
        const String& route,
        const String& from,
        const String& to,
        bool isFull,
        uint8_t numTickets,
        float unitPrice,
        const String& contact,
        const String& hotline
    ) {
        this->regNum = regNum;
        this->comName = comName;
        this->routeNo = routeNo;
        this->date = date;
        this->time = time;
        this->refNo = refNo;
        this->route = route;
        this->from = from;
        this->to = to;
        this->isFull = isFull;
        this->numTickets = numTickets;
        this->unitPrice = unitPrice;
        this->contact = contact;
        this->hotline = hotline;
    }

    void printTicket() {
        printer.wake();
        printer.setDefault();

        printer.justify('C');
        printer.setFont('B');
        printer.setSize('S');
        printer.println(regNum);

        printer.setSize('M');
        printer.println(comName);

        printer.setSize('S');
        printer.justify('L');
        printer.print(F("Route No: "));
        printer.print(routeNo);
        printer.print(F("  "));
        printer.justify('R');
        printer.print(F("Ref: "));
        printer.println(refNo);

        printer.justify('L');
        printer.print(date);
        printer.print(F("  "));
        printer.justify('R');
        printer.println(time);

        printer.justify('L');
        printer.println(route);

        printer.setSize('M');
        printer.justify('L');
        printer.print(F("From: "));
        printer.print(from);
        printer.justify('R');
        printer.print(F("  To: "));
        printer.println(to);

        printer.setSize('S');
        printer.justify('C');

        printer.print(isFull ? F("Full") : F("Half"));
        printer.print(F("  "));
        printer.print(numTickets);
        printer.print(F("x"));
        printer.print(unitPrice);
        printer.print(F(" = "));
        float total = numTickets * unitPrice;
        printer.println(total);

        printer.setSize('M');
        printer.boldOn();
        printer.print(F("Total : "));
        printer.println(total);
        printer.boldOff();

        printer.setSize('S');
        printer.println();
        printer.print(F("Contact : "));
        printer.println(contact);
        printer.print(F("Contact : "));
        printer.println(hotline);

        printer.println(F("___________________________"));
        printer.sleep();
    }

private:
    HardwareSerial serialPort;
    Adafruit_Thermal printer;

    String regNum, comName, routeNo, date, time, refNo;
    String route, from, to, contact, hotline;
    bool isFull;
    uint8_t numTickets;
    float unitPrice;
};

#endif // BUS_TICKET_PRINTER_H
