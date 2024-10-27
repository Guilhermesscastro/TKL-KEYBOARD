# USB HID Keyboard Project

This repository contains the source code and documentation for the **USB HID Keyboard Project**, developed as part of the LABSI course. The project implements a custom mechanical keyboard with a PT-PT TKL (Tenkeyless) layout featuring 87 keys, an OLED display with a Bongo Cat typing animation, a rotary encoder for multimedia control, and an LED indicator to reflect device states.

## Table of Contents
- [Introduction](#introduction)
- [Architecture](#architecture)
- [Hardware](#hardware)
- [Software](#software)
- [Source Code](#source-code)
- [Current Functionalities](#current-functionalities)
- [Results](#results)
- [Conclusions](#conclusions)
- [References](#references)
- [Acknowledgements](#acknowledgements)

## Introduction
This project is a prototype of a mechanical keyboard with a TKL format with a PCB board design based on the RP2040 microcontroller. This project was made in the LABSI course at Porto Higher Institute of Engineering (ISEP). In addition to the basic keyboard features, the project integrates:
- An **OLED display** that shows a **Bongo Cat typing animation** whenever the user types, adding a fun and interactive visual element.
- A **fully functional rotary encoder** that can be used for multimedia control or fine-tuning in design/editing software.
- An **LED indicator** that reflects the device's USB connection state (mounted, not mounted, suspended).



## Architecture
The overall architecture of the project is illustrated below:

![ArquiteturaGeral_V2](https://github.com/user-attachments/assets/4f6e67ad-08d4-4f27-a2c5-b71a01cf1c7b)


The core of the project is a microcontroller that performs the following functions:
- Detects key presses from the keyboard matrix.
- Processes inputs from the rotary encoder.
- Configures and updates the OLED display to show the typing animation.
- Manages LED indicators based on USB connection states.
  
The information from the keyboard and encoder is sent to the connected computer using the USB HID protocol via the microcontroller’s USB interface.

## Hardware
The hardware components of the project include the PCB and the following components:
- **RP2040 microcontroller**: Chosen for its numerous GPIO pins, USB communication capabilities, and ease of reprogramming via USB as a mass storage device.
- **128Mb Flash memory**, **12MHz quartz oscillator**, and a **5V-3.3V voltage regulator** to power the microcontroller.
- **Hot-swappable sockets** for the mechanical switches, allowing switches to be replaced without soldering.
- A **folded matrix** design for the key layout to optimize IO usage, reducing the required pins from 16x6 to 12x8.
- **Rotary Encoder**: For multimedia control.
- **OLED Display (SSD1306)**: For displaying animations and status information.
- **LED Indicator**: Blinks at different intervals to reflect USB states, show caps lock state, and blink at 1Hz which was a requirement of the LABSI course but can be disabled.

### PCB Design
The PCB design was inspired by various resources, including:
- **[Noah Kiser's TKL Series](https://www.youtube.com/watch?v=6Z49bynRqj8)**: For the keyboard layout and matrix design.
- **[Jeffminton's Keyboard Exposed Diodes](https://github.com/jeffminton/keyboard_exposed_diodes_standard_tkl)**: For implementing stable key matrix scanning.
- **[Masterzen](https://www.masterzen.fr/)** and **[Joe Scotto](https://www.youtube.com/@joe_scotto)**: For advanced PCB design techniques and case integration.

### 3D Printed Case
*(This section will be updated soon.)*

## Software
The firmware for the keyboard was developed in **C** using the **VSCode IDE** and **CMake** for compilation. The software stack includes:
- **Raspberry Pi Pico SDK**
- **TinyUSB library** for USB communication.
- **[pico-ssd1306](https://github.com/daschr/pico-ssd1306)** for OLED display control via I2C.
- **[pi-pico-pio-quadrature-encoder](https://github.com/jamon/pi-pico-pio-quadrature-encoder)** for handling rotary encoder inputs using PIO.

### Software Inspirations and Libraries
The project leverages several open-source libraries and resources:
- **[picokey](https://github.com/0bs3n/picokey)**: For keyboard matrix scanning and USB HID implementation.
- **[OLED-BongoCat-Revision](https://github.com/pedker/OLED-BongoCat-Revision)**: For integrating the Bongo Cat animation with the OLED display.
- **[pico-superkey-board](https://github.com/guruthree/pico-superkey-board)**: For enhancing keyboard functionalities and key processing.
- **[pico-ssd1306](https://github.com/daschr/pico-ssd1306)**: SSD1306 OLED display driver for Raspberry Pi Pico.
- **[pi-pico-pio-quadrature-encoder](https://github.com/jamon/pi-pico-pio-quadrature-encoder)**: PIO programs for handling quadrature encoder inputs.

## Source Code

The source code of this project is organized into several files, each responsible for different functionalities:

### `main.c`
The entry point of the firmware. It initializes all hardware components, sets up USB HID, and enters the main loop where it continuously handles USB tasks, HID reports, and updates the Bongo Cat animation.

### `keyboard.c` and `keyboard.h`
Handles the keyboard matrix scanning, key debouncing, HID report generation, and management of key states and buffers. Implements the logic for detecting key presses and sending appropriate HID reports to the host.

### `bongocat.c` and `bongocat.h`
Manages the Bongo Cat animation displayed on the OLED screen. Handles different animation states based on user interactions and timing to provide a dynamic visual feedback mechanism.

### `led.c` and `led.h`
Controls the LED indicator that reflects the USB connection state. Manages different blink patterns corresponding to various USB states such as mounted, not mounted, and suspended.

### `pico-ssd1306/ssd1306.c` and `pico-ssd1306/ssd1306.h`
Implements the driver for the SSD1306 OLED display. Handles initialization, drawing pixels, rendering images, and managing the display buffer.

### `encoder.pio` and `encoder.pio.h`
Provides the Programmable I/O (PIO) programs for handling the rotary encoder's quadrature signals. Manages the decoding of rotary inputs to enable smooth multimedia control.

### `encoder.pio.h`
Header file for `encoder.pio`, declaring the initialization functions for the encoder PIO state machines.

## Current Functionalities

### Key Features
- **87-Key TKL Layout**: Provides all essential keys with a compact form factor without a numpad.
- **Hot-Swappable Switches**: Allows users to replace mechanical switches without soldering.
- **OLED Display with Bongo Cat Animation**: Displays a playful animation that reacts to typing activity.
- **Rotary Encoder**: Enables multimedia controls such as volume adjustment.
- **LED Indicator**: Blinks at different intervals to indicate USB connection states:
  - **250 ms**: Not mounted.
  - **500 ms**: Mounted.
  - **2500 ms**: Suspended.
- **USB HID Protocol**: Ensures seamless integration with computers as a standard HID device.
- **Debounced Key Scanning**: Prevents ghosting and ensures reliable keypress detection.
  
### Software Enhancements
- **HID Task Management**: Efficiently handles HID reports and manages keyboard and encoder inputs.
- **Bongo Cat Animation Control**: Manages the animation states based on user interactions and elapsed time.
- **Rotary Encoder Acceleration**: Implements acceleration for quick rotary turns to enhance user experience.
- **Bootloader Mode Check**: Supports entering bootloader mode via key combination for firmware updates.

## Results
The final prototype works well, especially with the new addition of the Bongo Cat animation and the fully functional rotary encoder. The LED indicator effectively communicates the device's USB connection state to the user. Here are images of the assembled Keyboard:

![PCB_BAIXO](https://github.com/user-attachments/assets/8fc8e7ac-2bfe-4a12-98fa-5275a03cabcd)
![PCB_CIMA](https://github.com/user-attachments/assets/f243551e-7dbe-45a3-80ae-55906385d3e5)
![VID_20240911_211501-ezgif com-speed (1)](https://github.com/user-attachments/assets/599c9386-edca-4ca7-bd7f-e1e88283797e)



## Conclusions
Overall, the project has achieved most of its objectives, delivering a functional and interactive mechanical keyboard with unique features like the Bongo Cat animation and rotary encoder. Future improvements include:
- Updating the 3D printed case design (v2).
- Expanding the software features for additional display capabilities.
- Enhancing encoder functionalities for more control options.

Being given more time and downsizing the keyboard to a macropad could have allowed for more in-depth software development and additional customization when realizing the project during my semester, however, the current state of the project already includes the basic functionalities that were required by the LABSI course, and i continued to work on it as a personal project.

## References
1. [Raspberry Pi RP2040 Hardware Design](https://datasheets.raspberrypi.com/rp2040/hardware-design-with-rp2040.pdf)
2. [Japanese Duplex Matrix](https://kbd.news/The-Japanese-duplex-matrix-1391.html)
3. [Cherry MX Mechanical Switch GIF](https://blog.adafruit.com/2016/09/16/animated-gifs-help-visually-explain-cherry-mx-mechanical-keyboard-switches/)
4. [Kailh Hot Swap Sockets](https://divinikey.com/products/kailh-hot-swap-sockets)
5. [OLED Display Datasheet](https://mauser.pt/catalog/product_info.php?products_id=096-8754)
6. [Rotary Encoder Details](https://www.tme.eu/pt/details/ec12e20-24p24c-sw/codificadores-incrementais/sr-passives/)
7. [TinyUSB Documentation](https://docs.tinyusb.org/en/latest/info/index.html)
8. [pico-ssd1306](https://github.com/daschr/pico-ssd1306)
9. [USB HID Usage Table](https://www.usb.org/sites/default/files/hut1_4.pdf)
10. [picokey](https://github.com/0bs3n/picokey)
11. [OLED-BongoCat-Revision](https://github.com/pedker/OLED-BongoCat-Revision)
12. [pico-superkey-board](https://github.com/guruthree/pico-superkey-board)
13. [pi-pico-pio-quadrature-encoder](https://github.com/jamon/pi-pico-pio-quadrature-encoder)

## Acknowledgements
A heartfelt thank you to all the developers and communities that provided invaluable resources and inspiration for this project:
- **[0bs3n](https://github.com/0bs3n)** for the [picokey](https://github.com/0bs3n/picokey) project.
- **[Pedker](https://github.com/pedker)** for the [OLED-BongoCat-Revision](https://github.com/pedker/OLED-BongoCat-Revision) library.
- **[Guruthree](https://github.com/guruthree)** for the [pico-superkey-board](https://github.com/guruthree/pico-superkey-board).
- **[Daschr](https://github.com/daschr)** for the [pico-ssd1306](https://github.com/daschr/pico-ssd1306) display driver.
- **[Jamon Terrell](https://github.com/jamon)** for the [pi-pico-pio-quadrature-encoder](https://github.com/jamon/pi-pico-pio-quadrature-encoder).
- **[Noah Kiser](https://www.youtube.com/@noahkiser)** and **[Joe Scotto](https://www.youtube.com/@joe_scotto)** for their tutorials and PCB design inspirations.
- **[Masterzen](https://www.masterzen.fr/)** for advanced PCB design techniques and tools.
- **[Jeffminton](https://github.com/jeffminton)** for the [Keyboard Exposed Diodes Standard TKL](https://github.com/jeffminton/keyboard_exposed_diodes_standard_tkl) project.

Your open-source contributions have been instrumental in the development of this keyboard project. Thank you for sharing your knowledge and tools with the community!

---
