#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"

// Relay pins
#define RELAY1 23
#define RELAY2 22
#define RELAY3 19
#define RELAY4 18

// Default power state
#define DEFAULT_POWER_MODE false

// Provisioning details
const char *service_name = "RelayBoard";
const char *pop = "12345678";   // proof-of-possession, any 8+ chars

// RainMaker devices
static PowerSwitch switch1("Relay1", NULL, DEFAULT_POWER_MODE);
static PowerSwitch switch2("Relay2", NULL, DEFAULT_POWER_MODE);
static PowerSwitch switch3("Relay3", NULL, DEFAULT_POWER_MODE);
static PowerSwitch switch4("Relay4", NULL, DEFAULT_POWER_MODE);

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
    const char *devName = device->getDeviceName();

    bool newState = val.val.b;

    if (!strcmp(devName, "Relay1")) digitalWrite(RELAY1, newState ? HIGH : LOW);
    if (!strcmp(devName, "Relay2")) digitalWrite(RELAY2, newState ? HIGH : LOW);
    if (!strcmp(devName, "Relay3")) digitalWrite(RELAY3, newState ? HIGH : LOW);
    if (!strcmp(devName, "Relay4")) digitalWrite(RELAY4, newState ? HIGH : LOW);

    device->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, newState);
}

void sysProvEvent(arduino_event_t *sys_event)
{
    switch (sys_event->event_id)
    {
        case ARDUINO_EVENT_PROV_START:
            Serial.printf("\nBLE Provisioning Started\nName: %s\nPoP: %s\n", service_name, pop);
            printQR(service_name, pop, "ble");
            break;

        case ARDUINO_EVENT_PROV_CRED_SUCCESS:
            Serial.println("\nProvisioning successful!");
            break;

        default:
            break;
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    pinMode(RELAY3, OUTPUT);
    pinMode(RELAY4, OUTPUT);

    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY3, LOW);
    digitalWrite(RELAY4, LOW);

    // Initialize RainMaker node
    RMaker.initNode("SwitchBoard", "4-Relay");

    // Add devices and callbacks
    switch1.addCb(write_callback, NULL);
    switch2.addCb(write_callback, NULL);
    switch3.addCb(write_callback, NULL);
    switch4.addCb(write_callback, NULL);

    RMaker.addDevice(switch1);
    RMaker.addDevice(switch2);
    RMaker.addDevice(switch3);
    RMaker.addDevice(switch4);

    RMaker.start();

    // BLE provisioning
    WiFi.onEvent(sysProvEvent);

    WiFiProv.beginProvision(
        WIFI_PROV_SCHEME_BLE,
        WIFI_PROV_SCHEME_HANDLER_FREE_BTDM,
        WIFI_PROV_SECURITY_1,
        pop,
        service_name
    );

    Serial.println("RainMaker BLE 4-relay ready.");
}

void loop()
{
}
