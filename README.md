# MQTTDoorbell
This repo contains all the project artifacts for my MQTT "smartifier" doorbell module. The module detects a push of the wired doorbell button and triggers a relay to ring the doorbell, the ESP8266 detects this as well and sends a MQTT message to let my HASS.IO server know. From there I trigger automated notifications as well as playing a doorbell MP3 on my Sonos speakers throughout my home.

Please visit my write-up of [this project to get more details](https://www.jaycollett.com/2019/03/a-smarter-doorbell-without-all-the-extras/).
