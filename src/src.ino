#include "RMaker.h"
#include "WiFi.h"
#include "BLEProvisioning.h"

// Relay pins
#define RELAY1 23
#define RELAY2 22
#define RELAY3 19
#define RELAY4 18

// Create RainMaker node
static Node my_node("SwitchBoard", "SwitchBoardType");

// Device objects for each relay
static PowerSwitch switch1("Relay1", NULL);
static PowerSwitch switch2("Relay2", NULL);
static PowerSwitch switch3("Relay3", NULL);
static PowerSwitch switch4("Relay4", NULL);

// Callback when any switch state is changed from the app
void write_callback(Device *device, State &state) {
    const char *devName = device->getDeviceName();
    bool newState = state["power"].get<bool>();

    if (!strcmp(devName, "Relay1")) digitalWrite(RELAY1, newState);
    if (!strcmp(devName, "Relay2")) digitalWrite(RELAY2, newState);
    if (!strcmp(devName, "Relay3")) digitalWrite(RELAY3, newState);
    if (!strcmp(devName, "Relay4")) digitalWrite(RELAY4, newState);

    device->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, newState);
}

void setup() {
    Serial.begin(115200);

    // Relay pin setup
    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    pinMode(RELAY3, OUTPUT);
    pinMode(RELAY4, OUTPUT);

    // Default OFF
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, LOW);
    digitalWrite(RELAY3, LOW);
    digitalWrite(RELAY4, LOW);

    // Assign callback
    switch1.addCb(write_callback);
    switch2.addCb(write_callback);
    switch3.addCb(write_callback);
    switch4.addCb(write_callback);

    // Add devices to node
    my_node.addDevice(switch1);
    my_node.addDevice(switch2);
    my_node.addDevice(switch3);
    my_node.addDevice(switch4);

    // RainMaker init (BLE only)
    RMaker.initNode(my_node);

    // Start BLE provisioning (local mode)
    BLEProvisioning.begin("Relay-Board");

    Serial.println("RainMaker BLE 4-Relay Board Ready!");
}

void loop() {
    // Nothing needed here; RainMaker handles everything
}
