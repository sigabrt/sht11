sht11
=====

Basic code for reading the SHT11 temp/humidity sensor with an AVR chip.

Building
========

Requirements: scons, avr-gcc & friends. The SConstruct builds and deploys with an atmega328p in mind, modify appropriately for your setup.

To build the hex file:

    scons sht11.hex

To upload:

    scons sht11.upload

The SConstruct assumes an stk500 or equivalent at /dev/ttyACM0. Verified to work with an Arduino UNO.

Notes
=====

Both the clock and data pins should have external pullups, as the datasheet prohibits the mcu from driving them high itself. This is the reason for the odd-looking hi/lo macros in the code; to send a high pulse, the pin is configured as an input, which allows the pullups to yank it high. For a low pulse, the pin is set as an output, which by default drives the pin low.
