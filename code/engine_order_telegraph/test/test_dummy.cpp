#include <gtest/gtest.h>

#if defined(ARDUINO)
#include <Arduino.h>

void setup() {
    Serial.begin(115200); // same as "test_speed" in platformio.ini
}

void loop() {
    if (RUN_ALL_TESTS()) {
        // Run all tests.
    }

    delay(1000);
}

#else
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    // if you plan to use GMock, replace the line above with
    // ::testing::InitGoogleMock(&argc, argv);

    if (RUN_ALL_TESTS())
    ;

    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}
#endif