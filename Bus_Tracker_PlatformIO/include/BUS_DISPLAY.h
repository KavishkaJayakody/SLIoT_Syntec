#ifndef BUS_DISPLAY_H
#define BUS_DISPLAY_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "Config.h"

class BusDisplay {
public:
    BusDisplay() {
        // Initialize buffer pointer to null
        buf1 = nullptr;
    }

    ~BusDisplay() {
        // Free the buffer memory when done
        if (buf1) {
            free(buf1);
            buf1 = nullptr;
        }
    }

    void begin() {
        // Initialize the LCD
        lcd.begin();
        lcd.setRotation(3);
        lcd.fillScreen(TFT_BLACK);
        delay(100);
        
        // Initialize LVGL
        lv_init();
        
        // Allocate the buffer with the EXACT same size as working code
        // This is critical - use the exact same buffer size as in the working code
        buf1 = (lv_color_t*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT / 10 * sizeof(lv_color_t));
        if (!buf1) {
            Serial.println("Failed to allocate LVGL buffer memory!");
            return;
        }
        
        // Initialize LVGL draw buffer
        lv_disp_draw_buf_init(&draw_buf, buf1, NULL, SCREEN_WIDTH * SCREEN_HEIGHT / 10);
        
        // Register our flush callback
        static lv_disp_drv_t disp_drv;
        lv_disp_drv_init(&disp_drv);
        disp_drv.hor_res = SCREEN_WIDTH;
        disp_drv.ver_res = SCREEN_HEIGHT;
        disp_drv.flush_cb = flush_cb;
        disp_drv.draw_buf = &draw_buf;
        disp_drv.user_data = &lcd; // Store pointer to our lcd object
        lv_disp_drv_register(&disp_drv);
        
        // Create UI elements
        createUI();
        
        Serial.println("Display initialized");
    }

    void update() {
        // Process LVGL tasks
        lv_timer_handler();
    }

    void setTime(int h, int m, int s) {
        hours = h;
        minutes = m;
        seconds = s;
        displayTime();
    }

    void setDate(int y, int mo, int d, int wd) {
        year = y;
        month = mo;
        day = d;
        weekday = wd;
        displayTime();
    }

    void setLocation(const char* loc) {
        location = loc;
        if (locationLabel) {  // Check if UI has been created
            lv_label_set_text(locationLabel, location);
            lv_obj_align(locationLabel, LV_ALIGN_TOP_RIGHT, -10, 5);
        }
    }

    void displayTime() {
        if (!timeLabel || !dateLabel) return;  // Safety check
        
        char timeStr[9];
        char dateStr[32];

        // Format time as HH:MM:SS
        sprintf(timeStr, "%02d:%02d:%02d", hours, minutes, seconds);

        // Format date as Weekday, Month Day, Year
        sprintf(dateStr, "%s, %s %d, %d", weekdays[weekday], months[month], day, year);

        // Set the text and align
        lv_label_set_text(timeLabel, timeStr);
        lv_obj_align(timeLabel, LV_ALIGN_CENTER, 0, -20);

        lv_label_set_text(dateLabel, dateStr);
        lv_obj_align(dateLabel, LV_ALIGN_CENTER, 0, 30);
    }

    void updateBattery(int batteryLevel) {
        if (!batteryLabel || !batteryIcon) return;  // Safety check
        
        battery = batteryLevel;
        char batteryStr[8];
        sprintf(batteryStr, "%d%%", battery);
        lv_label_set_text(batteryLabel, batteryStr);

        // Update battery icon based on level
        if (battery > 75) {
            lv_label_set_text(batteryIcon, LV_SYMBOL_BATTERY_FULL);
        } else if (battery > 50) {
            lv_label_set_text(batteryIcon, LV_SYMBOL_BATTERY_3);
        } else if (battery > 25) {
            lv_label_set_text(batteryIcon, LV_SYMBOL_BATTERY_2);
        } else if (battery > 10) {
            lv_label_set_text(batteryIcon, LV_SYMBOL_BATTERY_1);
        } else {
            lv_label_set_text(batteryIcon, LV_SYMBOL_BATTERY_EMPTY);
        }
    }

private:
    TFT_eSPI lcd;
    lv_disp_draw_buf_t draw_buf;
    lv_color_t* buf1;

