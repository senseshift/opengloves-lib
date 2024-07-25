#include <catch2/catch_all.hpp>

#include <opengloves.hpp>
#include <opengloves/alpha.hpp>

using namespace opengloves;

void check(const InputData &input, const std::string &expected) {
  std::string encoded(256, '\0');

  AlphaEncoding::encodeInput(input, reinterpret_cast<uint8_t *>(encoded.data()),
                             encoded.size());

  REQUIRE(encoded.c_str() == expected);
}

TEST_CASE("AlphaEncoding::encodeInput", "[alpha]") {
  SECTION("InputPeripheralData") {
    check(InputPeripheralData(), "A0B0C0D0E0\n");

    SECTION("Curl only") {
      InputPeripheralData input;

      input.curl = {
          .thumb = {.curl_total = 0},
          .index = {.curl_total = 0},
          .middle = {.curl_total = 0},
          .ring = {.curl_total = 0},
          .pinky = {.curl_total = 0},
      };
      check(input, "A0B0C0D0E0\n");

      input.curl = {
          .thumb = {.curl_total = 0.5},
          .index = {.curl_total = 0.5},
          .middle = {.curl_total = 0.5},
          .ring = {.curl_total = 0.5},
          .pinky = {.curl_total = 0.5},
      };
      check(input, "A2047B2047C2047D2047E2047\n");

      input.curl = {
          .thumb = {.curl_total = 1.0},
          .index = {.curl_total = 1.0},
          .middle = {.curl_total = 1.0},
          .ring = {.curl_total = 1.0},
          .pinky = {.curl_total = 1.0},
      };
      check(input, "A4095B4095C4095D4095E4095\n");
    }

    SECTION("With Splay") {
      InputPeripheralData input;

      input.splay = {
          .thumb = 0.5,
          .index = 0.5,
          .middle = 0.5,
          .ring = 0.5,
          .pinky = 0.5,
      };
      check(input, "A0(AB)2047B0(BB)2047C0(CB)2047D0(DB)2047E0(EB)2047\n");
    }

    SECTION("Joints") {
      InputPeripheralData input;

      input.curl = {
        .thumb = { .curl = { 0.25f, 0.5f, 0.75f, 1.0f } },
      };
      check(input, "A1023(AAB)2047(AAC)3071(AAD)4095B0C0D0E0\n");

      input.curl = {
        .index = { .curl = { 0.25f, 0.5f, 0.75f, 1.0f } },
      };
      check(input, "A0B1023(BAB)2047(BAC)3071(BAD)4095C0D0E0\n");

      input.curl = {
        .thumb = { .curl = { 0.25f, 0.5f, 0.75f, 1.0f } },
        .index = { .curl = { 0.25f, 0.5f, 0.75f, 1.0f } },
        .middle = { .curl = { 0.25f, 0.5f, 0.75f, 1.0f } },
        .ring = { .curl = { 0.25f, 0.5f, 0.75f, 1.0f } },
        .pinky = { .curl = { 0.25f, 0.5f, 0.75f, 1.0f } },
      };
      check(input, "A1023(AAB)2047(AAC)3071(AAD)4095B1023(BAB)2047(BAC)3071(BAD)4095C1023(CAB)2047(CAC)3071(CAD)4095D1023(DAB)2047(DAC)3071(DAD)4095E1023(EAB)2047(EAC)3071(EAD)4095\n");
    }

    SECTION("Buttons") {
      InputPeripheralData input;

      input.button_a = { .press = true };
      input.button_calibrate = { .press = true };

      check(input, "A0B0C0D0E0JO\n");

      input.grab = { true };
      input.pinch = { true };

      check(input, "A0B0C0D0E0JOML\n");

      input.button_a = { .press = false };
      input.button_calibrate = { .press = false };

      check(input, "A0B0C0D0E0ML\n");

      input.grab = { false };
      input.pinch = { false };

      check(input, "A0B0C0D0E0\n");
    }
  }

  SECTION("InputInfoData") {
    check(
        InputInfoData{
            .hand = Hand_Left,
            .device_type = DeviceType_LucidGloves,
            .firmware_version = 42,
        },
        "(ZV)42(ZG)0(ZH)0\n");
  }
}
