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

// critical threashholds reached..
// as measured by the HC-SR04
static bool is_crit_low = false;
// as measured by the float switch
static bool is_lower_than = false;

void IRAM_ATTR switch_isr() {
    if (! is_lower_than) {
        Serial.println("INTERRUPT: critical water level!");
//TODO add a condensator..
//        is_lower_than = true;
//        water_flow_force_stop++;
    }
}

static void init_water() {

    // initialize the HC-SR04
    pinMode(WATER_CONTAINER_LEVEL_TRIGGER_PIN, OUTPUT);
    pinMode(WATER_CONTAINER_LEVEL_ECHO_PIN, INPUT);

    // initialize the two swim switches
    pinMode(WATER_CONTAINER_LEVEL_MIN_PIN, INPUT_PULLUP);
    attachInterrupt(WATER_CONTAINER_LEVEL_MIN_PIN, switch_isr, HIGH);
// disabled for now..
//    pinMode(WATER_CONTAINER_LEVEL_MAX_PIN, INPUT_PULLUP);

    // initialize the pump
    pinMode(WATER_PUMP_PIN, OUTPUT);
}

static void init_ble_water(BLEServer* ble_server) {

    BLEService *water_service = ble_server->createService(BLEUUID(WATER_SERVICE_UUID), 38, 0);
    water_container_distance_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_DISTANCE_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    water_container_level_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_LEVEL_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    water_container_depth_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_DEPTH_UUID, BLECharacteristic::PROPERTY_READ |
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
    water_container_pump_characteristic = water_service->createCharacteristic(
            WATER_CONTAINER_PUMP_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    water_container_distance_characteristic->addDescriptor(new BLE2902());
    water_container_level_characteristic->addDescriptor(new BLE2902());
    water_container_depth_characteristic->addDescriptor(new BLE2902());
    water_container_level_min_crit_characteristic->addDescriptor(new BLE2902());
    water_container_level_min_warn_characteristic->addDescriptor(new BLE2902());
    water_container_level_max_warn_characteristic->addDescriptor(new BLE2902());
    water_container_level_max_crit_characteristic->addDescriptor(new BLE2902());
    water_container_min_level_characteristic->addDescriptor(new BLE2902());
    water_container_max_level_characteristic->addDescriptor(new BLE2902());
    water_container_pump_characteristic->addDescriptor(new BLE2902());
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
    water_container_level_max_crit_characteristic->addDescriptor(desc4);
    BLE2904 *desc5 = new BLE2904();
    desc5->setFormat(0x01);
    desc5->setUnit(0x2701); // meter
    water_container_distance_characteristic->addDescriptor(desc5);
    BLE2904 *desc6 = new BLE2904();
    desc6->setFormat(0x01);
    desc6->setUnit(0x2701); // meter
    water_container_depth_characteristic->addDescriptor(desc6);
    char chars[4];
    dtostrf(0, 3, 0, chars);
    water_container_level_characteristic->setValue(chars);
    dtostrf(0, 3, 0, chars);
    water_container_distance_characteristic->setValue(chars);
    dtostrf(0, 3, 0, chars);
    water_container_depth_characteristic->setValue(chars);
    dtostrf(0, 3, 0, chars);
    water_container_level_min_crit_characteristic->setValue(chars);
    dtostrf(0, 3, 0, chars);
    water_container_level_min_warn_characteristic->setValue(chars);
    dtostrf(0, 3, 0, chars);
    water_container_level_max_warn_characteristic->setValue(chars);
    dtostrf(0, 3, 0, chars);
    water_container_level_max_crit_characteristic->setValue(chars);
    dtostrf(0, 3, 0, chars);
    water_container_min_level_characteristic->setValue(chars);
    dtostrf(0, 3, 0, chars);
    water_container_max_level_characteristic->setValue(chars);
    dtostrf(0, 3, 0, chars);
    water_container_pump_characteristic->setValue(chars);

    water_service->start();
}

void watering_loop(void* parameters) {
    for (;;) {
//        Serial.print("Watering action task is running on core ");
//        Serial.print(xPortGetCoreID());
//        Serial.println(".");
        pump_water(WATER_PUMP_PIN, water_container_pump_characteristic, PUMP_LOOP_DELAY);
    }
}

static void pump_water(int pin, BLECharacteristic* characteristic, int loop_delay) {

//TODO allow relay control to be inverted.
// I have a relay here which acts on NO - LOW,
// all the others switch on HIGH at NO..

    if (PUMP_MODE <= 0) {
    // we are in continuous mode, this means ignoring external watering requests
        if (water_flow_force_stop == 0) {
            digitalWrite(pin, LOW);
            set_ble_characteristic(characteristic, "1");
            mqtt_publish(WATER_SERVICE_UUID, WATER_CONTAINER_PUMP_UUID, "1");
            Serial.println("Pumping...");
        } else {
            digitalWrite(pin, HIGH);
            set_ble_characteristic(characteristic, "0");
            mqtt_publish(WATER_SERVICE_UUID, WATER_CONTAINER_PUMP_UUID, "0");
            Serial.println("Force stopping the pump...");
        }
        delay(loop_delay);
    } else if (PUMP_MODE == 1) {
    // we are in interval mode, pumping on request only (e.g. soil was too dry..)
        if (water_flow_start > 0) {
            if (water_flow_force_stop == 0) {
                digitalWrite(pin, LOW);
                set_ble_characteristic(characteristic, "1");
                mqtt_publish(WATER_SERVICE_UUID, WATER_CONTAINER_PUMP_UUID, "1");
                Serial.println("Starting the pump...");
            }
            delay(loop_delay / 2);
            digitalWrite(pin, HIGH);
            set_ble_characteristic(characteristic, "0");
            mqtt_publish(WATER_SERVICE_UUID, WATER_CONTAINER_PUMP_UUID, "0");
            Serial.println("Stopping the pump...");
            delay(loop_delay / 2);
        }
    } else {
    // pump is always off in this mode..
        digitalWrite(pin, HIGH);
        set_ble_characteristic(characteristic, "0");
        mqtt_publish(WATER_SERVICE_UUID, WATER_CONTAINER_PUMP_UUID, "0");
//        Serial.println("Turning off the pump...");
        delay(loop_delay);
    }
}

static void get_water_info() {

    // measure the distance to the water surface and add the depth

    // temperature is the most significant variable: https://en.wikipedia.org/wiki/Speed_of_sound
    // assumes 20°C if air_service is missing..
    main_loop_delay -= 100;
    digitalWrite(WATER_CONTAINER_LEVEL_TRIGGER_PIN, LOW);
    delayMicroseconds(99);
    digitalWrite(WATER_CONTAINER_LEVEL_TRIGGER_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(WATER_CONTAINER_LEVEL_TRIGGER_PIN, LOW);
    // (half the way), in seconds
    float water_distance_duration = pulseIn(WATER_CONTAINER_LEVEL_ECHO_PIN, HIGH) / 2;
    // (we should have two sensors, one just for calibration...)
    float sound_velocity_air = 331.3 + 0.606 * air_temperature;
    // in m
    float water_distance_value = water_distance_duration * sound_velocity_air / 1000000;

    if (water_distance_value > 0) {
        float water_depth = strtof(water_container_depth_characteristic->getValue().c_str(), NULL) - water_distance_value;

        float crit_threshhold = strtof(
            water_container_level_min_crit_characteristic->getValue().c_str(), NULL);
//disabled for now..
//    if (water_depth < crit_threshhold) {
//        if (!is_crit_low) {
//            water_flow_force_stop++;
//            is_crit_low = true;
//        }
//    } else {
//        if (is_crit_low) {
//            water_flow_force_stop--;
//            is_crit_low = false;
//        }
//    }

        Serial.print("Current water level is ");
        Serial.print(water_depth);
        Serial.print("m; Sensor-distance to surface is ");
        Serial.print(water_distance_value);
        Serial.println("m.");
        char water_dist_chars[7];
        dtostrf(water_distance_value, 6, 2, water_dist_chars);
        mqtt_publish(WATER_SERVICE_UUID, WATER_CONTAINER_DISTANCE_UUID, water_dist_chars);
        set_ble_characteristic(water_container_distance_characteristic, water_dist_chars);
        char water_depth_chars[7];
        dtostrf(water_depth, 6, 2, water_depth_chars);
        mqtt_publish(WATER_SERVICE_UUID, WATER_CONTAINER_LEVEL_UUID, water_depth_chars);
        set_ble_characteristic(water_container_level_characteristic, water_depth_chars);
    } else {
        Serial.println("Error reading the water level distance.");
    }

    // ask the float switches
    if (digitalRead(WATER_CONTAINER_LEVEL_MIN_PIN)) {
        if (is_lower_than) {
            is_lower_than = false;
            water_flow_force_stop--;
        }
        set_ble_characteristic(water_container_min_level_characteristic, "1");
        mqtt_publish(WATER_SERVICE_UUID, WATER_CONTAINER_MIN_LEVEL_UUID, "1");
        Serial.println("The container is full enough for the watering pump.");
    } else {
        if (!is_lower_than) {
            is_lower_than = true;
            water_flow_force_stop++;
        }
        set_ble_characteristic(water_container_min_level_characteristic, "0");
        mqtt_publish(WATER_SERVICE_UUID, WATER_CONTAINER_MIN_LEVEL_UUID, "0");
        Serial.println("WARNING: please do fill the container!");
    }
// currently disabled
//    if (digitalRead(WATER_CONTAINER_LEVEL_MAX_PIN)) {
//        set_ble_characteristic(water_container_max_level_characteristic, "1");
//        Serial.println("WARNING: the maximum level of the container is reached!");
//    } else {
//        set_ble_characteristic(water_container_min_level_characteristic, "0");
//    }
}

#endif
