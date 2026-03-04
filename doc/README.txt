ESP32-2432S028
AKA Cheap Yellow Board (CYD)

2.8-inch TFT touchscreen 320x240
ST7789 lcd driver
SD card slot
RGB LED

ESP32-2432S028
ESP-WROOM-32 Wifi b/g/n BT v4.2
240MHz
32MBit QSPI Flash. 520 KB SRAM
DIO Flash Mode
Upgrade Speed 921600
Xtensa Architecture

esptool.py                    \
  --chip esp32                \
  --baud 921600               \
  write_flash                 \
    --flash_mode dio          \
    --flash_size 32Mbit       \
    0x1000    bootloader.bin  \
    0x8000    partitions.bin  \
    0xe000    boot_app0.bin   \
    0x10000   firmware.bin


Reference Link
1. https://github.com/hexeguitar/ESP32_TFT_PIO
2. https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display
3. https://www.espboards.dev/esp32/cyd-esp32-2432s028/
4. https://kafkar.com/projects/smart-home/understanding-connectors-and-pinout-cheap-yellow-display-boardcyd-esp32-2432s028r/
