#include <Transmitter.h>

Transmitter::Transmitter(uint8_t resetPin, uint8_t power)
    : _resetPin(resetPin),
      _si4713(resetPin),
      _power(constrain(power, 88, 115))
{
}

bool Transmitter::begin()
{
  reset();

  if (!_si4713.begin())
    return false;

  setProperties();

  _si4713.setTXpower(_power);

  return true;
}

void Transmitter::autoTune()
{
  uint16_t bestFrequency = findBestFrequency();
  setFrequency(bestFrequency);
}

uint16_t Transmitter::getFrequency()
{
  return _frequency;
}

void Transmitter::setFrequency(uint16_t frequency)
{
  _si4713.tuneFM(frequency);
  _frequency = frequency;
}

// Although a similar function is allready implemented in the Adafruit library,
// this manual reset pulse is necessary for the Si4713 to initialize correctly.
// Without this step, the chip may fail to start properly.
void Transmitter::reset()
{
  pinMode(_resetPin, OUTPUT);
  digitalWrite(_resetPin, HIGH);
  delay(100);

  digitalWrite(_resetPin, LOW);
  delay(100);

  digitalWrite(_resetPin, HIGH);
  delay(100);
}

void Transmitter::setProperties()
{
  // Activate limiter, disable audio dynamic range control
  _si4713.setProperty(SI4713_PROP_TX_ACOMP_ENABLE, 0x02);
  // Set pre-emphasis for europe
  _si4713.setProperty(SI4713_PROP_TX_PREEMPHASIS, 0x01);
  // Set line level input according to the datasheet
  _si4713.setProperty(0x210, 0x31E7);
}

uint16_t Transmitter::findBestFrequency()
{
  const uint16_t startFrequency = 8800;
  const uint16_t endFrequency = 10800;

  uint16_t bestFrequency = startFrequency;
  uint8_t lowestNoise = UINT8_MAX;

  for (uint16_t frequency = startFrequency; frequency < endFrequency; frequency += 10)
  {
    _si4713.readTuneMeasure(frequency);
    _si4713.readTuneStatus();

    if (_si4713.currNoiseLevel <= lowestNoise)
    {
      bestFrequency = frequency;
      lowestNoise = _si4713.currNoiseLevel;
    }
  }

  return bestFrequency;
}