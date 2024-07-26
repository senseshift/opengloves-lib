#include <catch2/catch_all.hpp>

#include <opengloves.hpp>
#include <opengloves/alpha.hpp>

using namespace opengloves;

TEST_CASE("AlphaEncoding::encodeInput", "[alpha]") {
  BENCHMARK_ADVANCED("encode default")(Catch::Benchmark::Chronometer meter) {
    std::string buffer(256, '\0');
    InputPeripheralData input;

    meter.measure([&buffer, &input] {
      return AlphaEncoding::encodeInput(input, reinterpret_cast<uint8_t *>(buffer.data()), buffer.length());
    });
  };
}

TEST_CASE("AlphaEncoding::encodeOutput", "[alpha]") {
  BENCHMARK_ADVANCED("encode default")(Catch::Benchmark::Chronometer meter) {
    std::string buffer(256, '\0');
    OutputForceFeedbackData output;

    meter.measure([&buffer, &output] {
      return AlphaEncoding::encodeOutput(output, reinterpret_cast<uint8_t *>(buffer.data()), buffer.length());
    });
  };
}

TEST_CASE("AlphaEncoding::decodeOutput", "[alpha]") {
  BENCHMARK_ADVANCED("decode default")(Catch::Benchmark::Chronometer meter) {
    std::string data = "A0B0C0D0E0\n";

    meter.measure([&data] {
      return AlphaEncoding::decodeOutput(reinterpret_cast<uint8_t *>(data.data()), data.length());
    });
  };
}