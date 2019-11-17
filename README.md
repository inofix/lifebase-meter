# lifebase-plant-basic-esp32

## Overview

This project is an skeleton implementation of an environment meter
that can be used to monitor (and adapt) the surrounding of living
systems.

The hardware is based on espressif's[1] wonderful ESP32[2] and
we use the development board from DOIT[3] (DOIT ESP32 DEVKIT V1).

The current setup consists of just the following sensors

  * DHT22 - humidity and temperature sensor
  * TSL2561 - to measure the light exposure
  * some capacitive soil moisture meter
  * some float switch to measure the water level and actuators
  * a pump

## Installation

### Arduino IDE

As my toolchain with the ESP IDF stopped working at
a certain point. I tried out the Arduino IDE which
immediately just did work.
As I am not very happy with this solution, I will
try again with the ESP IDF later, but for now
we recommend using this solution:

  * Install the Arduino IDE
  * Add the board
    * Go to: "Tools", "Board"
    * Choose:
      * "DOIT ESP32 DEVKIT V1"
  * Add the libraries
   * Go to: "Sketch", "Include Library", "Manage Libraries"
   * Choose:
    * "Adafruit Unified Sensor"
    * "DHT sensor library"
    * "ESP32"
    * "ESP32 BLE Arduino"
    * "Adafruit TSL2591" (or TSL2561 resp.)

The development is in the subfolder calles `arduino-ide`, just
open the main .ino file in your IDE.

### ESP IDF

*Warning* This section is currently not working. See the
Arduino IDE section above..

## Bugs

Any bug report or feature request is warmly welcome via
the issue tracker here.

At the moment we have not quite decided yet how to handle
all the values recieved from the sensors - e.g. while for
light it is easy (i.e. Lux), we have absolutely no idea
how to handle
soil moisture. As a pragmatic approach we will just use
percentage for everything as a first approach and then
later replace it with the definite unit. Let's say, if
the BLE unit characteristic is not set, it is assumed
to be a temporarily percentage value.

---
    [1] https://www.espressif.com/
    [2] https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/index.html
    [3] http://doit.am
    [4] https://github.com/espressif/esp-idf.git
    [5] https://docs.espressif.com/projects/esp-idf/en/latest/
    [6] https://github.com/espressif/crosstool-NG.git
    [7] https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#setup-toolchain
    [8] https://www.arduino.cc/
    [9] https://github.com/espressif/arduino-esp32.git
    [10] http://adafruit.com/
    [11] https://github.com/adafruit/Adafruit\_Sensor
    [12] https://github.com/adafruit/DHT-sensor-library
    [13] https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
    [14] https://github.com/espressif/arduino-esp32/blob/master/docs/esp-idf_component.md
