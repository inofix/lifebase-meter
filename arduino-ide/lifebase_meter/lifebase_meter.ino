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

// system constants
#define LB_TAG "{{ LB_TAG }}"

// define the time to idle between measurements
// note: the DHT does not deliver new results faster than every 2s
#define MAIN_LOOP_DELAY {{ MAIN_LOOP_DELAY }}
int main_loop_delay;

// globally set the analog resolution to 16bit even if the
// hardware does not support it as to have the same thresholds
// on all boards..
#define ANALOG_RESOLUTION 16

// subject service
/// these are the UUID to distinguish the different elements/characteristics
/// they stay the same for all installations - think of it as variable-names
//// the UUID of the service
#define SUBJECT_SERVICE_UUID "{{ SUBJECT_SERVICE_UUID }}"
//// a custom name for the whole setup
#define SUBJECT_NAME_UUID "{{ SUBJECT_NAME_UUID }}"
//// a custom UUID for the whole setup
#define SUBJECT_UUID_UUID "{{ SUBJECT_UUID_UUID }}"
//// a type name for the whole setup
#define SUBJECT_TYPE_NAME_UUID "{{ SUBJECT_TYPE_NAME_UUID }}"
//// a type uuid for the whole setup
#define SUBJECT_TYPE_UUID_UUID "{{ SUBJECT_TYPE_UUID_UUID }}"
//// a status led for the whole setup
#define SUBJECT_LED_HEALTH_UUID "{{ SUBJECT_LED_HEALTH_UUID }}"
//// a warn led if something is wrong
#define SUBJECT_LED_WARN_UUID "{{ SUBJECT_LED_WARN_UUID }}"
#define SUBJECT_LED_IDENTIFY_UUID "{{ SUBJECT_LED_IDENTIFY_UUID }}"
/// these are the constants for the conditions and used e.g. for the status led
#define SUBJECT_HEALTH_GOOD "0"
#define SUBJECT_HEALTH_BAD "1"
#define SUBJECT_HEALTH_CRITICAL "2"
#define SUBJECT_HEALTH_DEAD "3"

/// system constants per system/setup
/// #change# These UUIDs should differ from setup to setup
#define SUBJECT_NAME "{{ SUBJECT_NAME }}"
#define SUBJECT_UUID "{{ SUBJECT_UUID }}"
#define SUBJECT_TYPE_NAME "{{ SUBJECT_TYPE_NAME }}"
#define SUBJECT_TYPE_UUID "{{ SUBJECT_TYPE_UUID }}"
/// pin config
#define SUBJECT_LED_RED_PIN 14
#define SUBJECT_LED_GREEN_PIN 27
#define SUBJECT_LED_BLUE_PIN 12
#define SUBJECT_LED_CHANNEL_RED 0
#define SUBJECT_LED_CHANNEL_GREEN 2
#define SUBJECT_LED_CHANNEL_BLUE 4
/// LED config
#define SUBJECT_LED_FREQUENCY 5000
#define SUBJECT_LED_RESOLUTION 8
/// the time a status loop takes; this is fixed as it shares the resources of
/// one cpu with the watering loop (and is seperate from the measurements)
#define SUBJECT_STATUS_LOOP 8000
TaskHandle_t StatusTask;

// measurements/action - #change# uncoment service UUIDs as needed - or use the script..
// light service configuration
//#define LIGHT_SERVICE_UUID "{{ LIGHT_SERVICE_UUID }}"
#if defined LIGHT_SERVICE_UUID
/// light sensor includes
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
//#include <Adafruit_TSL2591_U.h>
#define LIGHT_EXPOSURE_UUID "{{ LIGHT_EXPOSURE_UUID }}"
#define LIGHT_EXPOSURE_I2C_UID 1
#define LIGHT_EXPOSURE_SENSOR {{ LIGHT_EXPOSURE_SENSOR }}
#endif

// air service configuration
//#define AIR_SERVICE_UUID "{{ AIR_SERVICE_UUID }}"
#if defined AIR_SERVICE_UUID
#define AIR_TEMPERATURE_UUID "{{ AIR_TEMPERATURE_UUID }}"
#define AIR_HUMIDITY_UUID "{{ AIR_HUMIDITY_UUID }}"
/// air sensor includes
#include <DHT_U.h>
/// air sensor constants
#define DHTPIN 13
#define DHTTYPE DHT22
DHT_Unified dht(DHTPIN, DHTTYPE);
#endif

