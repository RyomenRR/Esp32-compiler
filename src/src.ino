#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"

// ==== Relay pins ====
#define RELAY1 23
#define RELAY2 22
#define RELAY3 19
#define RELAY4 18

// Default power state for all switches
#define DEFAULT_POWER_MODE   false

// BLE provisioning name & PoP (you'll type this in the app)
const char *service_name = "PROV_Relays";
const char *pop          = "12345678";   // anything 8+ chars is fine

// Node and devices
Node my_node("SwitchBoard", "SwitchBoardType");

// Standard Switch devices
static Switch switch1("Relay1", NULL, DEFAULT_POWER_MODE);
static Switch switch2("Relay2", NULL, DEFAULT_POWER_MODE);
static Switch switch3("Relay3", NULL, DEFAULT_POWER_MODE);
static Switch switch4("Relay4", NULL, DEFAULT_POWER_MODE);

// ========== Provisioning event callback (for QR + logs) ==========
void sysProvEvent(arduino_event_t *sys_event) {
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32S2
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n",
                    service_name, pop);
      printQR(service_name, pop, "softap");
#else
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n",
                    service_name, pop);
      printQR(service_name, pop, "ble");
#endif
      break;

    case ARDUINO_EVENT_PROV_INIT:
      // optional: keep auto stop
      break;

    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
      Serial.println("\nProvisioning successful. Connected to Wi-Fi!");
      break;

    default:
      break;
  }
}

// ========== Device write callback (from app / cloud) ==========
void write_callback(Device *device,
                    Param  *param,
                    const param_val_t val,
                    void   *priv_data,
                    write_ctx_t *ctx) {

  const char *dev_name  = device->getDeviceName();
  const char *param_name = param->getParamName();

  // We only care about the Power parameter
  if (strcmp(param_name, ESP_RMAKER_DEF_POWER_NAME) != 0) {
    return;
  }

  bool new_state = val.val.b;  // true / false from app

  // If your relay board is active-LOW, swap HIGH/LOW here.
  if (!strcmp(dev_name, "Relay1")) {
    digitalWrite(RELAY1, new_state ? HIGH : LOW);
  } else if (!strcmp(dev_name, "Relay2")) {
    digitalWrite(RELAY2, new_state ? HIGH : LOW);
  } else if (!strcmp(dev_name, "Relay3")) {
    digitalWrite(RELAY3, new_state ? HIGH : LOW);
  } else if (!strcmp(dev_name, "Relay4")) {
    digitalWrite(RELAY4, new_state ? HIGH : LOW);
  }

  // Report new value back to RainMaker
  device->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, new_state);
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  // ====== Relay GPIO ======
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  // Default OFF (change HIGH/LOW depending on relay type)
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);
  digitalWrite(RELAY4, LOW);

  // ====== RainMaker Node & Devices ======
  // initNode() normally returns a Node, but you can also use the
  // already-created my_node object via this overload:
  RMaker.initNode(my_node);

  // Attach callbacks
  switch1.addCb(write_callback, NULL);
  switch2.addCb(write_callback, NULL);
  switch3.addCb(write_callback, NULL);
  switch4.addCb(write_callback, NULL);

  // Add devices to node
  my_node.addDevice(switch1);
  my_node.addDevice(switch2);
  my_node.addDevice(switch3);
  my_node.addDevice(switch4);

  // Optional services (can be removed to save RAM)
  // RMaker.enableOTA(OTA_USING_TOPICS);
  // RMaker.enableTZService();
  // RMaker.enableSchedule();
  // RMaker.enableScenes();

  // Start RainMaker
  RMaker.start();

  // ====== Start BLE provisioning ======
  WiFi.onEvent(sysProvEvent);

#if CONFIG_IDF_TARGET_ESP32S2
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP,
                          WIFI_PROV_SCHEME_HANDLER_NONE,
                          WIFI_PROV_SECURITY_1,
                          pop,
                          service_name);
#else
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE,
                          WIFI_PROV_SCHEME_HANDLER_FREE_BTDM,
                          WIFI_PROV_SECURITY_1,
                          pop,
                          service_name);
#endif

  Serial.println("RainMaker 4-Relay Board firmware started.");
}

void loop() {
  // Nothing; callbacks + RainMaker handle everything.
}
