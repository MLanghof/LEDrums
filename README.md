# LEDrums

So you have a MIDI-enabled drum pad, an Arduino and an LED strip and want to make it sparkle when you hit the crash?
You've come to the right place!

## Components

The data flow in this project is as follows:

Drum pad -> Host computer (Processing sketch) -> Arduino -> RGB LED strip

During development, I used these main components:
- Millenium MD90 Mobile Drum
- Any computer with two free USB ports (and two USB-A to USB-B cables)
- Arduino Mega 2560 R3
- Adafruit NeoPixel Digital RGBW LED Strip - Black PCB 60 LED/m (4 meters)

I recommend reading and following the [Adafruit NeoPixel Überguide](https://learn.adafruit.com/adafruit-neopixel-uberguide).
This means that some additional basic electric components are required:
- A power supply with 5 V output with at least 10 A -> 50 W (but ideally 20 A -> 100 W) output
- A DC power supply plug adapter
- A breadboard
- A handful of jumper wires
- A 330 Ohm and a 100k Ohm resistor
- A 1000 uF capacitor

## Setup

- Follow the Adafruit guide(s) to set up the power and data connections for the Arduino and the LED strip.
- Install Processing 3.5.x and the Arduino IDE 2.0 on the host computer.
- Connect the Arduino to the host computer.
- (You might want to try some basic tests as described in the Adafruit guides at this point.)
- Open the two sketches in this repository with the respective IDEs.
- Install the required libraries through each IDE (see top of the respective main file).
- Use the Arduino IDE to identify the COM port that your Arduino landed on. Change the Processing sketch to use that COM port in `setup()`.

If you are using a different drum pad (or other MIDI device), uncomment `MidiBus.list()` in the Processing sketch and find your device in the console output.
Change an existing Instrument class or create a new one for your device.

As it stands, the different Instrument implementations are really boring. However, the architecture is flexible and extensible:
You can add new instruments seamlessly, and for more complex visualizations, some computation may also happen on the host computer instead.
