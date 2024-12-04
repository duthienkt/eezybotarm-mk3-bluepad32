# Eezybotarm Mk3 - Bluepad32

## Install

### Arduino IDE
Download and install IDE from https://www.arduino.cc/en/software

### BluePad32 for Arduino 
    
In Arduino IDE, go to `File > Preferences > Settings`, add new line in `Additional boards manager URLS` with value

```
https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
```

In tab `BOARD MANAGER` find and install `Bluepad32` library

### CheapStepper, ESP32Servo for Arduino

In tab `LIBRARY MANAGER` find and install `Bluepad32`, `ESP32Servo` library

## Hardware

### ESP32 and Gamepad

Reference: https://bluepad32.readthedocs.io/en/latest/FAQ/

I recommend using the ESP32 Dev Module board that supports "Bluetooth Classic" (AKA BR/EDR) for the best compatibility.

### EEZYbotArm MK3

Download at https://www.thingiverse.com/thing:2838859

How to assembly: https://sketchfab.com/scb3d/collections/assembly-instructions-eezybotarm-mk3-42976a01e685433eac334bb0455dcbfd

### Power supply

I recommend using a 5V power supply for the ESP32 and a 2S battery with a charging circuit...

### Mortor

3 x `28BYJ-48 Stepper Motor + Driver ULN2003`

1 x `G90 servo motor`

## Connection

|ESP32   | OUT     |
|--------|---------|
|        |ULN2003-1  |
| G14    | INT_1   |
| G27    | INT_2   |
| G26    | INT_3   |
| G25    | INT_4   | 
| GND    | GND     |
|        | ULN2003-2 |
| G33    | INT_1   |
| G32    | INT_2   |
| G23    | INT_3   |
| G22    | INT_4   |
| GND    | GND     |
|        | ULN2003-3 |
| G19    | INT_1   |
| G18    | INT_2   |
| G5     | INT_3   |
| G17    | INT_4   |
| GND    | GND     |
|        | Servo   |
| G12    | PWM     |
| GND    | GND     |

| Power supply|  |
|---------|-------|
| 5V          | Servo Vin|
| 5-12V       | ULN2003 Vin x3|
| GND         | GND*     |