// water service configuration
//#define WATER_SERVICE_UUID "{{ WATER_SERVICE_UUID }}"
#if defined WATER_SERVICE_UUID
/// the ultrasonic sensor HC-SR04 measuremnt - distance sensor to surface
#define WATER_CONTAINER_DISTANCE_UUID "{{ WATER_CONTAINER_DISTANCE_UUID }}"
/// the calculated water level (depth) - distance surface to ground
#define WATER_CONTAINER_LEVEL_UUID "{{ WATER_CONTAINER_LEVEL_UUID }}"
/// the maximum possible depth - distance sensor to ground
#define WATER_CONTAINER_DEPTH_UUID "{{ WATER_CONTAINER_DEPTH_UUID }}"
/// min/max sensors - swimming sensors on/off
#define WATER_CONTAINER_MIN_LEVEL_UUID "{{ WATER_CONTAINER_MIN_LEVEL_UUID }}"
#define WATER_CONTAINER_MAX_LEVEL_UUID "{{ WATER_CONTAINER_MAX_LEVEL_UUID }}"
/// actuator - the pump
#define WATER_CONTAINER_PUMP_UUID "{{ WATER_CONTAINER_PUMP_UUID }}"
/// thresholds to be set by the user
#define WATER_CONTAINER_MIN_CRIT_UUID "{{ WATER_CONTAINER_MIN_CRIT_UUID }}"
#define WATER_CONTAINER_MIN_WARN_UUID "{{ WATER_CONTAINER_MIN_WARN_UUID }}"
#define WATER_CONTAINER_MAX_WARN_UUID "{{ WATER_CONTAINER_MAX_WARN_UUID }}"
#define WATER_CONTAINER_MAX_CRIT_UUID "{{ WATER_CONTAINER_MAX_CRIT_UUID }}"

/// pin config
//// water container level: HC-SR04
#define WATERCONTAINERLEVELTRIGGERPIN 26
#define WATERCONTAINERLEVELECHOPIN 25
//// water container swimming on/off sensors
#define WATERCONTAINERLEVELMINPIN 4
#define WATERCONTAINERLEVELMAXPIN 15
//// water pump pin
#define WATERPUMPPIN 2
// switch between continuous and interval mode
#define PUMP_MODE {{ PUMP_MODE }}
// how long should the pump run and pause each time? (1/2 loop length each)
#define PUMP_LOOP_DELAY {{ PUMP_LOOP_DELAY }}
TaskHandle_t WateringTask;
#endif

// soil service configuration
//#define SOIL_SERVICE_UUID "{{ SOIL_SERVICE_UUID }}"
#if defined SOIL_SERVICE_UUID
#define SOIL_MOISTURE_UUID "{{ SOIL_MOISTURE_UUID }}"
#define SOILMOISTUREPIN 32
#define SOIL_MOISTURE_MIN_CRIT_UUID "{{ SOIL_MOISTURE_MIN_CRIT_UUID }}"
#define SOIL_MOISTURE_MIN_WARN_UUID "{{ SOIL_MOISTURE_MIN_WARN_UUID }}"
#define SOIL_MOISTURE_MAX_WARN_UUID "{{ SOIL_MOISTURE_MAX_WARN_UUID }}"
#define SOIL_MOISTURE_MAX_CRIT_UUID "{{ SOIL_MOISTURE_MAX_CRIT_UUID }}"
// MIN/MAX moisture depend on the plant and soil and we might even want
// to set it dynamically on the device and store it.
//TODO: Edit from the config script..
// default minimum of soil moisture (or when to water / warn / etc.)
#define SOIL_MOISTURE_MIN_CRIT_INIT 0
#define SOIL_MOISTURE_MIN_WARN_INIT 0
// default maximum of soil moisture (or when to stop watering or open valve
// / warn / etc.)
#define SOIL_MOISTURE_MAX_WARN_INIT 100
#define SOIL_MOISTURE_MAX_CRIT_INIT 100
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
//TODO: Edit from the config script..
#define SOIL_MOISTURE_ABSOLUTE_MIN 56399
#define SOIL_MOISTURE_ABSOLUTE_MAX 31488
// see _water.ino for the concrete MAX/MIN values..
#endif

