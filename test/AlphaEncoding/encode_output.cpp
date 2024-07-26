#include <catch2/catch_all.hpp>

#include <opengloves.hpp>
#include <opengloves/alpha.hpp>

using namespace opengloves;

void check(const OutputData &output, const std::string &expected) {
  std::string encoded(256, '\0');

  AlphaEncoding::encodeOutput(output, reinterpret_cast<uint8_t *>(encoded.data()),
                             encoded.size());

  REQUIRE(encoded.c_str() == expected);
}


TEST_CASE("AlphaEncoding::encodeOutput", "[alpha]") {
  check(OutputData(), "");

  SECTION("OutputForceFeedbackData") {
    check(OutputForceFeedbackData(), "A0B0C0D0E0\n");

    check(OutputForceFeedbackData {
        .thumb = 0.2f,
        .index = 0.4f,
        .middle = 0.6f,
        .ring = 0.8f,
        .pinky = 1.0f,
    }, "A819B1638C2457D3276E4095\n");
  }
}