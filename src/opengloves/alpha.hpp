#pragma once

#include <opengloves.hpp>

namespace opengloves {
  class AlphaEncoding {
    inline static constexpr const uint16_t MAX_ANALOG_VALUE = 4095;

    inline static constexpr const char* INFO_FIRMWARE_VERSION_KEY = "(ZV)";
    inline static constexpr const char* INFO_DEVICE_TYPE_KEY = "(ZG)";
    inline static constexpr const char* INFO_HAND_KEY = "(ZH)";

    public:
      static auto encodeInput(const InputData& input, uint8_t* buffer, size_t buffer_size) -> size_t;
      static auto encodeInputInfo(const InputInfoData& input, uint8_t* buffer, size_t buffer_size) -> size_t;
      static auto encodeInputPeripheral(const InputPeripheralData& input, uint8_t* buffer, size_t buffer_size) -> size_t;
  };

  inline auto AlphaEncoding::encodeInput(const opengloves::InputData &input, uint8_t *buffer, size_t buffer_size) -> size_t {
    if (std::holds_alternative<InputPeripheralData>(input)) {
      return AlphaEncoding::encodeInputPeripheral(std::get<InputPeripheralData>(input), buffer, buffer_size);
    } else if (std::holds_alternative<InputInfoData>(input)) {
      return AlphaEncoding::encodeInputInfo(std::get<InputInfoData>(input), buffer, buffer_size);
    }

    return 0;
  }

  inline auto AlphaEncoding::encodeInputInfo(const opengloves::InputInfoData &input, uint8_t *buffer, size_t buffer_size) -> size_t {
    const auto& keyFirmwareVersion = AlphaEncoding::INFO_FIRMWARE_VERSION_KEY;
    const auto& keyDeviceType = AlphaEncoding::INFO_DEVICE_TYPE_KEY;
    const auto& keyHand = AlphaEncoding::INFO_HAND_KEY;

    return snprintf(
        reinterpret_cast<char *const>(buffer),
        buffer_size,
        "%s%u%s%u%s%u\n",
        keyFirmwareVersion,
        input.firmware_version,
        keyDeviceType,
        input.device_type,
        keyHand,
        input.hand
    );
  }

  inline auto AlphaEncoding::encodeInputPeripheral(const opengloves::InputPeripheralData &input, uint8_t *buffer, size_t buffer_size) -> size_t {
    auto written = 0;

    const auto& curls = input.curl.fingers;
    const auto& splays = input.splay.fingers;

    for (auto i = 0; i < curls.size(); i++) {
      const auto &finger_curl = curls[i];
      const auto &finger_splay = splays[i];
      const auto finger_alpha_key = 'A' + i;

      written += snprintf(
        reinterpret_cast<char *const>(buffer + written),
        buffer_size - written,
        "%c%u",
        finger_alpha_key,
        static_cast<int>(finger_curl.curl_total * MAX_ANALOG_VALUE)
      );

      if (finger_splay > 0.0F) {
        written += snprintf(
            reinterpret_cast<char *const>(buffer + written),
            buffer_size - written,
            "(%cB)%u",
            finger_alpha_key,
            static_cast<int>(finger_splay * MAX_ANALOG_VALUE)
        );
      }
    }

    written += snprintf(reinterpret_cast<char *const>(buffer + written), buffer_size - written, "\n");

    return written;
  }
} // namespace opengloves
