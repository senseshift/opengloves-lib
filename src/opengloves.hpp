#pragma once

#include <array>
#include <variant>
#include <cstdint>

namespace opengloves {
    using HandIndex = std::uint8_t;
    enum Hand : HandIndex {
        Hand_Left,
        Hand_Right,
    };

    using DeviceTypeIndex = std::uint8_t;
    enum DeviceType : DeviceTypeIndex {
        DeviceType_LucidGloves = 0,
        DeviceType_Other = 255,
    };

    template<typename Tf = float>
    union InputFingerCurl {
        /// Access the curl as an array.
        std::array<Tf, 4> curl; // NOLINT(*-magic-numbers): I'm sure our fingers aren't changing anytime soon

        union {
            /// The total curl of the finger.
            /// Only use it if you do not use per-joint tracking.
            Tf curl_total;

            /// Access the individual curl joints.
            struct {
                Tf curl_joint0;
                Tf curl_joint1;
                Tf curl_joint2;
                Tf curl_joint3;
            };
        };

        auto operator==(const InputFingerCurl& other) const -> bool
        {
            return this->curl_joint0 == other.curl_joint0 && this->curl_joint1 == other.curl_joint1
                   && this->curl_joint2 == other.curl_joint2 && this->curl_joint3 == other.curl_joint3;
        }
    };
    using InputFingerCurlData = InputFingerCurl<float>;

    template<typename Tp>
    union InputFingers {
        std::array<Tp, 5> fingers; // NOLINT(*-magic-numbers): We aren't going to grow any new fingers soon tbh
        struct {
            Tp thumb;
            Tp index;
            Tp middle;
            Tp ring;

            union {
                Tp pinky;
                Tp little;
            };
        };

        auto operator==(const InputFingers& other) const -> bool
        {
            return this->thumb == other.thumb && this->index == other.index && this->middle == other.middle
                   && this->ring == other.ring && this->pinky == other.pinky;
        }
    };
    using InputFingersData = InputFingers<float>;

    template<typename Tf = float, typename Tb = bool>
    struct InputJoystick {
        Tf x, y;
        Tb press;
    };
    using InputJoystickData = InputJoystick<float, bool>;

    template<typename Tb = bool>
    struct InputButton {
        Tb press;
        // bool touch;
    };
    using InputButtonData = InputButton<bool>;

    template<typename Tf = float, typename Tb = bool>
    struct InputAnalogButton : public InputButton<Tb> {
        Tf value;
    };
    using InputAnalogButtonData = InputAnalogButton<float, bool>;

    /// Input data structure.
    ///
    /// I know, it is not the prettiest one, but we need this type of punning to efficiently encode/decode the data
    template<typename Tf = float, typename Tb = bool>
    struct InputPeripheral {
        template<
          typename U = Tf,
          typename V = Tb,
          std::enable_if_t<std::is_floating_point_v<U> && std::is_same_v<V, bool>, bool> = true>
        InputPeripheral()
        {
            this->curl.fingers = { {
              { 0.0F, 0.0F, 0.0F, 0.0F },
              { 0.0F, 0.0F, 0.0F, 0.0F },
              { 0.0F, 0.0F, 0.0F, 0.0F },
              { 0.0F, 0.0F, 0.0F, 0.0F },
              { 0.0F, 0.0F, 0.0F, 0.0F },
            } };
            this->splay.fingers = { 0.0F, 0.0F, 0.0F, 0.0F, 0.0F };
            this->joystick = { 0.0F, 0.0F, false };
            this->buttons = { { false, false, false, false, false } };
            this->analog_buttons = { { { false, 0.0F }, { false, 0.0F } } };
        }

        template<
          typename U = Tf,
          typename V = Tb,
          std::enable_if_t<std::is_pointer_v<U> && std::is_pointer_v<V>, bool> = true>
        InputPeripheral()
        {
            this->curl.fingers = { {
              { nullptr, nullptr, nullptr, nullptr },
              { nullptr, nullptr, nullptr, nullptr },
              { nullptr, nullptr, nullptr, nullptr },
              { nullptr, nullptr, nullptr, nullptr },
              { nullptr, nullptr, nullptr, nullptr },
            } };
            this->splay.fingers = { nullptr, nullptr, nullptr, nullptr, nullptr };
            this->joystick = { nullptr, nullptr, nullptr };
            this->buttons = { { nullptr, nullptr, nullptr, nullptr, nullptr } };
            this->analog_buttons = { { { nullptr, nullptr }, { nullptr, nullptr } } };
        }

        InputFingers<InputFingerCurl<Tf>> curl;
        InputFingers<Tf> splay;

        InputJoystick<Tf, Tb> joystick;

        union {
            /// Buttons as array.
            /// <b>MUST</b> be the same length as the struct below
            std::array<InputButton<Tb>, 5> buttons; // NOLINT(*-magic-numbers) We keep it here for clarity

            struct {
                InputButton<Tb> button_a;
                InputButton<Tb> button_b;
                InputButton<Tb> button_menu;
                InputButton<Tb> button_calibrate;
                InputButton<Tb> pinch;
            };
        };

        union {
            std::array<InputAnalogButton<Tf, Tb>, 2> analog_buttons;
            struct {
                InputAnalogButton<Tf, Tb> trigger;
                InputAnalogButton<Tf, Tb> grab;
            };
        };
    };
    using InputPeripheralData = InputPeripheral<float, bool>;

    struct InputInfoData {
        Hand hand;
        DeviceType device_type;

        unsigned int firmware_version;
    };

    struct InputInvalid {
        auto operator==(const InputInvalid& /*unused*/) const -> bool { return true; }
    };

    using InputData = std::variant<InputInvalid, InputInfoData, InputPeripheralData>;

    template<typename Tf = float, typename Tb = bool>
    using OutputForceFeedback = InputFingers<Tf>;
    using OutputForceFeedbackData = OutputForceFeedback<float, bool>;

    template<typename Tf = float, typename Tb = bool>
    struct OutputHaptics {
        Tf frequency;
        Tf duration;
        Tf amplitude;

        auto operator==(const OutputHaptics& other) const -> bool
        {
            return frequency == other.frequency && duration == other.duration && amplitude == other.amplitude;
        }
    };
    using OutputHapticsData = OutputHaptics<float, bool>;

    class OutputInvalid {
        auto operator==(const OutputInvalid& /*unused*/) const -> bool { return true; }
    };
    using OutputData = std::variant<OutputInvalid, OutputForceFeedbackData, OutputHapticsData>;

}