// extra service configuration
//#define EXTRA_SERVICE_UUID "{{ EXTRA_SERVICE_UUID }}"
#if defined EXTRA_SERVICE_UUID
#define EXTRA_LEAK_UUID "{{ EXTRA_LEAK_UUID }}"
#define EXTRA_LEAK_PIN 33
#endif

// WiFi includes
#include <WiFi.h>

// WiFi-MQTT includes
#include <PubSubClient.h>

// BLE includes
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <BLE2904.h>

#define WIFI_DEFAULT_SSID "{{ WIFI_DEFAULT_SSID }}"
#define WIFI_DEFAULT_PASSWORD "{{ WIFI_DEFAULT_PASSWORD }}"

#define WIFI_MQTT_DEFAULT_BROKER "{{ WIFI_MQTT_DEFAULT_BROKER }}"
#define WIFI_MQTT_DEFAULT_PORT {{ WIFI_MQTT_DEFAULT_PORT }}
#define WIFI_MQTT_DEFAULT_NAMESPACE "{{ WIFI_MQTT_DEFAULT_NAMESPACE }}"
#define WIFI_MQTT_DEFAULT_USER "{{ WIFI_MQTT_DEFAULT_USER }}"
#define WIFI_MQTT_DEFAULT_PASSWORD "{{ WIFI_MQTT_DEFAULT_PASSWORD }}"

// WiFi variables
//TODO Make this configurable over BLE (and store it over reboot)
//TODO We need BLE/WiFi enable/disable
//TODO We need a service for the connections that logs the connections too
//TODO Use #define to set an inital/default ssid etc.

//TODO Use WiFiClientSecure instead for SSL/TLS connections
WiFiClient wifi_client;

bool wifi_is_connected = false;

// MQTT variables
char* mqtt_broker = WIFI_MQTT_DEFAULT_BROKER;
int mqtt_port = WIFI_MQTT_DEFAULT_PORT;
char* mqtt_namespace = WIFI_MQTT_DEFAULT_NAMESPACE;
//TODO implement cert based auth too
char* mqtt_user = WIFI_MQTT_DEFAULT_USER;
char* mqtt_password = WIFI_MQTT_DEFAULT_PASSWORD;

PubSubClient mqtt_client(wifi_client);

// BLE variables
BLEServer* ble_server = NULL;
BLECharacteristic* subject_uuid_characteristic = NULL;
BLECharacteristic* subject_name_characteristic = NULL;
BLECharacteristic* subject_type_characteristic = NULL;
BLECharacteristic* subject_type_id_characteristic = NULL;
BLECharacteristic* subject_warn_characteristic = NULL;
BLECharacteristic* subject_health_characteristic = NULL;
BLECharacteristic* subject_identify_characteristic = NULL;
#if defined LIGHT_EXPOSURE_UUID
BLECharacteristic* light_exposure_characteristic = NULL;
#endif
#if defined AIR_SERVICE_UUID
BLECharacteristic* air_temperature_characteristic = NULL;
BLECharacteristic* air_humidity_characteristic = NULL;
#endif
#if defined WATER_SERVICE_UUID
BLECharacteristic* water_container_level_characteristic = NULL;
BLECharacteristic* water_container_depth_characteristic = NULL;
BLECharacteristic* water_container_distance_characteristic = NULL;
BLECharacteristic* water_container_level_min_crit_characteristic = NULL;
BLECharacteristic* water_container_level_min_warn_characteristic = NULL;
BLECharacteristic* water_container_level_max_warn_characteristic = NULL;
BLECharacteristic* water_container_level_max_crit_characteristic = NULL;
BLECharacteristic* water_container_min_level_characteristic = NULL;
BLECharacteristic* water_container_max_level_characteristic = NULL;
BLECharacteristic* water_container_pump_characteristic = NULL;
#endif
#if defined SOIL_SERVICE_UUID
BLECharacteristic* soil_moisture_characteristic = NULL;
BLECharacteristic* soil_moisture_min_crit_characteristic = NULL;
BLECharacteristic* soil_moisture_min_warn_characteristic = NULL;
BLECharacteristic* soil_moisture_max_warn_characteristic = NULL;
BLECharacteristic* soil_moisture_max_crit_characteristic = NULL;
#endif
#if defined EXTRA_SERVICE_UUID
BLECharacteristic* extra_leak_characteristic = NULL;
#endif
bool device_connected = false;
bool old_device_connected = false;
uint32_t value = 0;

