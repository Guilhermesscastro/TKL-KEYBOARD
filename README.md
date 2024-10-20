
# USB HID Keyboard Project

This repository contains the source code and documentation for the **USB HID Keyboard Project**, developed as part of the LABSI course. The project implements a custom mechanical keyboard with a PT-PT TKL (Tenkeyless) layout featuring 87 keys, an OLED display, and a rotary encoder.

## Table of Contents
- [Introduction](#introduction)
- [Architecture](#architecture)
- [Hardware](#hardware)
- [Software](#software)
- [Results](#results)
- [Conclusions](#conclusions)
- [References](#references)

## Introduction
This project is a prototype of a mechanical keyboard with TKL format, chosen for its popularity in gaming and productivity due to the balance between compactness and functionality. It omits the numpad but retains all other essential keys. In addition to the basic keyboard features, the project integrates:
- An **OLED display** that shows a **Bongo Cat typing animation** whenever the user types, adding a fun and interactive visual element.
- A **fully functional rotary encoder** that can be used for multimedia control or fine-tuning in design/editing software.

The prototype aims to deliver premium keyboard features at a lower cost, comparable to high-end keyboards available in the market.

## Architecture
The overall architecture of the project is illustrated below:

*(insert image/gif)*

The core of the project is a microcontroller that performs the following functions:
- Detects key presses from the keyboard matrix.
- Processes inputs from the rotary encoder.
- Configures and updates the OLED display to show the typing animation.

The information from the keyboard and encoder is sent to the connected computer using the USB HID protocol via the microcontroller’s USB interface.

## Hardware
The hardware components of the project include the PCB and the following components:
- **RP2040 microcontroller**: Chosen for its numerous GPIO pins, USB communication capabilities, and ease of reprogramming via USB as a mass storage device.
- **128Mb Flash memory**, **12MHz quartz oscillator**, and a **5V-3.3V voltage regulator** to power the microcontroller.
- **Hot-swappable sockets** for the mechanical switches, allowing switches to be replaced without soldering.
- A **folded matrix** design for the key layout to optimize IO usage, reducing the required pins from 16x6 to 12x8.

Below are some hardware-related images:

- Switch Mechanism: *(insert image/gif)*
- Hot-swap Socket: *(insert image/gif)*
- OLED Display: *(insert image/gif)*
- Rotary Encoder: *(insert image/gif)*

The PCB design was created using KiCad. Below are the schematics and PCB layout:

*(insert image/gif)*

### 3D Printed Case
*(This section will be updated soon.)*

## Software
The firmware for the keyboard was developed in **C** using the **VSCode IDE** and **CMake** for compilation. The software stack includes the **Raspberry Pi Pico SDK** and the **TinyUSB library** for USB communication. The display driver is controlled via the I2C protocol.

The software is divided into three C files:
- **main.c**: Handles initialization and tasks such as updating the display, managing HID reports, and controlling the LED indicator.
- **keyboard.c**: Processes key presses and manages HID reports sent to the connected computer.
- **led.c**: Controls the keyboard LED blinking at 1Hz.
- The **rotary encoder** is now fully functional and integrated into the firmware, allowing for volume control or other multimedia actions.

Key functionalities include:
- Detecting key presses and generating appropriate HID reports.
- Displaying the caps lock state and **Bongo Cat typing animation** on the OLED screen.
- Controlling volume or other settings via the rotary encoder.

Below are some flowcharts representing the main functions:

*(insert image/gif)*

### Known Issues
- **Spacebar double clicks**: Sometimes, the spacebar registers double clicks. This issue is under investigation and will be addressed in future updates.
- **3D printed case (v2)**: A second version of the case design is in progress and will be added in upcoming updates.

## Results
The final prototype works well, especially with the new addition of the Bongo Cat animation and the fully functional rotary encoder. However, there is still a known issue with the spacebar double clicking. Here are images of the assembled PCB:

*(insert image/gif)*

## Conclusions
Overall, the project has achieved most of its objectives, delivering a functional and interactive mechanical keyboard with a unique typing animation. Future improvements include:
- Fixing the spacebar double click issue.
- Updating the 3D printed case design (v2).
- Expanding the software features for additional display capabilities.

Given more time, downsizing the keyboard to a macropad could have allowed for more in-depth software development.

## References
1. [Raspberry Pi RP2040 Hardware Design](https://datasheets.raspberrypi.com/rp2040/hardware-design-with-rp2040.pdf)
2. [Japanese Duplex Matrix](https://kbd.news/The-Japanese-duplex-matrix-1391.html)
3. [Cherry MX Mechanical Switch GIF](https://blog.adafruit.com/2016/09/16/animated-gifs-help-visually-explain-cherry-mx-mechanical-keyboard-switches/)
4. [Kailh Hot Swap Sockets](https://divinikey.com/products/kailh-hot-swap-sockets)
5. [OLED Display Datasheet](https://mauser.pt/catalog/product_info.php?products_id=096-8754)
6. [Rotary Encoder Details](https://www.tme.eu/pt/details/ec12e20-24p24c-sw/codificadores-incrementais/sr-passives/)
7. [TinyUSB Documentation](https://docs.tinyusb.org/en/latest/info/index.html)
8. [Pico SSD1306 Library](https://github.com/daschr/pico-ssd1306)
9. [USB HID Usage Table](https://www.usb.org/sites/default/files/hut1_4.pdf)
