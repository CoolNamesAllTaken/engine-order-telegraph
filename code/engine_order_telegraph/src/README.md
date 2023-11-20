## Programmer Setup
[ESP-Prog Setup Instructions](https://docs.platformio.org/en/latest/plus/debug-tools/esp-prog.html)

# Troubleshooting
### Can't program ESP32, with `libusb_open() failed` error message.
Use Zadig to replace the driver on Dual RS232-HS (Interface 0) again with the WinUSB driver. This needs to be done every time the ESP-Prog adapter gets assigned a new COM port. Ugh.