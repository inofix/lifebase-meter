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
            WATER_CONTAINER_LEVEL_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    water_container_depth_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_DEPTH_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    water_container_distance_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_DISTANCE_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    water_container_level_min_crit_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_MIN_CRIT_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    water_container_level_min_warn_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_MIN_WARN_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    water_container_level_max_warn_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_MAX_WARN_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    water_container_level_max_crit_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_MAX_CRIT_UUID, BLECharacteristic::PROPERTY_READ |
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
    water_container_distance_characteristic->addDescriptor(new BLE2902());
    water_container_depth_characteristic->addDescriptor(new BLE2902());
    water_container_level_characteristic->addDescriptor(new BLE2902());
    water_container_min_level_characteristic->addDescriptor(new BLE2902());
    water_container_max_level_characteristic->addDescriptor(new BLE2902());
    water_pump_characteristic->addDescriptor(new BLE2902());
    BLE2904 *desc0 = new BLE2904();
    desc0->setFormat(0x01);
//  desc0->setFormat(BLE2904::FORMAT_UINT8); ??
    desc0->setUnit(0x2701); // meter
    water_container_level_characteristic->addDescriptor(desc0);
    BLE2904 *desc1 = new BLE2904();
    desc1->setFormat(0x01);
    desc1->setUnit(0x2701); // meter
    water_container_level_min_crit_characteristic->addDescriptor(desc1);
    BLE2904 *desc2 = new BLE2904();
    desc2->setFormat(0x01);
    desc2->setUnit(0x2701); // meter
    water_container_level_min_warn_characteristic->addDescriptor(desc2);
    BLE2904 *desc3 = new BLE2904();
    desc3->setFormat(0x01);
    desc3->setUnit(0x2701); // meter
    water_container_level_max_warn_characteristic->addDescriptor(desc3);
    BLE2904 *desc4 = new BLE2904();
    desc4->setFormat(0x01);
    desc4->setUnit(0x2701); // meter
    water_container_level_min_crit_characteristic->addDescriptor(desc4);

    water_service->start();
}

static void pump_water() {

    if (water_flow_start > 0) {
        if (PUMP_ON_COUNT <= 0) {
        // we are in continuous mode
            if (water_flow_force_stop == 0) {
                digitalWrite(WATERPUMPPIN, HIGH);
                set_ble_characteristic(water_pump_characteristic, "0");
                Serial.println("Pump is on...");
            } else {
                digitalWrite(WATERPUMPPIN, LOW);
                water_flow_start = 0;
                set_ble_characteristic(water_pump_characteristic, "1");
                Serial.println("Force stopping the pump...");
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

// critical threashholds reached..
// as measured by the HC-SR04
static bool is_crit_low = false;
// as measured by the float switch
static bool is_lower_than = false;

static void get_water_info() {

    // measure the distance to the water surface and add the depth

    // temperature is the most significant variable: https://en.wikipedia.org/wiki/Speed_of_sound
    // assumes 20°C if air_service is missing..
    loop_delay -= 100;
    digitalWrite(WATERCONTAINERLEVELTRIGGERPIN, LOW);
    delayMicroseconds(99);
    digitalWrite(WATERCONTAINERLEVELTRIGGERPIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(WATERCONTAINERLEVELTRIGGERPIN, LOW);
    // (half the way), in seconds
    float water_distance_duration = pulseIn(WATERCONTAINERLEVELECHOPIN, HIGH) / 2;
    // (we should have two sensors, one just for calibration...)
    float sound_velocity_air = 331.3 + 0.606 * air_temperature;
    // in m
    float water_distance_value = water_distance_duration * sound_velocity_air / 1000000;
    float water_depth = water_distance_value -
        strtof(water_container_distance_characteristic->getValue().c_str(), NULL) +
        strtof(water_container_depth_characteristic->getValue().c_str(), NULL);

    float crit_threshhold = strtof(
        water_container_level_min_crit_characteristic->getValue().c_str(), NULL);
    if (water_depth < crit_threshhold) {
        if (!is_crit_low) {
            water_flow_force_stop++;
            is_crit_low = true;
        }
    } else {
        if (is_crit_low) {
            water_flow_force_stop--;
            is_crit_low = false;
        }
    }

    Serial.print("Current water level is ");
    Serial.print(water_depth);
    Serial.println("m.");
    char water_depth_chars[4];
    dtostrf(water_depth, 1, 3, water_depth_chars);
    set_ble_characteristic(water_container_level_characteristic, water_depth_chars);

    // ask the float switches
    if (digitalRead(WATERCONTAINERLEVELMINPIN)) {
        if (is_lower_than) {
            water_flow_force_stop--;
            is_lower_than = false;
        }
        set_ble_characteristic(water_container_min_level_characteristic, "1");
        Serial.println("The container is full enough for the watering pump.");
    } else {
        if (!is_lower_than) {
            water_flow_force_stop++;
            is_lower_than = true;
        }
        set_ble_characteristic(water_container_min_level_characteristic, "0");
        Serial.println("WARNING: please do fill the container!");
    }
    if (digitalRead(WATERCONTAINERLEVELMAXPIN)) {
        set_ble_characteristic(water_container_max_level_characteristic, "1");
        Serial.println("WARNING: the maximum level of the container is reached!");
    } else {
        set_ble_characteristic(water_container_min_level_characteristic, "0");
    }
    pump_water();
}

#endif
