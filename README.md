# Doorbell Gateway MQTT Subscriber

This is the counterpart of [DoorbellPublisher](https://github.com/maehw/DoorbellPublisher) but can also be used independently. Please also find further documentation there.

The subscriber is basically what it's name indicates: a subscriber for MQTT messages provided by a MQTT broker.

The software part subscribes to messages coming from a doorbell. It is typically run on a (ESP8266 based) NodeMCU. In addition, a NeoPixel ring is used. Optionally, an SSD1306 display can be used for status indication and visualization.
