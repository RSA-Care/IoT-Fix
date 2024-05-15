#if !defined(OLED_h)
#define OLED_h

#include <Arduino.h>

#define OLED_I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin

void oledBegin();
void print(String message);
void println(String message);
void clearScreen();

#endif // OLED_h
