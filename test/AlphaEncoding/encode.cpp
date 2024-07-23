#include <catch.hpp>

#include <opengloves.hpp>
#include <opengloves/alpha.hpp>

using namespace opengloves;

void check(const InputData &input, const std::string &expected) {
  AlphaEncoding encoder;

  std::string encoded(256, '\0');
  encoder.encodeInput(input, reinterpret_cast<uint8_t *>(encoded.data()),
                      encoded.size());

  REQUIRE(encoded == expected);
}

TEST_CASE("AlphaEncoding::encodeInput()") {
  SECTION("Empty input") { check(InputData(), ""); }
}