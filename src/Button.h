#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <functional>

class Button
{
public:
  Button(uint8_t pin, unsigned long minHoldDuration, unsigned long maxTimeBetweenPresses, unsigned long debounceDelay = 25);

  void update();
  void onPress(std::function<void(uint8_t)> callback);
  void onHold(std::function<void()> callback);

private:
  uint8_t _pin;
  unsigned long _minHoldDuration;
  unsigned long _maxTimeBetweenPresses;
  unsigned long _debounceDelay;
  uint8_t _lastReading;
  uint8_t _lastState;
  unsigned long _lastDebounceTime;
  unsigned long _pressStartTime;
  unsigned long _lastPressEndTime;
  uint8_t _pressCount;
  std::function<void(uint8_t)> _onPressCallback;
  std::function<void()> _onHoldCallback;
  bool _holdTriggered;

  bool isPressed();
  void evaluatePresses();
  void reset();
};

#endif