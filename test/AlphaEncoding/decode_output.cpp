#include <catch2/catch_all.hpp>

#include <opengloves.hpp>
#include <opengloves/alpha.hpp>

using namespace opengloves;

template <typename T>
void check(const std::string &data, const T &expected) {
  auto actual = AlphaEncoding::decodeOutput(reinterpret_cast<const uint8_t *>(data.c_str()), data.size());

  REQUIRE(std::holds_alternative<T>(actual));
  REQUIRE(std::get<T>(actual) == expected);
}

TEST_CASE("AlphaEncoding::decodeOutput", "[alpha]") {
  check("", OutputInvalid());
  check("\n", OutputInvalid());

  SECTION("OutputForceFeedbackData") {
    check("A0B0C0D0E0\n", OutputForceFeedbackData{
        .thumb = 0.0f,
        .index = 0.0f,
        .middle = 0.0f,
        .ring = 0.0f,
        .pinky = 0.0f,
    });

    check("A0\n", OutputForceFeedbackData{
        .thumb = 0.0f,
        .index = 0.0f,
        .middle = 0.0f,
        .ring = 0.0f,
        .pinky = 0.0f,
    });

    check("A819B1638C2457D3276E4095\n", OutputForceFeedbackData{
        .thumb = 0.2f,
        .index = 0.4f,
        .middle = 0.6f,
        .ring = 0.8f,
        .pinky = 1.0f,
    });

    check("A4095B4095C4095D4095E4095\n", OutputForceFeedbackData{
        .thumb = 1.0f,
        .index = 1.0f,
        .middle = 1.0f,
        .ring = 1.0f,
        .pinky = 1.0f,
    });
  }
}