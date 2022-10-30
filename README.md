# What has been added with this FORK
The main.ino now has wifi support. The idea behind this fork is to allow one to display image on a waveshare display (10.3 inch in my case) with an ESP32. Big thanks to the original developper (clashman[https://github.com/clashman]) of this port/lib. (original project[https://github.com/clashman/it8951])
The app now waits for a websocket payload with a list of pixels to render them. The esp32 having very little memory, a full image cannot be sent or stored. This systeme allows you to send an image line of pixel by line of pixel (or more lines if it fits in the memory allocated in the c++ code). The

An html/js app helper is available as well. it can show you a simulation of the rendering in the browser, given a list of pixels. and can connect to a websocket and send the image to the actuel esp32 (while simulating it as well in the browser).

It is not the fastest code and can be improved a lot, the javascript rendering is wrong i think because of the pixel interpolation in the pixel list. but its good enough for a first release.

Simuator/test app demo available here : https://stan69b.github.io/Waveshare-10.3-it8951-esp32/html-helper-tool/

Feel free to open issues if you have questions on how to use.

INFO: code lacks wifi reconnection in case of router being rebooted... for now you have to unplug replug.
This is not a completed code, I plan on using this base to link it with home assistant and have it render a floorplan and part of the screen with weather, temperature of room infos.

# Quick hack to port the IT8951 display driver to Arduino
These instructions are as simple as possible to allow for easy reproduction. I don't endorse using the Arduino IDE for anything serious. The Espressif/IDF environment provides a way better experience.

The pins need 3.3V! The original Arduino only has 5V pins. Don't use that board without voltage dividers.

Original code taken from: https://github.com/waveshare/IT8951/

# Usage
1. Get a microcontroller and a Waveshare display with IT8951 driver board
    * i.e. an ESP32: https://www.dfrobot.com/product-1590.html (find cheaper and faster shipping ESP32s on ebay etc.)
    * i.e. the 6" version: https://www.waveshare.com/6inch-e-paper-hat.htm
2. Wire your Arduino-compatible board with the following pins
    * Power -> 5V (USB)
    * Ground -> Ground
    * MISO (blue)   19
    * MOSI (yellow) 18
    * SCK (orange)   5
    * CS (green)    12
    * RESET (white) 16
    * HRDY (purple) 17
3. Download Arduino IDE
    * install IDE support for ESP32 ([Tutorial](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/))
4. Flash this repo's code to the board
5. Enjoy your success

# Wiring
![Wiring](/it8951/wiring.jpg)

# Result
![Result](/it8951/result.jpg)

# Example Picture
The example picture that will show up on the epaper is [Obernberg am Inn Adlerwarte: Rotmilan](https://commons.wikimedia.org/wiki/File:Obernberg_am_Inn_Adlerwarte_Rotmilan-0136.jpg)
([CC-BY-SA-4.0](https://creativecommons.org/licenses/by-sa/4.0/)).
It was resized and encoded using 4 bit per pixel, little-endian-like.

To reproduce:
* ./convert_image.py 400 400 pic.jpg
* xxd -i pic > pic.ino
* manually insert width and height variable into pic.ino (unsigned int pic_width = 400; unsigned int pic_height = 400;)

The ESP's RAM is limited, so if you want full resolution images, you need to stream them to the device via WiFi or load them from flash storage.
