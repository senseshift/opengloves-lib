#include <catch2/catch_all.hpp>

#include <opengloves.hpp>
#include <opengloves/alpha.hpp>

using namespace opengloves;

TEST_CASE("Benchmark AlphaEncoding", "[benchmark][alpha]") {
  SECTION("encodeInput") {
    BENCHMARK_ADVANCED("encode default")(Catch::Benchmark::Chronometer meter) {
      std::string buffer(256, '\0');
      InputPeripheralData input;

      meter.measure([&buffer, &input] {
        return AlphaEncoding::encodeInput(input, reinterpret_cast<uint8_t *>(buffer.data()), buffer.length());
      });
    };

    BENCHMARK_ADVANCED("encode full")(Catch::Benchmark::Chronometer meter) {
      std::string buffer(256, '\0');
      InputPeripheralData input;

      input.curl = {
          .thumb = { .curl = { 0.25f, 0.5f, 0.75f, 1.0f } },
          .index = { .curl = { 0.25f, 0.5f, 0.75f, 1.0f } },
          .middle = { .curl = { 0.25f, 0.5f, 0.75f, 1.0f } },
          .ring = { .curl = { 0.25f, 0.5f, 0.75f, 1.0f } },
          .pinky = { .curl = { 0.25f, 0.5f, 0.75f, 1.0f } },
      };
      input.splay = {
          .thumb = 0.5,
          .index = 0.5,
          .middle = 0.5,
          .ring = 0.5,
          .pinky = 0.5,
      };
      input.button_a.press = true;
      input.button_b.press = true;
      input.button_menu.press = true;
      input.button_calibrate.press = true;
      input.trigger.press = true;
      input.grab.press = true;
      input.pinch.press = true;

      input.joystick = {
          .x = 0.5,
          .y = 0.5,
          .press = true,
      };

      meter.measure([&buffer, &input] {
        return AlphaEncoding::encodeInput(input, reinterpret_cast<uint8_t *>(buffer.data()), buffer.length());
      });
    };
  }

  SECTION("encodeOutput") {
    BENCHMARK_ADVANCED("encode default")(Catch::Benchmark::Chronometer meter) {
      std::string buffer(256, '\0');
      OutputForceFeedbackData output;

      meter.measure([&buffer, &output] {
        return AlphaEncoding::encodeOutput(output, reinterpret_cast<uint8_t *>(buffer.data()), buffer.length());
      });
    };
  }

  SECTION("decodeOutput") {
    BENCHMARK_ADVANCED("decode default")(Catch::Benchmark::Chronometer meter) {
      std::string data = "A0B0C0D0E0\n";

      meter.measure([&data] {
        return AlphaEncoding::decodeOutput(reinterpret_cast<uint8_t *>(data.data()), data.length());
      });
    };
  }
}
