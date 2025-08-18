# Remote Controlled Car with Horn using STM32 and ESP32
This project implements a remote-controlled car with real-time wireless communication through WiFi using an ESP32 and STM32.

---

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Hardware](#hardware)
- [Software](#software)
- [Poster](#poster)

---

## Overview
This project is a Wifi-controlled RC car built using an STM32 and ESP32 microcontroller. Using the ESP32 as an Access Point 
to create a local WiFi, we can connect a laptop with a Python client to send commands to the ESP32 over the WiFi network. The
ESP32 then communicates these commands to the STM32 microcontroller allowing for motor and sound control. <br>

The system allows a user to control the car's movement, speed, direction, and sound production (horn). The car's movement is controlled by 
two front wheels and stabilized using a third. The sound production is directly controlled by the STM32's output to an amplifier connected to a speaker. <br>

This project splits the UI and communication aspects and the control of the car into the two microcontrollers used. The STM32 is 
the control unit, interfacing with DC motors through an motor driver and the speaker and amplifier directly. All commands
are sent through the Python client through to the ESP32 to be sent to the STM32.

## Features
- Wi-Fi-based control with custom Python client
- Real-time speed and direction adjustment
- Sound horn activation via DMA-driven DAC-generated audio
- Independent microcontrollers for communication and control
- Low-latency I²C communication between ESP32 and STM32

### Controls

## Hardware
- ESP32-C3
- STM32L432KC
- H-Bridge Motor Driver
- Two DC motors
- 3 Wheels (2 connected to motor, 1 not connected)
- PAM83 Amplifier
- Speaker
- 9V and 5V battery pack
## Software
- C for STM32/ESP32
- PWM for Motor Control
- DMA + DAC for Sound Control
- I2C for Microcontroller Communication
- Python Client
- WiFi for User Interface
## Poster 

[Car Project Poster.pdf](https://github.com/user-attachments/files/21840768/Car.Project.Poster.pdf)