    lv_obj_t *timeLabel = nullptr;
    lv_obj_t *dateLabel = nullptr;
    lv_obj_t *batteryLabel = nullptr;
    lv_obj_t *batteryIcon = nullptr;
    lv_obj_t *locationLabel = nullptr;

    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    int day = 1;
    int month = 0;
    int year = 2025;
    int weekday = 0;
    int battery = 100;
    const char *location = "";

    const char *weekdays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    const char *months[12] = {"January", "February", "March", "April", "May", "June", "July",
                             "August", "September", "October", "November", "December"};

    void createUI() {
        // Create styles
        static lv_style_t timeStyle;
        lv_style_init(&timeStyle);
        lv_style_set_text_color(&timeStyle, lv_color_hex(0xFFFFFF));
        lv_style_set_text_font(&timeStyle, &lv_font_montserrat_24); // Larger font for time
        
        static lv_style_t dateStyle;
        lv_style_init(&dateStyle);
        lv_style_set_text_color(&dateStyle, lv_color_hex(0xCCCCCC));
        lv_style_set_text_font(&dateStyle, &lv_font_montserrat_24); // Medium font for date

        static lv_style_t headerStyle;
        lv_style_init(&headerStyle);
        lv_style_set_text_color(&headerStyle, lv_color_hex(0xAAAAAA));
        lv_style_set_text_font(&headerStyle, &lv_font_montserrat_14); // Smaller font for header

        // Create a gradient background
        lv_obj_t *screen = lv_scr_act();

        static lv_style_t style_bg;
        lv_style_init(&style_bg);
        lv_style_set_bg_color(&style_bg, lv_color_hex(0x000428));       // Dark blue
        lv_style_set_bg_grad_color(&style_bg, lv_color_hex(0x004e92));  // Lighter blue
        lv_style_set_bg_grad_dir(&style_bg, LV_GRAD_DIR_VER);

        lv_obj_add_style(screen, &style_bg, 0);

        // Battery icon and percentage at top left
        batteryIcon = lv_label_create(screen);
        lv_label_set_text(batteryIcon, LV_SYMBOL_BATTERY_FULL);
        lv_obj_set_pos(batteryIcon, 5, 5);
        lv_obj_add_style(batteryIcon, &headerStyle, 0);

        batteryLabel = lv_label_create(screen);
        lv_label_set_text_fmt(batteryLabel, "%d%%", battery);
        lv_obj_set_pos(batteryLabel, 25, 5);
        lv_obj_add_style(batteryLabel, &headerStyle, 0);

        // Location at top right
        lv_obj_t *locationIcon = lv_label_create(screen);
        lv_label_set_text(locationIcon, LV_SYMBOL_HOME);
        lv_obj_align(locationIcon, LV_ALIGN_TOP_RIGHT, -95, 5);
        lv_obj_add_style(locationIcon, &headerStyle, 0);

        locationLabel = lv_label_create(screen);
        lv_label_set_text(locationLabel, location);
        lv_obj_align(locationLabel, LV_ALIGN_TOP_RIGHT, -10, 5);
        lv_obj_add_style(locationLabel, &headerStyle, 0);

        // Large digital time in center
        timeLabel = lv_label_create(screen);
        lv_obj_add_style(timeLabel, &timeStyle, 0);
        lv_obj_align(timeLabel, LV_ALIGN_CENTER, 0, -20);

        // Date below time
        dateLabel = lv_label_create(screen);
        lv_obj_add_style(dateLabel, &dateStyle, 0);
        lv_obj_align(dateLabel, LV_ALIGN_CENTER, 0, 30);

        // Update display immediately
        displayTime();
        updateBattery(battery);
    }

    // Improved callback for LVGL display driver
    static void flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
        TFT_eSPI* lcd_ptr = (TFT_eSPI*)disp->user_data;
        
        if (lcd_ptr == nullptr) {
            Serial.println("Error: LCD pointer is null in display flush callback");
            lv_disp_flush_ready(disp);
            return;
        }
        
        uint32_t w = (area->x2 - area->x1 + 1);
        uint32_t h = (area->y2 - area->y1 + 1);

        lcd_ptr->startWrite();
        lcd_ptr->setAddrWindow(area->x1, area->y1, w, h);
        lcd_ptr->pushColors((uint16_t*)&color_p->full, w * h, true);
        lcd_ptr->endWrite();

        lv_disp_flush_ready(disp);
    }
};

#endif // BUS_DISPLAY_H