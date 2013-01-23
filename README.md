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

