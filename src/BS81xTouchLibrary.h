#ifndef BS81XTOUCHLIBRARY_H
#define BS81XTOUCHLIBRARY_H

#include "Arduino.h"
#include <Wire.h> // Required for I2C communication

class BS81xTouchLibrary {
  public:
    // Constructor for parallel interface models
    BS81xTouchLibrary(uint8_t *touchKeyPins, uint8_t numKeys, uint8_t outputPin);

    // Constructor for I2C models
    BS81xTouchLibrary(uint8_t i2cAddress);

    // Constructor for serial interface models
    BS81xTouchLibrary(uint8_t dataPin, uint8_t clockPin);

    // Initialize the library
    void begin();

    // Check if a specific key is touched (parallel or I2C modes)
    bool isTouched(uint8_t keyIndex);

    // Get all key states in a single read (serial or I2C modes)
    uint16_t getAllKeyStates();

    // Set sensitivity (optional based on external capacitor or I2C)
    void setSensitivity(uint8_t keyIndex, uint8_t sensitivityValue);

    // Enable or disable wake-up function
    void setWakeupFunction(bool enable);

    // Enable auto-calibration function
    void enableAutoCalibration(bool enable);

    // Set maximum key hold duration time
    void setMaxKeyOnDuration(uint16_t duration);

    // IRQ handler (for I2C and serial models with interrupt support)
    void attachInterruptHandler(void (*isr)());

  private:
    uint8_t _numKeys;
    uint8_t *_touchKeyPins;
    uint8_t _outputPin;

    uint8_t _i2cAddress;
    uint8_t _dataPin;
    uint8_t _clockPin;

    // Helper function for serial communication
    uint8_t readSerialData();

    // Helper function for I2C communication
    void writeI2CRegister(uint8_t reg, uint8_t value);
    uint8_t readI2CRegister(uint8_t reg);
};

#endif
