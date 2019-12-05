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

#if defined SOIL_SERVICE_UUID

static void init_soil() {

    pinMode(SOILMOISTUREPIN, INPUT);
}

static void init_ble_soil(BLEServer* ble_server) {

    BLEService *soil_service = ble_server->createService(SOIL_SERVICE_UUID);
    soil_moisture_characteristic = soil_service->createCharacteristic(
            SOIL_MOISTURE_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    soil_moisture_min_crit_characteristic = soil_service->createCharacteristic(
            SOIL_MOISTURE_MIN_CRIT_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    soil_moisture_min_warn_characteristic = soil_service->createCharacteristic(
            SOIL_MOISTURE_MIN_WARN_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    soil_moisture_max_warn_characteristic = soil_service->createCharacteristic(
            SOIL_MOISTURE_MAX_WARN_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    soil_moisture_max_crit_characteristic = soil_service->createCharacteristic(
            SOIL_MOISTURE_MAX_CRIT_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    char chars[3];
    dtostrf(SOIL_MOISTURE_MIN_CRIT_INIT, 3, 0, chars);
    soil_moisture_min_crit_characteristic->setValue(chars);
    dtostrf(SOIL_MOISTURE_MIN_WARN_INIT, 3, 0, chars);
    soil_moisture_min_warn_characteristic->setValue(chars);
    dtostrf(SOIL_MOISTURE_MAX_WARN_INIT, 3, 0, chars);
    soil_moisture_max_warn_characteristic->setValue(chars);
    dtostrf(SOIL_MOISTURE_MAX_CRIT_INIT, 3, 0, chars);
    soil_moisture_max_crit_characteristic->setValue(chars);
    soil_moisture_characteristic->addDescriptor(new BLE2902());
    soil_moisture_min_crit_characteristic->addDescriptor(new BLE2902());
    soil_moisture_min_warn_characteristic->addDescriptor(new BLE2902());
    soil_moisture_max_warn_characteristic->addDescriptor(new BLE2902());
    soil_moisture_max_crit_characteristic->addDescriptor(new BLE2902());
    soil_service->start();
}

static bool is_too_wet = false;
static bool is_too_dry = false;

static int read_soil_moisture_percentage(int pin, int abs_min, int abs_max,
        int min_crit, int min_warn, int max_warn, int max_crit) {

    Serial.print("Current soil moisture reported is ");
    int i = (100 - (analogRead(pin) - abs_max) * 100 / (abs_min - abs_max));
    Serial.print(i);

    if (0 == min_crit && 0 == min_warn && max_warn == 100 && max_crit == 100) {
        Serial.println("% (watering service is disabled).");
//        Serial.print("  min crit: ");
//        Serial.print(min_crit);
//        Serial.print("; min warn: ");
//        Serial.print(min_warn);
//        Serial.print("; max warn: ");
//        Serial.print(max_warn);
//        Serial.print("; max crit: ");
//        Serial.print(max_crit);
//        Serial.println(";");
    // we have 0 == min-crit == min-warn && 100 == max-crit == max-warn
    // already tested above..
    } else if (0 <= min_crit && min_crit <= min_warn &&
            max_warn <= max_crit && max_crit <= 100) {
        Serial.println("% (watering service is enabled).");
//        Serial.print("  min crit: ");
//        Serial.print(min_crit);
//        Serial.print("; min warn: ");
//        Serial.print(min_warn);
//        Serial.print("; max warn: ");
//        Serial.print(max_warn);
//        Serial.print("; max crit: ");
//        Serial.print(max_crit);
//        Serial.println(";");

        Serial.print("  This is actually ");
        if (i < min_warn) {
            Serial.println("too dry!");
            if (!is_too_dry) {
                water_flow_start++;
                is_too_dry = true;
            }
        } else if (max_warn < i) {
            Serial.println("too wet!");
            if (is_too_dry) {
                water_flow_start--;
                is_too_dry = false;
            }
        } else {
            Serial.println("just fine.");
        }
        if (i < max_crit) {
            if (is_too_wet) {
                water_flow_force_stop--;
                is_too_wet = false;
            }
        } else {
            if (!is_too_wet) {
                water_flow_force_stop++;
                is_too_wet = true;
            }
        }
    } else {
        Serial.println("%. WARNING: moisture min/max settings do not make sense!");
        Serial.print("  min crit: ");
        Serial.print(min_crit);
        Serial.print("; min warn: ");
        Serial.print(min_warn);
        Serial.print("; max warn: ");
        Serial.print(max_warn);
        Serial.print("; max crit: ");
        Serial.print(max_crit);
        Serial.println(";");
        // in this case we do not want to be responsible for using the pump..
        if (is_too_dry) {
            water_flow_start--;
            is_too_dry = false;
        }
    }

    return i;
}

static void get_soil_info() {

    int soil_moisture_min_crit = strtol(
            soil_moisture_min_crit_characteristic->getValue().c_str(), NULL, 10);
    int soil_moisture_min_warn = strtol(
            soil_moisture_min_warn_characteristic->getValue().c_str(), NULL, 10);
    int soil_moisture_max_warn = strtol(
            soil_moisture_max_warn_characteristic->getValue().c_str(), NULL, 10);
    int soil_moisture_max_crit = strtol(
            soil_moisture_max_crit_characteristic->getValue().c_str(), NULL, 10);

    int soil_moisture = read_soil_moisture_percentage(SOILMOISTUREPIN,
        SOIL_MOISTURE_ABSOLUTE_MIN, SOIL_MOISTURE_ABSOLUTE_MAX,
        soil_moisture_min_crit, soil_moisture_min_warn,
        soil_moisture_max_warn, soil_moisture_max_crit);
    char soil_moisture_chars[3];
    dtostrf(soil_moisture, 3, 0, soil_moisture_chars);
    set_ble_characteristic(soil_moisture_characteristic, soil_moisture_chars);
}

#endif
