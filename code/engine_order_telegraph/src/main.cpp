#include <Arduino.h>

#include "engine_order_telegraph.hh"

EngineOrderTelegraph * telegraph;
static uint32_t last_bell_ring_ms = millis();
const uint16_t kBellRingIntervalMs = 2000;

void setup() {
    EngineOrderTelegraph::EngineOrderTelegraphConfig telegraph_config = EngineOrderTelegraph::EngineOrderTelegraphConfig();
    // Override default configuration here if required.

    telegraph = new EngineOrderTelegraph(telegraph_config);
    Serial.begin(115200);
    telegraph->Init();

    #ifdef SETUP
    telegraph->left_servo_position = 0;
    telegraph->right_servo_position = 0;
    while (true) {};
    #endif
}

void loop() {
    

    uint32_t current_timestamp_ms = millis();
    // if (current_timestamp_ms > last_bell_ring_ms + kBellRingIntervalMs) {
    //     telegraph->RingBell(2);
    //     last_bell_ring_ms = current_timestamp_ms;

    //     telegraph->left_servo_position++;
    //     telegraph->left_servo_position = telegraph->left_servo_position > telegraph->kNumPositions ? 0 : telegraph->left_servo_position;

    //     telegraph->right_servo_position++;
    //     telegraph->right_servo_position = telegraph->right_servo_position > telegraph->kNumPositions ? 0 : telegraph->right_servo_position;
    // }
    telegraph->left_servo_position = telegraph->right_lever_position;
    telegraph->right_servo_position = telegraph->left_lever_position;
    telegraph->Update();
}