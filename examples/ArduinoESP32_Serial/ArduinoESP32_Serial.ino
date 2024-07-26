#include <Arduino.h>

#include <opengloves.hpp>
#include <opengloves/alpha.hpp>

#define FINGER_THUMB_PIN 32
#define FINGER_INDEX_PIN 35
#define FINGER_MIDDLE_PIN 34
#define FINGER_RING_PIN 39
#define FINGER_PINKY_PIN 36

#define ANALOG_MAX 4095.0f

using namespace opengloves;

void setup() {
  Serial.begin(9600);
}

InputPeripheralData input;
std::string buffer(256, '\0');

void loop() {
  input.curl.thumb.curl_total = analogRead(FINGER_THUMB_PIN) / ANALOG_MAX;
  input.curl.index.curl_total = analogRead(FINGER_INDEX_PIN) / ANALOG_MAX;
  input.curl.middle.curl_total = analogRead(FINGER_MIDDLE_PIN) / ANALOG_MAX;
  input.curl.ring.curl_total = analogRead(FINGER_RING_PIN) / ANALOG_MAX;
  input.curl.pinky.curl_total = analogRead(FINGER_PINKY_PIN) / ANALOG_MAX;

  auto const length = AlphaEncoding::encodeInput(input, reinterpret_cast<uint8_t *>(buffer.data()), buffer.size());

  Serial.write(buffer.data(), length);
}