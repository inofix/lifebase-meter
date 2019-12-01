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

// The sensor used here is a 'Capacitive Soil Moisture Sensor v1.2' from
// diymore.cc. We protected it against the permanent exposure in a
// potentially wet and unfriendly environment with lacquer (electronics
// and PCB edges..).
// In the reference implementation with a very loose and humus-rich soil
// we found the following values:
//   - in water: 31488 (100%)
//   - after watering: 47133 (38%)
//   - 'quite dry' soil: 52944 (14%)
//   - in complete isolation: 56368 (0%)
//TODO: verify or specify for differnt soil types..

#define SOIL_MOISTURE_ABSOLUTE_MIN 56399
#define SOIL_MOISTURE_ABSOLUTE_MAX 31488

// MIN/MAX moisture depend on the plant and soil and we might even want
// to set it dynamically on the device and store it.
//TODO: store it
//TODO: authentication needed
//..
// default minimum of soil moisture (or when to water / warn / etc.)
int soil_moisture_min_crit = 0;
char soil_moisture_min_crit_chars[3];
int soil_moisture_min_warn = 0;
char soil_moisture_min_warn_chars[3];
// default maximum of soil moisture (or when to stop watering or open valve
// / warn / etc.)
int soil_moisture_max_warn = 100;
char soil_moisture_max_warn_chars[3];
int soil_moisture_max_crit = 100;
char soil_moisture_max_crit_chars[3];

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
    dtostrf(soil_moisture_min_crit, 3, 0, soil_moisture_min_crit_chars);
    soil_moisture_min_crit_characteristic->setValue(soil_moisture_min_crit_chars);
    dtostrf(soil_moisture_min_warn, 3, 0, soil_moisture_min_warn_chars);
    soil_moisture_min_warn_characteristic->setValue(soil_moisture_min_warn_chars);
    dtostrf(soil_moisture_max_warn, 3, 0, soil_moisture_max_warn_chars);
    soil_moisture_max_warn_characteristic->setValue(soil_moisture_max_warn_chars);
    dtostrf(soil_moisture_max_crit, 3, 0, soil_moisture_max_crit_chars);
    soil_moisture_max_crit_characteristic->setValue(soil_moisture_max_crit_chars);
    soil_moisture_characteristic->addDescriptor(new BLE2902());
    soil_moisture_min_crit_characteristic->addDescriptor(new BLE2902());
    soil_moisture_min_warn_characteristic->addDescriptor(new BLE2902());
    soil_moisture_max_warn_characteristic->addDescriptor(new BLE2902());
    soil_moisture_max_crit_characteristic->addDescriptor(new BLE2902());
    soil_service->start();
}

bool is_too_wet = false;
bool is_too_dry = false;

static int read_soil_moisture_percentage(int PIN) {

    int i = (100 - (analogRead(SOILMOISTUREPIN) - SOIL_MOISTURE_ABSOLUTE_MAX) *
            100 / (SOIL_MOISTURE_ABSOLUTE_MIN - SOIL_MOISTURE_ABSOLUTE_MAX));

    soil_moisture_min_warn = strtol(
            soil_moisture_min_warn_characteristic->getValue().c_str(), NULL, 10);
    soil_moisture_max_warn = strtol(
            soil_moisture_max_characteristic->getValue().c_str(), NULL, 10);

    if (0 < soil_moisture_min_warn < soil_moisture_max_warn < 100) {
        if (i < soil_moisture_min_warn) {
            if (!is_too_dry) {
                water_flow_start++;
                is_too_dry = true;
            }
            if (is_too_wet) {
                water_flow_force_stop--;
                is_too_wet = false;
            }
        } else if (i > soil_moisture_max_warn) {
            if (!is_too_wet) {
                water_flow_force_stop++;
                is_too_wet = true;
            }
            if (is_too_dry) {
                water_flow_start--;
                is_too_dry = false;
            }
        } else {
            if (is_too_wet) {
                water_flow_force_stop--;
                is_too_wet = false;
            }
            if (is_too_dry) {
                water_flow_start--;
                is_too_dry = false;
            }
        }
    } else {
        set_ble_characteristic(soil_moisture_min_crit_characteristic, "0");
        set_ble_characteristic(soil_moisture_min_warn_characteristic, "0");
        set_ble_characteristic(soil_moisture_max_warn_characteristic, "100");
        set_ble_characteristic(soil_moisture_max_crit_characteristic, "100");
    }
    return i;
}

static void get_soil_info() {

    int soil_moisture = read_soil_moisture_percentage(SOILMOISTUREPIN);
    char soil_moisture_chars[3];
    dtostrf(soil_moisture, 3, 0, soil_moisture_chars);
    Serial.print("Current soil moisture reported is ");
    Serial.print(soil_moisture);
    Serial.print("%. This is actually ");
    if (is_too_dry) {
        Serial.println("too dry!");
    } else if (is_too_wet) {
        Serial.println("too wet!");
    } else {
        Serial.println("just fine.");
    }
    set_ble_characteristic(soil_moisture_characteristic, soil_moisture_chars);
}

#endif
