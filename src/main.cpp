#include <Arduino.h>
#include <Transmitter.h>
#include <BluetoothA2DPSink.h>
#include <Button.h>

#define SI4713_RESET_PIN 19
#define SI4713_POWER 95

#define I2S_NUM I2S_NUM_0

#define BUTTON_PIN 14
#define BUTTON_MIN_HOLD_DURATION 1000
#define BUTTON_MAX_TIME_BETWEEN_PRESSES 250
#define BUTTON_DEBOUNCE_DELAY 25

Transmitter transmitter(SI4713_RESET_PIN, SI4713_POWER, 10530);
BluetoothA2DPSink a2dpSink;

Button button(
    BUTTON_PIN,
    BUTTON_MIN_HOLD_DURATION,
    BUTTON_MAX_TIME_BETWEEN_PRESSES,
    BUTTON_DEBOUNCE_DELAY);

void setup_i2s()
{
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = 44100,                         // corrected by info from bluetooth
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, /* the A2DP 16 bits will be expanded to 32 bits */
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = 0, // default interrupt priority
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = false,
      .tx_desc_auto_clear = true // avoiding noise in case of data unavailability
  };

  // Configure I2S pin assignments
  i2s_pin_config_t pin_config = {
      .bck_io_num = 18,                // Bit clock pin
      .ws_io_num = 16,                 // Word select pin
      .data_out_num = 17,              // Data output pin
      .data_in_num = I2S_PIN_NO_CHANGE // No input data pin
  };

  // Install I2S driver and configure the pins
  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, &pin_config);
}

void onStreamRead(const uint8_t *data, uint32_t length)
{
  size_t bytes_written;
  // Write received audio data to I2S for output
  i2s_write(I2S_NUM, data, length, &bytes_written, portMAX_DELAY);
}

void onButtonPress(uint8_t pressCount)
{
  Serial.printf("Button pressed %u times!\n", pressCount);
}

void onButtonHold()
{
  Serial.println("Button hold!");
  transmitter.autoTune();
}

void setup()
{
  delay(3000);
  Serial.begin(9600);

  transmitter.begin();
  Serial.println(transmitter.getFrequency());

  setup_i2s();

  a2dpSink.set_stream_reader(onStreamRead, false);
  a2dpSink.start("ESP32", true);

  button.onPress(onButtonPress);
  button.onHold(onButtonHold);
}

void loop()
{
  button.update();

  delay(10);
}