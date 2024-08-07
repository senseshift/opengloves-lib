#pragma once

#include <opengloves.hpp>

#include <map>
#include <variant>

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
      static auto encodeInput(const InputData& input, uint8_t* buffer, int buffer_size) -> int;
      static auto encodeInputInfo(const InputInfoData& input, uint8_t* buffer, int buffer_size) -> int;
      static auto encodeInputPeripheral(const InputPeripheralData& input, uint8_t* buffer, int buffer_size) -> int;

      static auto encodeOutput(const OutputData& output, uint8_t* buffer, int buffer_size) -> int;
      static auto encodeOutputForceFeedback(const OutputForceFeedbackData& output, uint8_t* buffer, int buffer_size) -> int;
      static auto encodeOutputHaptics(const OutputHapticsData& output, uint8_t* buffer, int buffer_size) -> int;

      static auto decodeOutput(const uint8_t* buffer, size_t buffer_size) -> OutputData;

      static auto splitPairs(const char* buffer, size_t buffer_size, std::map<std::string, std::string>& pairs) -> void;
  };

  inline auto AlphaEncoding::encodeInput(const InputData &input, uint8_t *buffer, int buffer_size) -> int {
    if (std::holds_alternative<InputPeripheralData>(input)) {
      return AlphaEncoding::encodeInputPeripheral(std::get<InputPeripheralData>(input), buffer, buffer_size);
    } else if (std::holds_alternative<InputInfoData>(input)) {
      return AlphaEncoding::encodeInputInfo(std::get<InputInfoData>(input), buffer, buffer_size);
    }

    return 0;
  }

  inline auto AlphaEncoding::encodeInputInfo(const InputInfoData &input, uint8_t *buffer, int buffer_size) -> int {
    const auto& keyFirmwareVersion = AlphaEncoding::INFO_FIRMWARE_VERSION_KEY;
    const auto& keyDeviceType = AlphaEncoding::INFO_DEVICE_TYPE_KEY;
    const auto& keyHand = AlphaEncoding::INFO_HAND_KEY;

    return snprintf(
        reinterpret_cast<char*>(buffer),
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

  inline auto AlphaEncoding::encodeInputPeripheral(const InputPeripheralData &input, uint8_t *buffer, int buffer_size) -> int {
    auto written = 0;

    const auto& curls = input.curl.fingers;
    const auto& splays = input.splay.fingers;

    for (size_t i = 0; i < curls.size(); i++) {
      const auto &finger_curl = curls[i];
      const auto &finger_splay = splays[i];
      const char finger_alpha_key = 'A' + static_cast<char>(i);

      int n = snprintf(
        reinterpret_cast<char*>(buffer + written),
        buffer_size - written,
        "%c%u",
        finger_alpha_key,
        static_cast<int>(finger_curl.curl_total * MAX_ANALOG_VALUE)
      );
      if (n < 0 || n >= buffer_size - written) {
        return written;
      }
      written += n;

      if (finger_splay > 0.0F) {
        n = snprintf(
            reinterpret_cast<char*>(buffer + written),
            buffer_size - written,
            "(%cB)%u",
            finger_alpha_key,
            static_cast<int>(finger_splay * MAX_ANALOG_VALUE)
        );
        if (n < 0 || n >= buffer_size - written) {
          return written;
        }
        written += n;
      }

      const auto& joints = finger_curl.curl;
      for (size_t j = 1; j < joints.size(); j++) {
        const auto& joint = joints[j];
        const char joint_alpha_key = 'A' + j;

        if (joint == 0.0F) {
          continue;
        }

        n = snprintf(
            reinterpret_cast<char*>(buffer + written),
            buffer_size - written,
            "(%cA%c)%u",
            finger_alpha_key,
            joint_alpha_key,
            static_cast<int>(joint * MAX_ANALOG_VALUE)
        );
        if (n < 0 || n >= buffer_size - written) {
          return written;
        }
        written += n;
      }
    }

    if (input.joystick.x != 0.0F) {
      int n = snprintf(
          reinterpret_cast<char*>(buffer + written),
          buffer_size - written,
          "F%u",
          static_cast<int>(input.joystick.x * MAX_ANALOG_VALUE)
        );
      if (n < 0 || n >= buffer_size - written) {
        return written;
      }
      written += n;
    }
    if (input.joystick.y != 0.0F) {
      int n = snprintf(
          reinterpret_cast<char*>(buffer + written),
          buffer_size - written,
          "G%u",
          static_cast<int>(input.joystick.y * MAX_ANALOG_VALUE)
      );
      if (n < 0 || n >= buffer_size - written) {
        return written;
      }
      written += n;
    }
    if (input.joystick.press) {
      int n = snprintf(
          reinterpret_cast<char*>(buffer + written),
          buffer_size - written,
          "H"
        );
      if (n < 0 || n >= buffer_size - written) {
        return written;
      }
      written += n;
    }

    const auto& buttons = input.buttons;
    for (size_t i = 0; i < buttons.size(); i++) {
      const auto& button = buttons[i];
      if (button.press) {
        const auto& buttonKey = AlphaEncoding::BUTTON_ALPHA_KEY[i];
        int n = snprintf(
            reinterpret_cast<char*>(buffer + written),
            buffer_size - written,
            "%c",
            buttonKey
          );

        if (n < 0 || n >= buffer_size - written) {
          return written;
        }
        written += n;
      }
    }

    const auto& analog_buttons = input.analog_buttons;
    for (size_t i = 0; i < analog_buttons.size(); i++) {
      const auto& button = analog_buttons[i];
      if (button.press) {
        const auto& buttonKey = AlphaEncoding::ANALOG_BUTTON_ALPHA_KEY[i];
        int n = snprintf(
            reinterpret_cast<char*>(buffer + written),
            buffer_size - written,
            "%c",
            buttonKey
        );
        if (n < 0 || n >= buffer_size - written) {
          return written;
        }
        written += n;
      }
    }

    int n = snprintf(
        reinterpret_cast<char*>(buffer + written),
        buffer_size - written,
        "\n"
    );
    if (n < 0 || n >= buffer_size - written) {
      return written;
    }
    written += n;

    return written;
  }

  inline auto AlphaEncoding::encodeOutput(const OutputData &output, uint8_t *buffer, int buffer_size) -> int {
    if (std::holds_alternative<OutputForceFeedbackData>(output)) {
      return AlphaEncoding::encodeOutputForceFeedback(std::get<OutputForceFeedbackData>(output), buffer, buffer_size);
    } else if (std::holds_alternative<OutputHapticsData>(output)) {
      return AlphaEncoding::encodeOutputHaptics(std::get<OutputHapticsData>(output), buffer, buffer_size);
    }

    return 0;
  }

  inline auto AlphaEncoding::encodeOutputForceFeedback(const OutputForceFeedbackData &output, uint8_t *buffer, int buffer_size) -> int {
    return snprintf(
        reinterpret_cast<char*>(buffer),
        buffer_size,
        "A%uB%uC%uD%uE%u\n",
        static_cast<int>(output.thumb * MAX_ANALOG_VALUE),
        static_cast<int>(output.index * MAX_ANALOG_VALUE),
        static_cast<int>(output.middle * MAX_ANALOG_VALUE),
        static_cast<int>(output.ring * MAX_ANALOG_VALUE),
        static_cast<int>(output.pinky * MAX_ANALOG_VALUE)
    );
  }

  inline auto AlphaEncoding::encodeOutputHaptics(const opengloves::OutputHapticsData &output, uint8_t *buffer, int buffer_size) -> int {
    return snprintf(
        reinterpret_cast<char*>(buffer),
        buffer_size,
        "F%.2fG%.2fH%.2f\n",
        output.frequency,
        output.duration,
        output.amplitude
    );
  }

  inline auto AlphaEncoding::decodeOutput(const uint8_t *buffer, size_t buffer_size) -> OutputData {
    if (buffer_size == 0) {
      return OutputInvalid{};
    }

    auto map = std::map<std::string, std::string>();
    AlphaEncoding::splitPairs(reinterpret_cast<const char*>(buffer), buffer_size, map);

    if (map.empty()) {
      return OutputInvalid{};
    }

    // We assume all commands are for ffb, if there is any ffb command
    const auto& thumb_curl = map.find("A");
    const auto& index_curl = map.find("B");
    const auto& middle_curl = map.find("C");
    const auto& ring_curl = map.find("D");
    const auto& pinky_curl = map.find("E");

    if (thumb_curl != map.end() || index_curl != map.end() || middle_curl != map.end() ||
        ring_curl != map.end() || pinky_curl != map.end()
    ) {
      OutputForceFeedbackData ffb{};

      if (thumb_curl != map.end()) {
        ffb.thumb = std::stof(thumb_curl->second) / MAX_ANALOG_VALUE;
      }

      if (index_curl != map.end()) {
        ffb.index = std::stof(index_curl->second) / MAX_ANALOG_VALUE;
      }

      if (middle_curl != map.end()) {
        ffb.middle = std::stof(middle_curl->second) / MAX_ANALOG_VALUE;
      }

      if (ring_curl != map.end()) {
        ffb.ring = std::stof(ring_curl->second) / MAX_ANALOG_VALUE;
      }

      if (pinky_curl != map.end()) {
        ffb.pinky = std::stof(pinky_curl->second) / MAX_ANALOG_VALUE;
      }

      return ffb;
    }

    const auto& frequency = map.find("F");
    const auto& duration = map.find("G");
    const auto& amplitude = map.find("H");

    if (frequency != map.end() || duration != map.end() || amplitude != map.end()) {
      OutputHapticsData haptics{};
    
      if (frequency != map.end()) {
            haptics.frequency = std::stof(frequency->second);
      }

      if (duration != map.end()) {
            haptics.duration = std::stof(duration->second);
      }

      if (amplitude != map.end()) {
            haptics.amplitude = std::stof(amplitude->second);
      }

      return haptics;
    }

    return OutputInvalid{};
  }

  // todo: in theory, we can use std::string_view here, or const char*
  inline auto AlphaEncoding::splitPairs(const char *buffer, size_t buffer_size, std::map<std::string, std::string> &pairs) -> void {
    pairs.clear();

    if (buffer_size == 0) {
      return;
    }

    // we will split AAA100BBB200 into pairs of (AAA, 100), (BBB, 200)

    const char* keyStart = buffer;
    const char* keyEnd = buffer;
    const char* valueStart = nullptr;

    for (size_t i = 0; i < buffer_size; i++) {
      if (isdigit(buffer[i]) || buffer[i] == '.') {
        if (valueStart == nullptr) {
          keyEnd = buffer + i;
          valueStart = buffer + i;
        }
      } else {
        if (valueStart != nullptr) {
          pairs.emplace(
              std::string(keyStart, keyEnd),
              std::string(valueStart, buffer + i)
          );
          keyStart = buffer + i;
          valueStart = nullptr;
        }
      }
    }

    // Insert the last pair if any
    if (valueStart != nullptr && keyStart != valueStart) {
      pairs.emplace(
          std::string(keyStart, keyEnd),
          std::string(valueStart, buffer + buffer_size)
      );
    }
  }
} // namespace opengloves
