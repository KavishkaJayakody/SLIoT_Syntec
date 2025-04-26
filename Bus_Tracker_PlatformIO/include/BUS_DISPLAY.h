#ifndef BUS_DISPLAY_H
#define BUS_DISPLAY_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "Config.h"

class BusDisplay {
public:
    BusDisplay() {}

    void begin();
    void update();
    void displayTime();
    void updateBattery(int batteryLevel);
    void setTime(int hours, int minutes, int seconds);
    void setDate(int year, int month, int day, int weekday);
    void setLocation(const char* location);
    void createUI();

private:
    TFT_eSPI lcd;
    lv_disp_draw_buf_t draw_buf;
    lv_color_t *buffer;
    
    lv_obj_t *timeLabel;
    lv_obj_t *dateLabel;
    lv_obj_t *batteryLabel;
    lv_obj_t *batteryIcon;
    lv_obj_t *locationLabel;
    
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
    
    static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
};

#endif // BUS_DISPLAY_H

void BusDisplay::begin() {
    // Initialize the LCD
    lcd.begin();
    lcd.setRotation(1);
    lcd.fillScreen(TFT_BLACK);
    
    // Initialize LVGL
    lv_init();
    
    // Allocate buffer for LVGL
    buffer = (lv_color_t*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT / 10 * sizeof(lv_color_t));
    if (buffer == NULL) {
        Serial.println("LVGL buffer allocation failed!");
        return;
    }
    
    // Initialize LVGL draw buffer
    lv_disp_draw_buf_init(&draw_buf, buffer, NULL, SCREEN_WIDTH * SCREEN_HEIGHT / 10);
    
    // Initialize and register the display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    
    // Create UI elements
    createUI();
    
    Serial.println("Display initialized");
}

void BusDisplay::update() {
    // This function will be called regularly by the ticker
    // to ensure LVGL tasks are processed
    lv_timer_handler();
}

void BusDisplay::setTime(int h, int m, int s) {
    hours = h;
    minutes = m;
    seconds = s;
    displayTime();
}

void BusDisplay::setDate(int y, int mo, int d, int wd) {
    year = y;
    month = mo;
    day = d;
    weekday = wd;
    displayTime();
}

void BusDisplay::setLocation(const char* loc) {
    location = loc;
    lv_label_set_text(locationLabel, location);
}

void BusDisplay::my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    TFT_eSPI* lcd_ptr = (TFT_eSPI*)disp->user_data;
    lcd_ptr->startWrite();
    lcd_ptr->setAddrWindow(area->x1, area->y1, w, h);
    lcd_ptr->pushColors((uint16_t*)&color_p->full, w * h, true);
    lcd_ptr->endWrite();

    lv_disp_flush_ready(disp);
}

void BusDisplay::createUI() {
    // Create styles
    static lv_style_t timeStyle;
    lv_style_init(&timeStyle);
    lv_style_set_text_color(&timeStyle, lv_color_hex(0xFFFFFF));

    static lv_style_t dateStyle;
    lv_style_init(&dateStyle);
    lv_style_set_text_color(&dateStyle, lv_color_hex(0xCCCCCC));

    static lv_style_t headerStyle;
    lv_style_init(&headerStyle);
    lv_style_set_text_color(&headerStyle, lv_color_hex(0xAAAAAA));

    // Create a beautiful gradient background
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

void BusDisplay::displayTime() {
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

void BusDisplay::updateBattery(int batteryLevel) {
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