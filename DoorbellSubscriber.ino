// Display via external SSD1306 OLED display and/or NeoPixels
// Include the correct display library
// it is https://github.com/ThingPulse/esp8266-oled-ssd1306 --> see API description there

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "SSD1306Wire.h"

#include "images.h"
#include "config.h"

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, SDA, SCL);

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_NeoPixel pixels(NEOPIXEL_NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup_wifi(void);
void setup_mqtt(void);
void drawBrokenLink(void);
void drawBellUnknown(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void setNeopixelColor(uint8_t r, uint8_t g, uint8_t b);

volatile uint8_t g_nTimeout = TIMEOUT_RESET_VAL;

void setup()
{
    /* start the serial console for debugging/logging */
    Serial.begin(115200);
    Serial.println();

    pixels.begin(); // initialize NeoPixel object
    pixels.clear(); // set all pixel colors to 'off'
    delay(100);
    setNeopixelColor(NEOPIXEL_COLOR_ORANGE);
    delay(200);
  
    display.init();
    display.flipScreenVertically();
  
    setup_wifi();
    setup_mqtt();

    setNeopixelColor(NEOPIXEL_COLOR_GREEN);
    delay(1000);
    setNeopixelColor(NEOPIXEL_COLOR_BLACK);
}

void mqtt_reconnect()
{
    bool bBroken = false;

    // Loop until we're reconnected
    while( !client.connected() )
    {
        Serial.print("Attempting MQTT connection...");

        // Attempt to connect
        if( client.connect("DoorbellSubscriber", "", "") )
        {
            Serial.println("connected");

            /* subscriber must subscribe here */
            client.subscribe(MQTT_TOPIC_CONNECTION);
            client.subscribe(MQTT_TOPIC_PROD);
            Serial.println("subscribed");
        }
        else
        {
            bBroken = true;
            setNeopixelColor(NEOPIXEL_COLOR_RED);
            display.clear();
            drawBrokenLink();
            display.display();

            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" trying again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
    setNeopixelColor(NEOPIXEL_COLOR_BLACK);

    if( bBroken )
    {
        /* connection was broken (could not directly be re-established; should remove the broken link symbol */
        display.clear();
        display.display();
    }
}

void setup_wifi()
{
    delay(10);

    display.clear();
    drawWifiDisconnected();
    display.display();
    delay(2000);

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to WiFi with SSID: \"");
    Serial.print(WIFI_SSID);
    Serial.println("\"");
  
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
  
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 20, "WiFi connected!");
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 40, WiFi.localIP().toString() );
    display.display();

    delay(3000);
}

void setup_mqtt(void)
{
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(mqtt_callback);

    if (!client.connected())
    {
        mqtt_reconnect();
    }
}

void drawBell(bool bOnOff)
{
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files

    if( bOnOff )
    {
        display.drawXbm(34, 2, doorbell_width, doorbell_height, doorbell_bits);
    }
    else
    {
        display.drawXbm(34, 2, doorbell_noring_width, doorbell_noring_height, doorbell_noring_bits);
    }
}

void drawBellUnknown(void)
{
    display.drawXbm(34, 2, doorbell_unknown_width, doorbell_unknown_height, doorbell_unknown_bits);
}

void drawBrokenLink(void)
{
    display.drawXbm(34, 2, broken_link_width, broken_link_height, broken_link_bits);
}

void drawWifiDisconnected(void)
{
    display.drawXbm(34, 2, wifi_disconnected_width, wifi_disconnected_height, wifi_disconnected_bits);
}

void drawHeart(void)
{
    display.drawXbm(118, 2, heart_width, heart_height, heart_bits);
}

void loop()
{
    static uint8_t nProgressBarVal = 100;
    if (!client.connected())
    {
        mqtt_reconnect();
    }
    client.loop();

    /* only decrement when timeout has not already occured, else uint underflow */
    if( g_nTimeout > 0 )
    {
        g_nTimeout--;
        Serial.print("Timeout: ");
        Serial.println(g_nTimeout);

        /* detect transition: timeout reached */
        if( 0 == g_nTimeout )
        {
            setNeopixelColor(NEOPIXEL_COLOR_RED);
            Serial.print("Remote doorbell timed out!");
        }
        else
        {
            setNeopixelColor(NEOPIXEL_COLOR_BLACK);
        }
    }

    display.clear();
    nProgressBarVal = g_nTimeout*100/TIMEOUT_RESET_VAL;
    if( 0 == g_nTimeout )
    {
        drawBellUnknown();
    }
    else
    {
        display.drawProgressBar(1, 1, 110, 6, nProgressBarVal);
    }
    display.display();
    delay(1000);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length)
{
    (void)topic; /* don't care about the topic as we're only subscribed to a single one */

    /* reset timeout */
    g_nTimeout = TIMEOUT_RESET_VAL;

    Serial.println("Message arrived!");

    Serial.print("Message payload: ");
    for(int i=0; i<length; i++)
    {
        Serial.print( (char)payload[i] );
    }
    Serial.println();

    if( 0 == strncmp((const char*)payload, MQTT_MSG_DOORBELL, 8) )
    {
        /* we received a ding-dong! */
        Serial.println("Ding-dong!");

        for(int k=0; k<10; k++)
        {
            display.clear();
            drawBell(true);
            display.display();
            setNeopixelColor(NEOPIXEL_COLOR_BLUE);
            delay(1000);
          
            display.clear();
            drawBell(false);
            display.display();
            setNeopixelColor(NEOPIXEL_COLOR_WHITE);
            delay(1000);
        }

        setNeopixelColor(NEOPIXEL_COLOR_BLACK);
    }
    else
    {
        /* we did not receive a ding-dong; at least signal heartbeat! */

        /* add heart symbol for heartbeat to the display */
        //display.clear();
        drawHeart();
        display.display();
  
        /* make sure that the heart is displayed at least for a short time */
        delay(300);
    }
}

void setNeopixelColor(uint8_t r, uint8_t g, uint8_t b)
{
//    Serial.print("[DEBUG] Setting neopixel color to r=");
//    Serial.print(r);
//    Serial.print(", g=");
//    Serial.print(g);
//    Serial.print(", b=");
//    Serial.println(b);

    for(int i=0; i < NEOPIXEL_NUMPIXELS; i++)
    {
        pixels.setPixelColor(i, r, g, b);
    }
    pixels.show();
}

