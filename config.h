/* WiFi settings */
#define WIFI_SSID               "WifiSsid"
#define WIFI_PASSWORD           "WifiPassword"

/* MQTT server, topic & message configuration */
#define MQTT_SERVER             "192.168.0.23"
#define MQTT_PORT               1883
#define MQTT_TOPIC_CONNECTION   "doorbell_connection"
#define MQTT_MSG_HEARTBEAT      "heartbeat"
#define MQTT_MSG_CONNECT        "connected"
#define MQTT_TOPIC_PROD         "doorbell"
#define MQTT_MSG_DOORBELL       "dingdong"
#define TIMEOUT_RESET_VAL       (40u) /* connection timeout in seconds */

/* NeoPixel configuration (number of pixels and GPIO connection pin) */
#define NEOPIXEL_PIN            D6
#define NEOPIXEL_NUMPIXELS      (12u)

/* adjust colors/brightness according to your needs */
#define NEOPIXEL_COLOR_RED      10, 0, 0
#define NEOPIXEL_COLOR_ORANGE   10, 5, 0
#define NEOPIXEL_COLOR_GREEN    0, 10, 0
#define NEOPIXEL_COLOR_BLUE     0, 0, 10
#define NEOPIXEL_COLOR_BLACK    0, 0, 0
#define NEOPIXEL_COLOR_WHITE    10, 10, 10

