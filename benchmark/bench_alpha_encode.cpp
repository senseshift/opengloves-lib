#include <catch2/catch_all.hpp>

#include <opengloves.hpp>
#include <opengloves/alpha.hpp>

using namespace opengloves;

TEST_CASE("benchmark/encoding/alpha/input", "[benchmark]") {
  BENCHMARK_ADVANCED("encode default")(Catch::Benchmark::Chronometer meter) {
    std::string buffer(256, '\0');
    InputPeripheralData input;

    meter.measure([&buffer, &input] {
      return AlphaEncoding::encodeInput(input, reinterpret_cast<uint8_t *>(buffer.data()), buffer.length());
    });
  };
}