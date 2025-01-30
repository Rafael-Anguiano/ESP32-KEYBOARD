# Wireless Corne Keyboard

![image](https://github.com/user-attachments/assets/1f7778c2-979a-491d-9b50-c5eaa662c57b)

## Description

This is a Corne Mechanical Keyboard made wireless by using two ESP32 as microcontrollers. The keyboard is split into two parts, each part has 21 keys, a microcontroller, a battery, and a switch to turn it on and off. The keyboard uses the [ESP32-BLE-KEYBOARD](https://github.com/T-vK/ESP32-BLE-Keyboard) library to connect to the computer via Bluetooth.

## Table of Contents

- [Description](#description)
- [Instructions](#instructions)
- [Releases](#releases)
- [Materials and Tools](#materials-and-tools)
- [Results](#results)
- [Dependencies](#dependencies)
- [Authors](#authors)

## Instructions

In this repository you will find everything you need to build your own Wireless Corne Keyboard, and personalize it as you like. The following steps will guide you through the process of building the keyboard.

The repository contains the following parts:

1. **Code**. The code used to program the ESP32 microcontrollers, separated by Left and Right Keyboard. **In Code**
2. **Schematic**. The schematic of the keyboard. **In Releases**
3. **PCB Design**. The PCB design of the keyboard. **In Releases**
4. **Gerber Files**. The files used to create the PCB. **In Releases**
5. **Case Design**. The case design of the keyboard. (Coming soon)

> [!NOTE]
> In the repository, each Keyboard has it's own configuration for the PlatformIO, to facilitate the programming of the microcontrollers.
>
> If you need just the source code, check the **src directory** inside each Keyboard directory.

## Releases

In the [releases](https://github.com/Rafael-Anguiano/Wireless-Corne-Keyboard/releases/tag/v1.0) section you will find the files used around this project.

- **Gerber files**. Used to order the PCB.
- **KiCad project**, used to design the PCB.
  - **Schematic**. The schematic of the keyboard.
  - **PCB Design**. The PCB design of the keyboard.
- **Case Design**. The case design of the keyboard. (Coming soon)

## Materials and Tools

- 2 ESP32 Devkit v1.
- 2 Micro Switch on/off.
- 4 JST Conector of 2 pins.
- 4 Li-Po Batteries of 3.7V.
- 42 MX Hotswap Sockets.
- 42 MX Switches.
- 42 Keycaps.
- 42 Diodes 1N4148W (smd).
- Soldering equipment.

## Dependencies

- [ESP32-BLE-KEYBOARD](https://github.com/T-vK/ESP32-BLE-Keyboard): Library used to connect the ESP32 to the computer via Bluetooth and send the key stroke.

## Results

## Current Issues

- The space between the keys has to be adjusted.
- There is missing a case design for the keyboard.
- There is missing a charging circuit for the batteries.

## Authors

- **[Rafael Anguiano](https://github.com/Rafael-Anguiano)** / *Software Engineer*
- **[Ramón Meza](https://github.com/RAMF-Meza)** / *Mechatronics Engineer*
- **[Joaquín Meza](https://github.com/Joaquin-Meza)** / *Biomedical Engineer*
