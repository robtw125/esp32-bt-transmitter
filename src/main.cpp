#include <Arduino.h>
#include <BluetoothA2DPSink.h>
#include <driver/i2s.h>
#include "Transmitter.h"

#define I2S_NUM I2S_NUM_0

#define I2S_BCK 26
#define I2S_LRCK 25
#define I2S_DIN 22

BluetoothA2DPSink a2dp_sink;

void readDataStream(const uint8_t *data, uint32_t length)
{
  size_t bytes_written;
  i2s_write(I2S_NUM, data, length, &bytes_written, portMAX_DELAY);
}

void setup()
{
  Transmitter transmitter(19, 88);
  transmitter.begin();
  transmitter.setFrequency(11530);

  Serial.begin(115200);

  // I2S-Konfiguration
  static const i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = 44100,                         // corrected by info from bluetooth
      .bits_per_sample = (i2s_bits_per_sample_t)32, /* the A2DP 16 bits will be expanded to 32 bits */
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = 0, // default interrupt priority
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = false,
      .tx_desc_auto_clear = true // avoiding noise in case of data unavailability
  };

  i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_BCK,
      .ws_io_num = I2S_LRCK,
      .data_out_num = I2S_DIN,
      .data_in_num = -1};

  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, &pin_config);

  a2dp_sink.set_i2s_config(i2s_config);
  a2dp_sink.set_pin_config(pin_config);

  // Bluetooth A2DP starten
  a2dp_sink.set_stream_reader(readDataStream, false);
  a2dp_sink.start("MyMusic");
}

void loop()
{
  delay(10);
}
