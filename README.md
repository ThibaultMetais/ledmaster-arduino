# Led master 

A bluetooth controler for a WS2811 led strip.

This project is linked with an [ionic app](https://github.com/ThibaultMetais/ledmaster2) in order to control the strip.

## Build

The project as been originaly created for an Arduino Mega.

You will need the Adafruit NeoPixel library included in the project.

## Wiring
```
HC06 RX -> TX1
HC06 TX -> RX1
Arduino 6 (pwm) -> Do
```
The HC-06 should be powered with 3.3V.

## Bluetooth commands

### Gets

getInfo => get the state of the controler into JSON.

### Sets

setMode:[mode Number]

setColors:[RGB in ascii 255, no separator between colors] 

setDuration:[duration in seconds]

setIntensity:[intensity from 0 to 255]

setOnOff:[1/0]
