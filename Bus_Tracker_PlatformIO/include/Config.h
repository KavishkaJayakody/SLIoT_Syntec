#ifndef CONFIG_H
#define CONFIG_H

// Device Identification
#define DEVICE_ID "BUS001"

// Serial Port Configurations
#define GPS_SERIAL_NUM 2    // UART2
#define GPRS_SERIAL_NUM 1   // UART1
#define DEBUG_BAUD_RATE 115200
#define GPS_BAUD_RATE 9600
#define GPRS_BAUD_RATE 38400

// GPS Pin Configuration
#define GPS_RX_PIN GPIO_NUM_16
#define GPS_TX_PIN GPIO_NUM_17

// GPRS Pin Configuration
#define GPRS_RX_PIN GPIO_NUM_4
#define GPRS_TX_PIN GPIO_NUM_5

// Keypad Pin Configuration
const uint8_t KEYPAD_ROW_PINS[4] = {13, 12, 14, 27};
const uint8_t KEYPAD_COL_PINS[4] = {26, 25, 33, 32};

// GPRS Network Configuration
#define GPRS_APN "internet"
#define DEFAULT_SMS_NUMBER "+94712345678"

// TCP Server Configuration
#define TCP_SERVER "your-server.com"
#define TCP_PORT 8080

// Timing Configuration
#define TCP_UPDATE_INTERVAL 60000    // 1 minute in milliseconds
#define GPS_UPDATE_INTERVAL 1000     // 1 second in milliseconds

// Buffer Sizes
#define TCP_MESSAGE_BUFFER_SIZE 200
#define SMS_MESSAGE_BUFFER_SIZE 100

#endif // CONFIG_H 