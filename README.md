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

The development itself is done in the subfolder called `arduino-ide`,
just open the main .ino file in your IDE - or in your favorite
editor.

To upload/run the code there are some further steps needed,
please see the section ['Running the Code'](#running-the-code)
below.

### ESP IDF

*Warning* This section is currently not working. See the
Arduino IDE section above..


## Running the Code

Prior to using the code on your ESP32, you will have to individualize
the installation. There are a few things that you will have to define
for the project to be usable, this is first and foremost the individual
UUID per device, but also which services should be enabled/disabled
and how they will be propagated later. You can easily identify all the
places in the code by searching their placeholders double wrapped in
braces (curly brackets), e.g. something like: '{{ FOO\_BAR }}'.

We have created a custom (bash) shell script for this purpose. This is a quick
and dirty solution which will have to be replaced in the future, but for the
moment it is quite handy for us. If you do not have access to a bash, you
will have to change the values manually/semi-manually. This is how to use
the script.

Change to the top folder of your project, where you will find both
files, the script `create_config.sh` and an empty config
`lifebase_meter-example.conf`. You can execute the script to get some
idea with the help option:

    $ ./create_config.sh --help

For a standard installation with a random UUID one could run this:

    $ ./create_config.sh Foobar

This command would then create a customized installation and a configfile
for future builds under the folder `build` as follows:

    $ ls build/
    lifebase_meter-Foobar

The resulting arduino project `lifebase_meter-Foobar.ino` can then be loaded
from within the Arduino IDE and flashed onto the ESP32 as usual.


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