// globally accessible air temperature (used in water distance..)
// set default to 20°C in case the air service is not available
float air_temperature = 20;

// used by both soil and water service together to control
// pumps/valves
int water_flow_force_stop = 0;
int water_flow_start = 0;

// Status of the environement
const char* health = SUBJECT_HEALTH_GOOD;

static void init_sensors() {

    Serial.print("Initializing the sensors.. ");
    initArduino();

#if defined LIGHT_SERVICE_UUID
    init_light();
#endif
#if defined AIR_SERVICE_UUID
    init_air();
#endif
#if defined WATER_SERVICE_UUID
    init_water();
#endif
#if defined SOIL_SERVICE_UUID
    init_soil();
#endif
#if defined EXTRA_SERVICE_UUID
    init_extra();
#endif
    Serial.println("done.");
}

class LBMServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* ble_server) {
      device_connected = true;
    };

    void onDisconnect(BLEServer* ble_server) {
      device_connected = false;
    }
};

static bool check_wifi_status() {

    // Just enough time to travel to the moon, or get a connection to the AP..
    delay(2000);

    if (WiFi.status() == WL_CONNECTED) {

        Serial.println("");
        Serial.println("WiFi is connected!");
        Serial.print("The IP address is: ");
        Serial.print(WiFi.localIP());
        return true;
    } else {
        Serial.println("The WiFi connection is not ready (yet).");
        return false;
    }
}

static bool change_wifi(char* ssid, char* password) {

    if (WiFi.status() == WL_CONNECTED) {

        WiFi.disconnect();
    }

    Serial.print("Connecting '");
    Serial.print(WiFi.getHostname());
    Serial.print("' to the WiFi network '");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    return check_wifi_status();
}

static bool init_wifi() {

    if (WiFi.status() == WL_CONNECTED) {

        return true;
    }

    Serial.print("Starting WiFi on '");
    // Calling begin() twice, first as a workaround for #2501
    // and to get access to SSID
    WiFi.begin();
    delay(500);

    WiFi.setHostname(LB_TAG);

    Serial.print(WiFi.getHostname());
    Serial.println("'...");

    // See if we already have a network defined, if not using default
    if (WiFi.SSID() != "") {

        Serial.print("Trying to connect to network: '");
        Serial.print(WiFi.SSID());
        Serial.println("'...");
        WiFi.begin();
    } else {
        const char* ssid = WIFI_DEFAULT_SSID;
        const char* password = WIFI_DEFAULT_PASSWORD;
        if (ssid != "") {
            Serial.print("Falling back to default network: '");
            Serial.print(ssid);
            Serial.println("'...");
            WiFi.begin(ssid, password);
        }
    }

    return check_wifi_status();
}

//TODO - just for testing yet...
static bool init_mqtt() {

    if (mqtt_broker == "") {
        return false;
    }

    if (WiFi.status() != WL_CONNECTED) {

        if (!init_wifi()) {
            return false;
        }
    }

    mqtt_client.setServer(mqtt_broker, mqtt_port);
    Serial.print("Connecting to MQTT '");
    Serial.print(mqtt_user);
    Serial.print("@");
    Serial.print(mqtt_broker);
    Serial.print(":");
    Serial.print(mqtt_port);
    Serial.print("': ");

    delay(500);

    if (mqtt_client.connect(LB_TAG, mqtt_user, mqtt_password)) {

        Serial.println(" success!");
        return true;
    } else {

        Serial.println(" failed...");
        return false;
    }
}

static void mqtt_publish(String service_uuid, String uuid, const char* value) {
    if (!mqtt_client.connected()) {
        if (!init_mqtt()) {

            return;
        }
    }

    String t = mqtt_namespace;
    t += "/";
    t += SUBJECT_TYPE_UUID;
    t += "/";
    t += SUBJECT_UUID;
    t += "/";
    t += service_uuid;
    t += "/";
    t += uuid;
    mqtt_client.publish(t.c_str(), value);
}

