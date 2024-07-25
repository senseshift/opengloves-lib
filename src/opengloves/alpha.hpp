#pragma once

#include <opengloves.hpp>

namespace opengloves {
  class AlphaEncoding {
    inline static constexpr const uint16_t MAX_ANALOG_VALUE = 4095;

    /// Alpha keys for fingers.
    /// <b>MUST</b> be in the same order as the `InputFingerData` struct.
    inline static constexpr const std::array<unsigned char, 5> FINGER_ALPHA_KEY = { {
        'A', // Thumb
        'B', // Index
        'C', // Middle
        'D', // Ring
        'E', // Pinky
    } };

    /// Alpha keys for buttons.
    /// <b>MUST</b> be in the same order as the `InputPeripheralData` struct.
    inline static constexpr const std::array<unsigned char, 5> BUTTON_ALPHA_KEY = { {
        'J', // Button A
        'K', // Button B
        'N', // Button Menu
        'O', // Button Calibrate
        'M', // Button Pinch
    } };

    /// Alpha keys for analog buttons.
    /// <b>MUST</b> be in the same order as the `InputPeripheralData` struct.
    inline static constexpr const std::array<unsigned char, 2> ANALOG_BUTTON_ALPHA_KEY = { {
        'I', // Trigger
        'L', // Grab
    } };

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

      const auto& joints = finger_curl.curl;
      for (auto j = 1; j < joints.size(); j++) {
        const auto& joint = joints[j];
        const auto joint_alpha_key = 'A' + j;

        if (joint == 0.0F) {
          continue;
        }

        written += snprintf(
            reinterpret_cast<char *const>(buffer + written),
            buffer_size - written,
            "(%cA%c)%u",
            finger_alpha_key,
            joint_alpha_key,
            static_cast<int>(joint * MAX_ANALOG_VALUE)
        );
      }
    }

    if (input.joystick.x != 0.0F) {
      written += snprintf(
          reinterpret_cast<char *const>(buffer + written),
          buffer_size - written,
          "F%u",
          static_cast<int>(input.joystick.x * MAX_ANALOG_VALUE)
        );
    }
    if (input.joystick.y != 0.0F) {
      written += snprintf(
          reinterpret_cast<char *const>(buffer + written),
          buffer_size - written,
          "G%u",
          static_cast<int>(input.joystick.y * MAX_ANALOG_VALUE)
      );
    }
    if (input.joystick.press) {
      written += snprintf(
          reinterpret_cast<char *const>(buffer + written),
          buffer_size - written,
          "H"
        );
    }

    const auto& buttons = input.buttons;
    for (auto i = 0; i < buttons.size(); i++) {
      const auto& button = buttons[i];
      if (button.press) {
        const auto& buttonKey = AlphaEncoding::BUTTON_ALPHA_KEY[i];
        written += snprintf(
            reinterpret_cast<char *const>(buffer + written),
            buffer_size - written,
            "%c",
            buttonKey
          );
      }
    }

    const auto& analog_buttons = input.analog_buttons;
    for (auto i = 0; i < analog_buttons.size(); i++) {
      const auto& button = analog_buttons[i];
      if (button.press) {
        const auto& buttonKey = AlphaEncoding::ANALOG_BUTTON_ALPHA_KEY[i];
        written += snprintf(
            reinterpret_cast<char *const>(buffer + written),
            buffer_size - written,
            "%c",
            buttonKey
        );
      }
    }

    written += snprintf(
        reinterpret_cast<char *const>(buffer + written),
        buffer_size - written,
        "\n"
    );

    return written;
  }
} // namespace opengloves
