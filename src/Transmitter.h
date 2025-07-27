#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <Arduino.h>
#include <Adafruit_Si4713.h>

class Transmitter
{
public:
  Transmitter(uint8_t resetPin, uint8_t power);
  bool begin();
  uint16_t getFrequency();
  void setFrequency(uint16_t frequency);
  void autoTune();

private:
  uint8_t _resetPin;
  Adafruit_Si4713 _si4713;
  uint8_t _power;
  uint16_t _frequency;

  void reset();
  void setProperties();
  uint16_t findBestFrequency();
};

#endif