static void init_ble() {

    BLEDevice::init(LB_TAG);

    BLEDevice::setPower(ESP_PWR_LVL_P9);

    ble_server = BLEDevice::createServer();
    ble_server->setCallbacks(new LBMServerCallbacks());

    BLEService *subject_service = ble_server->createService(SUBJECT_SERVICE_UUID);
    subject_uuid_characteristic = subject_service->createCharacteristic(
            SUBJECT_UUID_UUID, BLECharacteristic::PROPERTY_READ
    );
    subject_name_characteristic = subject_service->createCharacteristic(
            SUBJECT_NAME_UUID, BLECharacteristic::PROPERTY_READ
    );
    subject_type_characteristic = subject_service->createCharacteristic(
            SUBJECT_TYPE_NAME_UUID, BLECharacteristic::PROPERTY_READ
    );
    subject_type_id_characteristic = subject_service->createCharacteristic(
            SUBJECT_TYPE_UUID_UUID, BLECharacteristic::PROPERTY_READ
    );
    subject_warn_characteristic = subject_service->createCharacteristic(
            SUBJECT_LED_WARN_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    subject_health_characteristic = subject_service->createCharacteristic(
            SUBJECT_LED_HEALTH_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY
    );
    subject_identify_characteristic = subject_service->createCharacteristic(
            SUBJECT_LED_IDENTIFY_UUID, BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
#if defined LIGHT_SERVICE_UUID
    init_ble_light(ble_server);
#endif
#if defined AIR_SERVICE_UUID
    init_ble_air(ble_server);
#endif
#if defined WATER_SERVICE_UUID
    init_ble_water(ble_server);
#endif
#if defined SOIL_SERVICE_UUID
    init_ble_soil(ble_server);
#endif
#if defined EXTRA_SERVICE_UUID
    init_ble_extra(ble_server);
#endif

//    subject_uuid_characteristic->addDescriptor(new BLE2902());
    subject_uuid_characteristic->setValue(SUBJECT_UUID);
    subject_name_characteristic->setValue(SUBJECT_NAME);
    subject_type_characteristic->setValue(SUBJECT_TYPE_NAME);
    subject_type_id_characteristic->setValue(SUBJECT_TYPE_UUID);
    subject_warn_characteristic->setValue("0");
    subject_health_characteristic->setValue(health);
    subject_identify_characteristic->setValue("0");
    subject_service->start();
    BLEAdvertising *ble_advertising = BLEDevice::getAdvertising();
    ble_advertising->addServiceUUID(SUBJECT_SERVICE_UUID);
    ble_advertising->setScanResponse(true);
    ble_advertising->setMinPreferred(0x06);
    ble_advertising->setMaxPreferred(0x12);
    BLEDevice::startAdvertising();
}

static void set_ble_characteristic(BLECharacteristic* characteristic, std::string value) {
    // notify changed value
    if (device_connected) {
        characteristic->setValue(value);
        characteristic->notify();
        delay(333);
    }
    // disconnecting
    if (!device_connected && old_device_connected) {
        delay(500); // Based on BLEnotify: give the bluetooth stack the chance to get things ready
        ble_server->startAdvertising(); // Based on BLEnotify: restart advertising
        old_device_connected = device_connected;
    }
    // connecting
    if (device_connected && !old_device_connected) {
        // do stuff here on connecting
        old_device_connected = device_connected;
    }
}

void status_loop(void* parameters) {

    for (;;) {
//        Serial.print("Status action task is running on core ");
//        Serial.print(xPortGetCoreID());
//        Serial.println(".");
        status_led();
    }
}

void status_led() {

    // in case the user searches the lifebasemeter in the field
    int identify = strtol(
        subject_identify_characteristic->getValue().c_str(), NULL, 10);
    set_ble_characteristic(subject_identify_characteristic, "0");
    for (int i = 0; i < identify; i++) {
        Serial.print("Greeting the user: 'Heeeeello, over heeere!'");
        for (int j = 0; j < 256; j++) {
            ledcWrite(SUBJECT_LED_CHANNEL_RED, j);
            ledcWrite(SUBJECT_LED_CHANNEL_GREEN, j);
            ledcWrite(SUBJECT_LED_CHANNEL_BLUE, j);
            delay(1);
        }
        for (int j = 255; j > 0; j--) {
            ledcWrite(SUBJECT_LED_CHANNEL_RED, j);
            ledcWrite(SUBJECT_LED_CHANNEL_GREEN, j);
            ledcWrite(SUBJECT_LED_CHANNEL_BLUE, j);
            delay(1);
        }
    }


    // under 'unstable conditions' show a blinking warning
    if (water_flow_force_stop > 0) {
        // tell the user to stop watering (or that some problem exists..)
        for (int i = 128; i < 256; i++) {
            ledcWrite(SUBJECT_LED_CHANNEL_RED, 0);
            ledcWrite(SUBJECT_LED_CHANNEL_GREEN, 0);
            ledcWrite(SUBJECT_LED_CHANNEL_BLUE, i);
            delay(24);
        }
    } else if (water_flow_start > 0) {
        // tell the user to play the pump (or that it is pumping..)
        for (int i = 128; i > 0; i--) {
            ledcWrite(SUBJECT_LED_CHANNEL_RED, 0);
            ledcWrite(SUBJECT_LED_CHANNEL_GREEN, 0);
            ledcWrite(SUBJECT_LED_CHANNEL_BLUE, i);
            delay(1);
        }
    } else {
        // under 'stable conditions', just show the status
        //TODO at the moment the health is only set by the user..
        health = subject_health_characteristic->getValue().c_str();
        Serial.print("The overall health condition of this setup is ");
        if (strcmp(health, SUBJECT_HEALTH_GOOD) == 0) {
            Serial.print("good");
            ledcWrite(SUBJECT_LED_CHANNEL_RED, 0);
            ledcWrite(SUBJECT_LED_CHANNEL_GREEN, 8);
            ledcWrite(SUBJECT_LED_CHANNEL_BLUE, 0);
        } else if (strcmp(health, SUBJECT_HEALTH_BAD) == 0) {
            Serial.print("warning");
            ledcWrite(SUBJECT_LED_CHANNEL_RED, 8);
            ledcWrite(SUBJECT_LED_CHANNEL_GREEN, 8);
            ledcWrite(SUBJECT_LED_CHANNEL_BLUE, 0);
        } else {
            Serial.print("critical");
            ledcWrite(SUBJECT_LED_CHANNEL_RED, 8);
            ledcWrite(SUBJECT_LED_CHANNEL_GREEN, 0);
            ledcWrite(SUBJECT_LED_CHANNEL_BLUE, 0);
        }
        Serial.println(".");
        delay(SUBJECT_STATUS_LOOP);
    }
}

void setup() {

    Serial.begin(115200);
    init_ble();
    //TODO see if we need to put this to 'status_loop()' in case connection is lost..
    init_wifi();
    init_sensors();

    ledcSetup(SUBJECT_LED_CHANNEL_RED, SUBJECT_LED_FREQUENCY,
                                        SUBJECT_LED_RESOLUTION);
    ledcSetup(SUBJECT_LED_CHANNEL_GREEN, SUBJECT_LED_FREQUENCY,
                                        SUBJECT_LED_RESOLUTION);
    ledcSetup(SUBJECT_LED_CHANNEL_BLUE, SUBJECT_LED_FREQUENCY,
                                        SUBJECT_LED_RESOLUTION);
    ledcAttachPin(SUBJECT_LED_RED_PIN, SUBJECT_LED_CHANNEL_RED);
    ledcAttachPin(SUBJECT_LED_GREEN_PIN, SUBJECT_LED_CHANNEL_GREEN);
    ledcAttachPin(SUBJECT_LED_BLUE_PIN, SUBJECT_LED_CHANNEL_BLUE);

    // usStackDepth: 8000
    // last parameter: core
    xTaskCreatePinnedToCore(status_loop, "StatusTask", 8000, NULL, 0,
        &StatusTask, 0);

#if defined WATER_SERVICE_UUID
    xTaskCreatePinnedToCore(watering_loop, "WateringTask", 8000, NULL, 0,
        &WateringTask, 0);
#endif
}

void loop() {
    Serial.println("--");
//    Serial.print("Main task running on core ");
//    Serial.print(xPortGetCoreID());
//    Serial.println(".");

    // set the resolution for all analog sensors
    analogReadResolution(ANALOG_RESOLUTION);

    main_loop_delay = MAIN_LOOP_DELAY;

#if defined LIGHT_SERVICE_UUID
    get_light_info();
#endif
#if defined AIR_SERVICE_UUID
    get_dht_info();
#endif
#if defined SOIL_SERVICE_UUID
    get_soil_info();
#endif
#if defined WATER_SERVICE_UUID
    get_water_info();
#endif
#if defined EXTRA_SERVICE_UUID
    get_extra_info();
#endif

    // report the overal health status of this setup
    mqtt_publish(SUBJECT_SERVICE_UUID, SUBJECT_LED_HEALTH_UUID, health);

    // now, just wait for the next loop
    delay(main_loop_delay);
}
