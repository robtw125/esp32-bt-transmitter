#include <Button.h>

Button::Button(uint8_t pin, unsigned long minHoldDuration, unsigned long maxTimeBetweenPresses, unsigned long debounceDelay)
    : _pin(pin),
      _minHoldDuration(minHoldDuration),
      _maxTimeBetweenPresses(maxTimeBetweenPresses),
      _debounceDelay(debounceDelay),
      _lastReading(HIGH),
      _lastState(HIGH),
      _lastDebounceTime(millis()),
      _pressStartTime(millis()),
      _lastPressEndTime(millis()),
      _pressCount(0),
      _onPressCallback(nullptr),
      _onHoldCallback(nullptr),
      _holdTriggered(false)
{
  pinMode(pin, INPUT_PULLUP);
}

void Button::update()
{
  evaluatePresses();
  uint8_t reading = digitalRead(_pin);

  if (reading != _lastReading)
  {
    _lastDebounceTime = millis();
    _lastReading = reading;
  }

  if ((millis() - _lastDebounceTime >= _debounceDelay) && reading != _lastState)
  {
    if (reading == LOW)
    {
      _pressStartTime = millis();
      _pressCount++;
      _holdTriggered = false;
    }
    else
    {
      _lastPressEndTime = millis();
    }

    _lastState = reading;
  }
}

void Button::onPress(std::function<void(uint8_t)> callback)
{
  _onPressCallback = callback;
}

void Button::onHold(std::function<void()> callback)
{
  _onHoldCallback = callback;
}

void Button::evaluatePresses()
{
  if (_lastState == LOW)
  {
    unsigned long holdDuration = millis() - _pressStartTime;

    if (holdDuration >= _minHoldDuration && !_holdTriggered)
    {
      if (_onHoldCallback)
        _onHoldCallback();

      _holdTriggered = true;
      reset();
    }
  }
  else
  {
    unsigned long timeSincePressed = millis() - _lastPressEndTime;

    if (timeSincePressed > _maxTimeBetweenPresses && _pressCount > 0)
    {
      if (_onPressCallback)
        _onPressCallback(_pressCount);

      reset();
    }
  }
}

void Button::reset()
{
  _pressCount = 0;
}