#include "engine_order_telegraph.hh"

#define ABS(x) ((x) > 0 ? (x) : (-(x)))

/** Public Functions **/

EngineOrderTelegraph::EngineOrderTelegraph(EngineOrderTelegraph::EngineOrderTelegraphConfig config_in)
    : config_(config_in) 
{

}

void EngineOrderTelegraph::Init() {
    // Set up digital inputs.
    pinMode(config_.mute_select_pin, INPUT);

    //  Set up digital outputs.
    pinMode(config_.bell_enable_pin, OUTPUT);
    pinMode(config_.light_enable_pin, OUTPUT);

    // Set up servos.
    pinMode(config_.left_servo_enable_pin, OUTPUT);
    left_servo_.attach(config_.left_servo_pulse_pin);
    pinMode(config_.right_servo_enable_pin, OUTPUT);
    right_servo_.attach(config_.right_servo_pulse_pin);

    // Set up analog inputs.
    // Nothing required here for ESP32.

    // Ring bell when done initializing.
    RingBell(2);
}

void EngineOrderTelegraph::Update() {
    
    Serial.printf("Left Lever: %dcts(%d) Right Lever: %dcts(%d) Dimmer: %d Dings: %d\r\n", 
        left_lever_position_counts,
        left_lever_position,
        right_lever_position_counts,
        right_lever_position,
        analogRead(config_.light_dimmer_pos_pin),
        num_pending_bell_rings
    );
    // Loopback mode for testing.
    ReadLeverPositions();
    UpdateBell();
    UpdateServos();
    UpdateLight();
}

void EngineOrderTelegraph::DeInit() {
    left_servo_.detach();
    right_servo_.detach();
}

void EngineOrderTelegraph::RingBell(uint16_t num_rings=1) {
    num_pending_bell_rings += num_rings;
}

/** Private Functions **/

/**
 * @brief Checks position of both levers and enqueues bell rings if necessary. Called by Update().
*/
void EngineOrderTelegraph::ReadLeverPositions() {
    int old_left_lever_position = left_lever_position;
    int old_left_lever_position_counts = old_left_lever_position*kLeftLeverStepCounts + kLeftLeverStartCounts;
    // left_lever_position_counts = (left_lever_position_counts + analogRead(config_.left_lever_pos_pin)) / 2;
    left_lever_position_counts = analogRead(config_.left_lever_pos_pin);

    if (
        left_lever_position_counts < old_left_lever_position_counts + kLeftLeverStepCounts/2 - kLeverStepHysteresisCounts ||
        left_lever_position_counts > old_left_lever_position_counts - kLeftLeverStepCounts/2 + kLeverStepHysteresisCounts
    ) {
        // Only allow lever position update if hysteresis is overcome.
        left_lever_position = (left_lever_position_counts - kLeftLeverStartCounts + kLeftLeverStepCounts/2) / kLeftLeverStepCounts;
        volatile int absDeltaRings = ABS(left_lever_position-old_left_lever_position);
        RingBell(absDeltaRings);
    }

    int old_right_lever_position = right_lever_position;
    int old_right_lever_position_counts = old_right_lever_position*kRightLeverStepCounts + kRightLeverStartCounts;
    right_lever_position_counts = analogRead(config_.right_lever_pos_pin);
    if (
        right_lever_position_counts > old_right_lever_position_counts + kRightLeverStepCounts/2 + kLeverStepHysteresisCounts ||
        right_lever_position_counts < old_right_lever_position_counts - kRightLeverStepCounts/2 - kLeverStepHysteresisCounts
    ) {
        // Only allow lever position update if hysteresis is overcome.
        right_lever_position = (right_lever_position_counts - kRightLeverStartCounts + kRightLeverStepCounts/2) / kRightLeverStepCounts;
        volatile int absDeltaRings = ABS(right_lever_position-old_right_lever_position);
        RingBell(absDeltaRings);
    }
}

/**
 * @brief Updates the bell plunger to extend / retract if necessary to ring the bell. Called by Update().
*/
void EngineOrderTelegraph::UpdateBell() {
    
    muted_ = digitalRead(config_.mute_select_pin);
    if (muted_) {
        num_pending_bell_rings = 0; // Clear all pending rings if muted.
    }

    uint32_t current_timestamp_ms = millis();
    if (bell_plunger_down_ && (current_timestamp_ms - last_bell_ring_timestamp_ms_ > kBellPlungerDownIntervalMs)) {
        // Bell plunger is currently depressed, and it's time to retract it.
        digitalWrite(config_.bell_enable_pin, 0);
        bell_plunger_down_ = false;
    }
    else if (
        // Could add a mute check here if we thought num_pending_bell_rings might get modified after the initial mute check.
        num_pending_bell_rings > 0 && 
        current_timestamp_ms - last_bell_ring_timestamp_ms_ > kMinBellRingIntervalMs
    ) {
        last_bell_ring_timestamp_ms_ = current_timestamp_ms;
        digitalWrite(config_.bell_enable_pin, 1);
        bell_plunger_down_ = true;
        num_pending_bell_rings--;
    }
}

/**
 * @brief Moves the servos as required, and powers them down when they're not in use to keep them quiet. Called by Update().
*/
void EngineOrderTelegraph::UpdateServos() {
    if (left_servo_position != left_servo_last_written_position || right_servo_position != right_servo_last_written_position) {
        // Servos need to move!
        // Enable the servos.
        last_servo_move_timestamp_ms_ = millis();
        servos_enabled_ = true;
        digitalWrite(config_.left_servo_enable_pin, 1);
        digitalWrite(config_.right_servo_enable_pin, 1);

        // Convert positions to degrees and write to the servos.
        left_servo_.write(kServoEndDegrees - left_servo_position*kServoPositionIncrementDegrees);
        right_servo_.write(right_servo_position*kServoPositionIncrementDegrees + kServoStartDegrees);

        // Update the last written servo positions so we don't come back here till the desired position changes.
        left_servo_last_written_position = left_servo_position;
        right_servo_last_written_position = right_servo_position;

    } else if (servos_enabled_ && millis() > last_servo_move_timestamp_ms_ + kServoMoveIntervalMs) {
        // Time to turn off the servos, they've had enough time to move.
        servos_enabled_ = false;
        digitalWrite(config_.left_servo_enable_pin, 0);
        digitalWrite(config_.right_servo_enable_pin, 0);
    }
    
}

/**
 * @brief Updates the light as required.
*/
void EngineOrderTelegraph::UpdateLight() {
    light_brightness_ = analogRead(config_.light_dimmer_pos_pin) * kLightBrightnessMaxCounts / kLightDimmerMaxCounts;
     analogWrite(config_.light_enable_pin, light_brightness_);
}