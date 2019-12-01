/*
    Life Base's Setup Meter

    This is a generic stack of lifebase related standard interactions with
    the environment of subjects of interest.

    This work was inspired by examples from the arduino-ide and esp-idf, resp.
    arduino-esp:

    - https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/examples/
    - https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp

    BLE specs can be found here:

    - https://www.bluetooth.com/specifications/gatt/characteristics/
    - https://www.bluetooth.com/specifications/assigned-numbers/units/

    For copyright and/or -left, warranty, terms of use, and such information,
    please have a look at the LICENSE file in the topmost directory...
*/

#if defined WATER_SERVICE_UUID

static void init_water() {

    // initialize the HC-SR04
    pinMode(WATERCONTAINERLEVELTRIGGERPIN, OUTPUT);
    pinMode(WATERCONTAINERLEVELECHOPIN, INPUT);

    // initialize the two swim switches
    pinMode(WATERCONTAINERLEVELMINPIN, INPUT);
    pinMode(WATERCONTAINERLEVELMAXPIN, INPUT);

    // initialize the pump
    pinMode(WATERPUMPPIN, OUTPUT);
}

static void init_ble_water(BLEServer* ble_server) {

    BLEService *water_service = ble_server->createService(WATER_SERVICE_UUID);
    water_container_level_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_MIN_LEVEL_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    water_container_level_min_crit_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_MIN_LEVEL_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    water_container_level_min_warn_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_MIN_LEVEL_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    water_container_level_max_warn_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_MIN_LEVEL_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    water_container_level_max_crit_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_MIN_LEVEL_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    water_container_depth_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_MIN_LEVEL_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    water_container_min_level_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_MIN_LEVEL_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    water_container_max_level_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_MAX_LEVEL_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    water_pump_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_PUMP_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    water_container_level_characteristic->addDescriptor(new BLE2902());
    water_container_level_min_crit_characteristic->addDescriptor(new BLE2902());
    water_container_level_min_warn_characteristic->addDescriptor(new BLE2902());
    water_container_level_max_warn_characteristic->addDescriptor(new BLE2902());
    water_container_level_max_crit_characteristic->addDescriptor(new BLE2902());
    water_container_depth_characteristic->addDescriptor(new BLE2902());
    water_container_level_characteristic->addDescriptor(new BLE2902());
    water_container_min_level_characteristic->addDescriptor(new BLE2902());
    water_container_max_level_characteristic->addDescriptor(new BLE2902());
    water_pump_characteristic->addDescriptor(new BLE2902());
    BLE2904 *desc0 = new BLE2904();
//    desc0->setFormat(0x01);
//    desc0->setUnit(0x2701); // meter
//    BLE2904 *desc1 = new BLE2904();
//    desc1->setFormat(0x01);
//    desc1->setUnit(0x2700);
//    BLE2904 *desc2 = new BLE2904();
//    desc2->setFormat(0x01);
//    desc2->setUnit(0x2700);

    water_service->start();
}

static void pump_water() {

    if (water_flow_start > 0) {
        if (PUMP_ON_COUNT < 0) {
        // we are in continuous mode
            if (water_flow_force_stop == 0) {
                digitalWrite(WATERPUMPPIN, HIGH);
                set_ble_characteristic(water_pump_characteristic, "0");
                Serial.println("Pump is on...");
            }
        } else {
        // we are in interval mode
            for (int i = 0; i < PUMP_ON_COUNT; i++) {
                if (water_flow_force_stop == 0) {
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
    } else {
        digitalWrite(WATERPUMPPIN, LOW);
        set_ble_characteristic(water_pump_characteristic, "1");
        Serial.println("Pump is off...");
    }
}

//TODO: it is probably more a problem of the device than the view...
//TODO:
static void get_water_info() {

    //TODO: the max level is not used yet..
    if (digitalRead(WATERCONTAINERLEVELMINPIN)) {
        set_ble_characteristic(water_container_min_level_characteristic, "1");
        Serial.println("The container is full enough for the watering pump.");
        pump_water();
    } else {
        set_ble_characteristic(water_container_min_level_characteristic, "0");
        Serial.println("WARNING: Please do fill the container!");
    }
}

#endif
