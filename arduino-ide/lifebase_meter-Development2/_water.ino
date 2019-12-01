/*
    Life Base's Setup Meter

    This is a generic stack of lifebase related standard interactions with
    the environment of subjects of interest.

    This work was inspired by examples from the arduino-ide and esp-idf, resp.
    arduino-esp:

    - https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/examples/
    - https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp

    For copyright and/or -left, warranty, terms of use, and such information,
    please have a look at the LICENSE file in the topmost directory...
*/

#if defined WATER_SERVICE_UUID

static void init_water() {

    pinMode(WATERCACHEPOTLEVELMINPIN, INPUT);
    pinMode(WATERCACHEPOTLEVELMAXPIN, INPUT);
    pinMode(WATERPUMPPIN, OUTPUT);
}

static void init_ble_water(BLEServer* ble_server) {

    BLEService *water_service = ble_server->createService(WATER_SERVICE_UUID);
    water_cachepot_level_min_characteristic = water_service->createCharacteristic(
            WATER_CACHEPOT_LEVEL_MIN_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    water_cachepot_level_max_characteristic = water_service->createCharacteristic(
            WATER_CACHEPOT_LEVEL_MAX_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    water_pump_characteristic = water_service->createCharacteristic(
            WATER_PUMP_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    water_cachepot_level_min_characteristic->addDescriptor(new BLE2902());
    water_cachepot_level_max_characteristic->addDescriptor(new BLE2902());
    water_pump_characteristic->addDescriptor(new BLE2902());
    BLE2904 *desc0 = new BLE2904();
    desc0->setFormat(0x01);
    desc0->setUnit(0x2700);
    BLE2904 *desc1 = new BLE2904();
    desc1->setFormat(0x01);
    desc1->setUnit(0x2700);
    BLE2904 *desc2 = new BLE2904();
    desc2->setFormat(0x01);
    desc2->setUnit(0x2700);

    water_service->start();
}

static void pump_water() {

    if (is_too_dry) {
        for (int i = 0; i < PUMP_ON_COUNT; i++) {
            if (!is_too_wet) {
                digitalWrite(WATERPUMPPIN, HIGH);
                set_ble_characteristic(water_pump_characteristic, "0");
                Serial.println("Pump is on...");
            }
            delay(PUMP_ON_TIME);
            digitalWrite(WATERPUMPPIN, LOW);
            set_ble_characteristic(water_pump_characteristic, "1");
            Serial.println("Pump is off...");
            loop_delay -= PUMP_ON_TIME;
        }
    }
}

//TODO: it is probably more a problem of the device than the view...
//TODO:
static void get_water_info() {

    // just to be sure ..
    digitalWrite(WATERPUMPPIN, LOW);
    set_ble_characteristic(water_pump_characteristic, "1");
    //TODO: the max level is not used yet..
    if (digitalRead(WATERCACHEPOTLEVELMINPIN)) {
        set_ble_characteristic(water_cachepot_level_min_characteristic, "1");
        Serial.println("The cachepot is full enough for the watering pump.");
        pump_water();
    } else {
        set_ble_characteristic(water_cachepot_level_min_characteristic, "0");
        Serial.println("WARNING: Please do fill the cachepot!");
    }
    // just to be sure ..
    digitalWrite(WATERPUMPPIN, LOW);
    set_ble_characteristic(water_pump_characteristic, "1");
    Serial.println("Pump is off...");
}

#endif
