#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <Arduino.h>
#include <Adafruit_Si4713.h>
#include <Preferences.h>

#define NAMESPACE "transmitter"
#define KEY "fmFrequency"

class Transmitter
{
public:
  Transmitter(uint8_t resetPin, uint8_t power, uint16_t defaultFrequency = 10530);
  bool begin();
  const uint16_t getFrequency();
  void setFrequency(uint16_t frequency);
  void autoTune();

private:
  uint8_t _resetPin;
  Adafruit_Si4713 _si4713;
  uint8_t _power;
  uint16_t _frequency;
  Preferences _preferences;
  uint16_t _defaultFrequency;

  void reset();
  void setProperties();
  uint16_t findBestFrequency();
  void saveFrequency();
  uint16_t loadFrequency();
};

#endif