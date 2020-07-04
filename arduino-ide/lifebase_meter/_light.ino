/*
    Life Base's Setup Meter

    This is a generic stack of lifebase related standard interactions with
    the environment of subjects of interest.

    This work was inspired by examples from the arduino-ide and esp-idf, resp.
    arduino-esp:

    - https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/examples/
    - https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    - https://github.com/adafruit/Adafruit_TSL2561

    BLE specs can be found here:

    - https://www.bluetooth.com/specifications/gatt/characteristics/
    - https://www.bluetooth.com/specifications/assigned-numbers/units/

    For copyright and/or -left, warranty, terms of use, and such information,
    please have a look at the LICENSE file in the topmost directory...
*/

#if defined LIGHT_SERVICE_UUID
#if LIGHT_EXPOSURE_SENSOR == 2561
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, LIGHT_EXPOSURE_I2C_UID);
#elif LIGHT_EXPOSURE_SENSOR == 2591
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);
#endif

static void init_light() {

    tsl.enableAutoRange(true);

    /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */
    tsl.begin();
}

static void init_ble_light(BLEServer* ble_server) {

    BLEService *light_service = ble_server->createService(LIGHT_SERVICE_UUID);
    light_exposure_characteristic = light_service->createCharacteristic(
            LIGHT_EXPOSURE_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    light_exposure_characteristic->addDescriptor(new BLE2902());
    // https://www.bluetooth.com/specifications/assigned-numbers/units/
    BLE2904 *desc0 = new BLE2904();
    desc0->setUnit(0x2731); // lux
    light_exposure_characteristic->addDescriptor(desc0);
    light_service->start();
}

//TODO: figure out how to display light exposure
//        - let's use a custom defined percentage value for now
static void get_light_info() {

    sensors_event_t event;
    tsl.getEvent(&event);

    int light_value = 0;
    if (event.light) {
        light_value = event.light;
    }

    char light_chars[3];
    dtostrf(light_value, 3, 0, light_chars);
    set_ble_characteristic(light_exposure_characteristic, light_chars);
    mqtt_publish(LIGHT_EXPOSURE_UUID, light_chars);
    Serial.print("The current light exposure is ");
    Serial.print(light_value);
    Serial.println(" Lux");
}

#endif
