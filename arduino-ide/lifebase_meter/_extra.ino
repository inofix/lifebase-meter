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

#if defined EXTRA_SERVICE_UUID

bool is_leaked = false;

void IRAM_ATTR leak_isr() {
    // increase the threashold: make sure it was no false alarm
    delay(2);
    if (!digitalRead(EXTRA_LEAK_PIN)) {
        if (! is_leaked) {
            Serial.println("INTERRUPT: leak detected!");
            is_leaked = true;
            water_flow_force_stop++;
        }
    }
}

static void init_extra() {

    pinMode(EXTRA_LEAK_PIN, INPUT_PULLUP);
    attachInterrupt(EXTRA_LEAK_PIN, leak_isr, FALLING);
}

  #if defined BLE
static void init_ble_extra(BLEServer* ble_server) {

    BLEService *extra_service = ble_server->createService(EXTRA_SERVICE_UUID);
    extra_leak_characteristic = extra_service->createCharacteristic(
            EXTRA_LEAK_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    extra_leak_characteristic->setValue("0");
    extra_leak_characteristic->addDescriptor(new BLE2902());
    extra_service->start();
}
  #endif

static void get_extra_info() {

    if (digitalRead(EXTRA_LEAK_PIN)) {
        if (is_leaked) {
            is_leaked = false;
            water_flow_force_stop--;
        }
        Serial.println("There was no water detected outside the system!");
  #if defined BLE
        set_ble_characteristic(extra_leak_characteristic, "0");
  #endif
  #if defined WIFI
        mqtt_publish(EXTRA_SERVICE_UUID, EXTRA_LEAK_UUID, "0");
  #endif
    } else {
        if (!is_leaked) {
            is_leaked = true;
            water_flow_force_stop++;
        }
        Serial.println("Warning: There was water detected outside the system!");
  #if defined BLE
        set_ble_characteristic(extra_leak_characteristic, "1");
  #endif
  #if defined WIFI
        mqtt_publish(EXTRA_SERVICE_UUID, EXTRA_LEAK_UUID, "1");
  #endif
    }
}

#endif
