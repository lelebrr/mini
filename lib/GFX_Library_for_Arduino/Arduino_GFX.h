#ifndef ARDUINO_GFX_H
#define ARDUINO_GFX_H

#include <Arduino.h>

// Arduino GFX Library - Simplified version for SH8601
// This is a placeholder until we get the actual library from Waveshare demo

class Arduino_GFX {
public:
  Arduino_GFX(int16_t w, int16_t h) : _width(w), _height(h) {}
  
  virtual void begin() = 0;
  virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
  virtual void fillScreen(uint16_t color) = 0;
  virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) = 0;
  virtual void setCursor(int16_t x, int16_t y) = 0;
  virtual void setTextColor(uint16_t color) = 0;
  virtual void setTextSize(uint8_t size) = 0;
  virtual void print(const char *text) = 0;
  virtual void println(const char *text) = 0;
  
  int16_t width() const { return _width; }
  int16_t height() const { return _height; }

protected:
  int16_t _width;
  int16_t _height;
};

class Arduino_SH8601 : public Arduino_GFX {
public:
  Arduino_SH8601(class Arduino_DataBus *bus, int8_t rst, uint8_t rotation, bool ips, int16_t w, int16_t h)
    : Arduino_GFX(w, h), _bus(bus), _rst(rst), _rotation(rotation), _ips(ips) {}
  
  void begin() override {
    // Initialize display
    if (_rst >= 0) {
      pinMode(_rst, OUTPUT);
      digitalWrite(_rst, HIGH);
      delay(10);
      digitalWrite(_rst, LOW);
      delay(10);
      digitalWrite(_rst, HIGH);
      delay(10);
    }
    
    // Send initialization commands
    _bus->writeCommand(0x01); // Software reset
    delay(20);
    
    _bus->writeCommand(0x36); // Memory data access control
    _bus->writeData(0x00);
    
    _bus->writeCommand(0x3A); // Interface pixel format
    _bus->writeData(0x55);
    
    _bus->writeCommand(0xB2); // Porch control
    _bus->writeData(0x0C);
    _bus->writeData(0x0C);
    _bus->writeData(0x00);
    _bus->writeData(0x33);
    _bus->writeData(0x33);
    
    _bus->writeCommand(0xB7); // Gate control
    _bus->writeData(0x35);
    
    _bus->writeCommand(0xBB); // VCOM setting
    _bus->writeData(0x19);
    
    _bus->writeCommand(0xC0); // LCM control
    _bus->writeData(0x2C);
    
    _bus->writeCommand(0xC2); // VDV and VRH command enable
    _bus->writeData(0x01);
    
    _bus->writeCommand(0xC3); // VRH voltage
    _bus->writeData(0x19);
    
    _bus->writeCommand(0xC4); // VDV voltage
    _bus->writeData(0x20);
    
    _bus->writeCommand(0xC6); // Frame rate control
    _bus->writeData(0x0F);
    
    _bus->writeCommand(0xD0); // Power control
    _bus->writeData(0xA4);
    _bus->writeData(0xA1);
    
    _bus->writeCommand(0xE0); // Positive voltage gamma
    _bus->writeData(0xD0);
    _bus->writeData(0x04);
    _bus->writeData(0x0D);
    _bus->writeData(0x11);
    _bus->writeData(0x13);
    _bus->writeData(0x2B);
    _bus->writeData(0x3C);
    _bus->writeData(0x44);
    _bus->writeData(0x51);
    _bus->writeData(0x2F);
    _bus->writeData(0x1F);
    _bus->writeData(0x1F);
    _bus->writeData(0x20);
    _bus->writeData(0x23);
    
    _bus->writeCommand(0xE1); // Negative voltage gamma
    _bus->writeData(0xD0);
    _bus->writeData(0x04);
    _bus->writeData(0x0D);
    _bus->writeData(0x11);
    _bus->writeData(0x13);
    _bus->writeData(0x2B);
    _bus->writeData(0x3C);
    _bus->writeData(0x44);
    _bus->writeData(0x51);
    _bus->writeData(0x2F);
    _bus->writeData(0x1F);
    _bus->writeData(0x1F);
    _bus->writeData(0x20);
    _bus->writeData(0x23);
    
    _bus->writeCommand(0x21); // Display inversion on
    _bus->writeCommand(0x11); // Sleep out
    delay(120);
    _bus->writeCommand(0x29); // Display on
  }
  
  void drawPixel(int16_t x, int16_t y, uint16_t color) override {
    // Set address window
    _bus->writeCommand(0x2A);
    _bus->writeData((x >> 8) & 0xFF);
    _bus->writeData(x & 0xFF);
    _bus->writeData(((x + 1) >> 8) & 0xFF);
    _bus->writeData((x + 1) & 0xFF);
    
    _bus->writeCommand(0x2B);
    _bus->writeData((y >> 8) & 0xFF);
    _bus->writeData(y & 0xFF);
    _bus->writeData(((y + 1) >> 8) & 0xFF);
    _bus->writeData((y + 1) & 0xFF);
    
    // Write pixel data
    _bus->writeCommand(0x2C);
    _bus->writeData((color >> 8) & 0xFF);
    _bus->writeData(color & 0xFF);
  }
  
  void fillScreen(uint16_t color) override {
    // Fill entire screen with color
    fillRect(0, 0, _width, _height, color);
  }
  
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override {
    // Fill rectangle with color
    for (int16_t row = y; row < y + h; row++) {
      for (int16_t col = x; col < x + w; col++) {
        drawPixel(col, row, color);
      }
    }
  }
  
  void setCursor(int16_t x, int16_t y) override {
    _cursor_x = x;
    _cursor_y = y;
  }
  
  void setTextColor(uint16_t color) override {
    _text_color = color;
  }
  
  void setTextSize(uint8_t size) override {
    _text_size = size;
  }
  
  void print(const char *text) override {
    while (*text) {
      drawChar(_cursor_x, _cursor_y, *text, _text_color, 0, _text_size);
      _cursor_x += 6 * _text_size;
      if (_cursor_x >= _width) {
        _cursor_x = 0;
        _cursor_y += 8 * _text_size;
      }
      text++;
    }
  }
  
  void println(const char *text) override {
    print(text);
    print("\n");
  }

private:
  Arduino_DataBus *_bus;
  int8_t _rst;
  uint8_t _rotation;
  bool _ips;
  int16_t _cursor_x = 0;
  int16_t _cursor_y = 0;
  uint16_t _text_color = 0xFFFF;
  uint8_t _text_size = 1;
  
  void drawChar(int16_t x, int16_t y, char c, uint16_t color, uint16_t bg, uint8_t size) {
    // Simple character drawing implementation
    const uint8_t font[] = {
      // 8x8 font data would go here
    };
    
    uint8_t col = (c - 32) * 8;
    for (int8_t i = 0; i < 8; i++) {
      uint8_t line = font[col + i];
      for (int8_t j = 0; j < 8; j++) {
        if (line & (1 << j)) {
          drawPixel(x + j * size, y + i * size, color);
        } else {
          drawPixel(x + j * size, y + i * size, bg);
        }
      }
    }
  }
};

#endif // ARDUINO_GFX_H