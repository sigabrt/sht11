sht11
=====

Basic, hacky code for reading the SHT11 temp/humidity sensor with an AVR chip.

Building
========

Requirements: scons, avr-gcc & friends.

    scons sht11.hex

to build the hex file.

    scons sht11.upload

to upload. Currently this is tied to my setup, which is an ATMega328p with the arduino bootloader for simple USB/serial programming.

Notes
=====

Both the clock and data pins should have external pullups, as the datasheet prohibits the mcu from driving them high itself. This is the reason for the odd-looking hi/lo macros in the code; to send a high pulse, the pin is configured as an input, which allows the pullups to yank it high. For a low pulse, the pin is set as an output, which by default drives the pin low.
