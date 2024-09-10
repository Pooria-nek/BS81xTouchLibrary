#include "BS81xTouchLibrary.h"

// Constructor for parallel interface models
BS81xTouchLibrary::BS81xTouchLibrary(uint8_t *touchKeyPins, uint8_t numKeys, uint8_t outputPin) {
  _touchKeyPins = touchKeyPins;
  _numKeys = numKeys;
  _outputPin = outputPin;
}

// Constructor for I2C models
BS81xTouchLibrary::BS81xTouchLibrary(uint8_t i2cAddress) {
  _i2cAddress = i2cAddress;
}

// Constructor for serial interface models
BS81xTouchLibrary::BS81xTouchLibrary(uint8_t dataPin, uint8_t clockPin) {
  _dataPin = dataPin;
  _clockPin = clockPin;
}

// Initialize the library
void BS81xTouchLibrary::begin() {
  if (_i2cAddress) {
    Wire.begin(); // Initialize I2C communication
  } else if (_dataPin && _clockPin) {
    pinMode(_dataPin, INPUT);
    pinMode(_clockPin, OUTPUT);
  } else {
    // Initialize touch key pins for parallel models
    for (uint8_t i = 0; i < _numKeys; i++) {
      pinMode(_touchKeyPins[i], INPUT);
    }
    pinMode(_outputPin, OUTPUT);
  }
}

// Check if a specific key is touched (parallel or I2C modes)
bool BS81xTouchLibrary::isTouched(uint8_t keyIndex) {
  if (_i2cAddress) {
    // For I2C models, read the key status register
    uint8_t status = readI2CRegister(0x08);  // Assuming key status starts at 0x08
    return status & (1 << keyIndex);
  } else {
    return digitalRead(_touchKeyPins[keyIndex]) == LOW;  // Parallel mode
  }
}

// Get all key states in a single read (serial or I2C modes)
uint16_t BS81xTouchLibrary::getAllKeyStates() {
  if (_i2cAddress) {
    uint8_t keyStatusLow = readI2CRegister(0x08);  // Read lower byte
    uint8_t keyStatusHigh = readI2CRegister(0x09); // Read higher byte for BS8116A-3
    return (keyStatusHigh << 8) | keyStatusLow;
  } else if (_dataPin && _clockPin) {
    // For serial interface models, read the data stream
    return readSerialData();
  }
  return 0;
}

// Set sensitivity (optional based on external capacitor or I2C)
void BS81xTouchLibrary::setSensitivity(uint8_t keyIndex, uint8_t sensitivityValue) {
  if (_i2cAddress) {
    writeI2CRegister(0xB5 + keyIndex, sensitivityValue);  // Adjust sensitivity via I2C
  }
}

// Enable or disable wake-up function
void BS81xTouchLibrary::setWakeupFunction(bool enable) {
  if (_i2cAddress) {
    uint8_t reg = readI2CRegister(0xB0);  // Read the option register
    if (enable) {
      reg &= ~(1 << 7);  // Enable wake-up
    } else {
      reg |= (1 << 7);   // Disable wake-up
    }
    writeI2CRegister(0xB0, reg);
  }
}

// Enable auto-calibration function
void BS81xTouchLibrary::enableAutoCalibration(bool enable) {
  // Implement auto-calibration based on user needs (default is ON)
  if (enable) {
    // Configure auto-calibration settings
  }
}

// Set maximum key hold duration time
void BS81xTouchLibrary::setMaxKeyOnDuration(uint16_t duration) {
  // Typically fixed at 64s, but you can implement a custom handler here
}

// IRQ handler (for I2C and serial models with interrupt support)
void BS81xTouchLibrary::attachInterruptHandler(void (*isr)()) {
  // Attach an interrupt handler for models supporting IRQ
  attachInterrupt(digitalPinToInterrupt(_outputPin), isr, FALLING);
}

// Helper function for serial communication
uint8_t BS81xTouchLibrary::readSerialData() {
  uint8_t result = 0;
  for (int i = 0; i < 8; i++) {
    digitalWrite(_clockPin, LOW);
    delayMicroseconds(20);
    result |= digitalRead(_dataPin) << i;
    digitalWrite(_clockPin, HIGH);
    delayMicroseconds(20);
  }
  return result;
}

// Helper function for I2C communication
void BS81xTouchLibrary::writeI2CRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(_i2cAddress);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t BS81xTouchLibrary::readI2CRegister(uint8_t reg) {
  Wire.beginTransmission(_i2cAddress);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(_i2cAddress, (uint8_t)1);
  return Wire.read();
}
