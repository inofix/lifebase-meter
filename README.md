# lifebase-plant-basic-esp32

## Overview

This project is a skeleton implementation of an environment meter
that can be used to monitor (and adapt) the surrounding of living
systems.

The hardware is based on espressif's[1](#1) wonderful ESP32[2](#2) and
we use the development board from DOIT[3](#3) (DOIT ESP32 DEVKIT V1).

The current standard setup consists of just the following sensors

  * DHT22 - humidity[10](#10) and temperature sensor
  * TSL2591/TSL2561[10](#10) - to measure the light exposure
  * some capacitive soil moisture meter
  * some float switch to measure the water level and actuators
  * a pump

## Installation

### Arduino IDE

As my toolchain with the ESP IDF[4](#4) stopped working at
a certain point, I reimplemented it for the Arduino[8](#8) IDE
just to see whether it would work there, and it
immediately just did.
Over the last year I was asked to do workshops with children on that
basis and as it is probably easier to start with Arduino, I think
we'll just stay with this solution:

  * Install the Arduino IDE
  * Add the board
    * Add an additional boards manager
      * From "Files" choose "Preferences"
      * Under "Additional Boards Manager URLs" enter URL: `https://dl.espressif.com/dl/package_esp32_index.json`
    * Now go to: "Tools", "Board"
    * From "Boards Manager" choose "ESP32 Arduino"
    * Exit this level and then choose the following from the resp. menu:
      * Board: "ESP32 Dev Module"
        * Partition Scheme: "Minimal SPIFFS" (otherwise the sketch is too big)
      * (do not use "DOIT ESP32 DEVKIT V1" as there are no options available here)
  * Add the libraries
    * Go to: "Sketch", "Include Library", "Manage Libraries"
    * Choose:
      * "Adafruit Unified Sensor"[11](#11)
      * "DHT sensor library"[12](#12)
      * "ESP32 BLE Arduino"[9](#9)
      * "Adafruit TSL2591" (or TSL2561 resp.)

The development itself is done in the subfolder called `arduino-ide`,
just open the main .ino file in your IDE - or in your favorite
editor.

To upload/run the code there are some further steps needed,
please see the section ['Running the Code'](#running-the-code)
below.

### ESP IDF

*Warning* This section (ESP IDF[4](#4) is currently not working. See the
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
`lifebase_meter-.conf.stub`. You can execute the script to get some
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
all the value's units recieved from the sensors - e.g. while for
light it is easy (i.e. Lux), we have absolutely no idea
how to handle
soil moisture. As a pragmatic approach we will just use
percentage for everything as a first approach and then
later replace it with the definite unit. Let's say, if
the BLE unit characteristic is not set, it is assumed
to be a temporarily percentage value.

Another problem poses the interpretation of the values.
E.g. with different soil types we have a different behavior
of moisture --
or with different lifeforms, we have different interpretations
for the light conditions. The plants do not care too much about
Lux, and their needs also differ between species and life phases.

---
  1. <a name="1" href="https://www.espressif.com/">https://www.espressif.com/</a>
  1. <a name="2" href="https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/index.html">https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/index.html</a>
  1. <a name="3" href="http://doit.am">http://doit.am</a>
  1. <a name="4" href="https://github.com/espressif/esp-idf.git">https://github.com/espressif/esp-idf.git</a>
  1. <a name="5" href="https://docs.espressif.com/projects/esp-idf/en/latest/">https://docs.espressif.com/projects/esp-idf/en/latest/</a>
  1. <a name="6" href="https://github.com/espressif/crosstool-NG.git">https://github.com/espressif/crosstool-NG.git</a>
  1. <a name="7" href="https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#setup-toolchain">https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#setup-toolchain</a>
  1. <a name="8" href="https://www.arduino.cc/">https://www.arduino.cc/</a>
  1. <a name="9" href="https://github.com/espressif/arduino-esp32.git">https://github.com/espressif/arduino-esp32.git</a>
  1. <a name="10" href="http://adafruit.com/">http://adafruit.com/</a>
  1. <a name="11" href="https://github.com/adafruit/Adafruitb_Sensor">https://github.com/adafruit/Adafruit_Sensor</a>
  1. <a name="12" href="https://github.com/adafruit/DHT-sensor-library">https://github.com/adafruit/DHT-sensor-library</a>
  1. <a name="13" href="https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html">https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html</a>
  1. <a name="14" href="https://github.com/espressif/arduino-esp32/blob/master/docs/esp-idf_component.md">https://github.com/espressif/arduino-esp32/blob/master/docs/esp-idf_component.md</a>
