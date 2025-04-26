#ifndef BUS_DATA_H
#define BUS_DATA_H

#include <Arduino.h>

class BusData;

extern BusData bus;

class BusData {
public:
    // Identification
    String registrationNumber;
    String companyName;
    String routeNumber;
    String referenceNumber;

    // Travel Info
    String routeName;
    String fromHalt;
    String toHalt;
    bool isFullTicket;

    // Date & Time
    String date;
    String time;

    // Ticket Info
    uint8_t ticketCount = 0;
    float unitPrice = 0.0f;

    // Contact Info
    String driverPhone;
    String hotline;

    // GPS Location
    float latitude = 0.0f;
    float longitude = 0.0f;

    // Bus halts along Galkissa – Piliyandala route
    enum Halt {
        GALKISSA,
        RATMALANA,
        ANGULANA,
        MOUNT_LAVINIA,
        DEHIWALA,
        KOHUWALA,
        NUGEGODA,
        WIJERAMA,
        NAVINNA,
        MAHARAGAMA,
        BORALASGAMUWA,
        POLHENA,
        KAHATHUDUWA,
        PANDURA,
        PILIYANDALA,
        NUM_HALTS
    };

    static const char* haltNames[NUM_HALTS];

    // Track current halt index
    uint8_t currentHaltIndex = 0;

    // Functions to access and manipulate current halt
    const char* getCurrentHaltName() const {
        return haltNames[currentHaltIndex];
    }

    void goToNextHalt() {
        if (currentHaltIndex < NUM_HALTS - 1) {
            currentHaltIndex++;
            //Serial.println(getCurrentHaltName());
        }
    }

    void goToPreviousHalt() {
        if (currentHaltIndex > 0) {
            currentHaltIndex--;
            //Serial.println(getCurrentHaltName());
        }
    }

    BusData() = default;
};

// Define halt names
const char* BusData::haltNames[BusData::NUM_HALTS] = {
    "Galkissa",
    "Ratmalana",
    "Angulana",
    "Mount Lavinia",
    "Dehiwala",
    "Kohuwala",
    "Nugegoda",
    "Wijerama",
    "Navinna",
    "Maharagama",
    "Boralesgamuwa",
    "Polhena",
    "Kahathuduwa",
    "Pandura",
    "Piliyandala"
};

#endif // BUS_DATA_H
