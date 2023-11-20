#ifndef _ENGINE_ORDER_TELEGRAPH_HH_
#define _ENGINE_ORDER_TELEGRAPH_HH_

#include "Arduino.h"
#include "esp32_servo.hh"

class EngineOrderTelegraph {
public:
    struct EngineOrderTelegraphConfig {
        // Digital Outputs
        uint16_t bell_enable_pin = 26;
        uint16_t light_enable_pin = 27;
        uint16_t left_servo_pulse_pin = 25;
        uint16_t left_servo_enable_pin = 33;
        uint16_t right_servo_pulse_pin = 32;
        uint16_t right_servo_enable_pin = 21;

        // Analog Inputs
        uint16_t light_dimmer_pos_pin = 34;
        uint16_t left_lever_pos_pin = 36;
        uint16_t right_lever_pos_pin = 39;

        // Digital Input
        uint16_t mute_select_pin = 2;
    };

    static const uint8_t kNumPositions = 7; // Number of lever or indicator positions.
    // const uint16_t kLeftLeverThresholds[kNumPositions] = {2580, 2200, 1800, 1400, 990, 590, 255};
    // const uint16_t kRightLeverThresholds[kNumPositions] = {1300, 1540, 1900, 2320, 2730, 3160, 3650};
    const int kLeftLeverStartCounts = 2580; // position 0
    const int kLeftLeverStepCounts = -400;
    const int kRightLeverStartCounts = 1150; // position 0
    const int kRightLeverStepCounts = 400;

    // Servo Constants
    const uint16_t kServoStartDegrees = 0;
    const uint16_t kServoEndDegrees = 180;
    const uint16_t kServoPositionIncrementDegrees = 30;
    const uint16_t kServoMoveIntervalMs = 1000; // How long to keep servos on to let them move.

    // Lever Constants
    const uint16_t kLeverMaxCounts = 0xFFF;
    // const uint16_t kLeverStartCounts = 0;
    // const uint16_t kLeverEndCounts = 0xFF;
    // const uint16_t kLeverPositionIncrementCounts = (kLeverEndCounts - kLeverStartCounts) / kNumPositions;

    // Bell Constants
    const uint16_t kMinBellRingIntervalMs = 200;
    const uint16_t kBellPlungerDownIntervalMs = 50;

    EngineOrderTelegraph(EngineOrderTelegraphConfig config_in);
    void Init();
    void Update();
    void DeInit();
    void RingBell(uint16_t num_rings);

    // Lever Variables
    int left_lever_position_counts = 0;
    int left_lever_position = 0;
    int right_lever_position_counts = 0;
    int right_lever_position = 0;

    // Servo Variables
    uint8_t left_servo_position = 4;
    uint8_t right_servo_position = 4;

    // Bell Variables
    uint16_t num_pending_bell_rings = 0;

    // Light Variables
    uint16_t kLightDimmerMaxCounts = 0xFFF;
    uint16_t kLightBrightnessMaxCounts = 0xFF;

private:
    EngineOrderTelegraphConfig config_;

    Servo left_servo_;
    Servo right_servo_;

    void ReadLeverPositions();
    void UpdateBell();
    void UpdateServos();
    void UpdateLight();

    // Servo Variables
    // Set last written servo positions out of bounds to trigger a new write on startup.
    uint8_t left_servo_last_written_position = 100;
    uint8_t right_servo_last_written_position = 100;
    // Enable and disable left and right servos together to make things easy.
    uint32_t last_servo_move_timestamp_ms_ = 0;
    bool servos_enabled_ = false;

    // Bell Variables
    uint32_t last_bell_ring_timestamp_ms_ = 0;
    bool bell_plunger_down_ = false;
    bool muted_ = false;

    // Light Variables
    uint8_t light_brightness_ = 0;

};

#endif /* _ENGINE_ORDER_TELEGRAPH_HH_ */