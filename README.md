# AI-Powered-Digital-Twin-System-for-Comprehensive-Gait-Assessment

## Problem Statement:
*Post-surgery rehabilitation requires continuous gait monitoring to assess recovery, detect asymmetry, and prevent fall risk.

*Existing solutions are either laboratory-dependent, expensive, and lack personalized AI-driven analysis for home-based use.

*Standard wearables lack the resolution to measure dynamic plantar pressure distribution, making it impossible to detect compensatory movements or subtle improvements in gait symmetry

<img width="1063" height="484" alt="image" src="https://github.com/user-attachments/assets/3612590a-24e2-4468-9439-d813fcc01deb" />


## Introduction
Human gait is a complex, rhythmic pattern of locomotion that emerges from the coordinated interaction of the musculoskeletal and central nervous systems. The quantitative study of gait – termed gait analysis – encompasses the measurement of spatial and temporal parameters such as stride length, step cadence, stance and swing phase durations, plantar pressure distribution, joint kinematics, and postural sway. Clinically, gait analysis is indispensable for assessing the progress of patients recovering from orthopaedic interventions such as total knee arthroplasty (TKA) and hip replacement, neurological conditions including Parkinson's disease and post-stroke hemiplegia, and for elderly fall prevention programmes.

In this project, we are building a wearable device that continuously acquires sensor data which is from the 16 FSRs (Force Sensitive Resistors) and a 6 - axis IMU (Inertial Measurement Unit) and sends it over the BLE (Bluetooth Low Energy) to a custom mobile app. The computation is performed by dual core STM32WB55 MCU, which has a ARM Cortex M0+ and M4. A trained model determines the various metrics of the person such as step length, stride length, cadence etc and this data is sent to the Mobile Application.

## Project Objectives:
*Remote Diagnostics 
*Injury Prevention
*Wireless Data Fusion
*Dynamic Symmetry Scoring
*Mobile Digital Twin
*Future Scalability

<img width="706" height="774" alt="image" src="https://github.com/user-attachments/assets/b3b88010-c857-4b73-a2b9-34edc16826e3" />

## Methodology:
<img width="1500" height="687" alt="image" src="https://github.com/user-attachments/assets/3b7eb6a3-ce8e-43ca-bba6-7ca7d616c6ef" />

## My Role:
In this project, my role was to mainly design the PCB of the wearable device and to develop a quick prototype of the working concept.
I have used KiCAD to design the schematic along with placement and routing of all the componenents by following the manufacturer’s datasheets.

### List of Components used:
* STM32WB55CGU6 - A dual core MCU consisting of (ARM Cortex M0+ and M4) and 49 pin IC package.
* CD74HC4067 - A 16:1 Multiplexer used for connecting the 16 - FSR sensor insole.
* MCP73871 - This IC provides a Battery Management System for the wearable, it inputs the supply power from the USB, powers the wearable along with charging the battery.
* LT1962-3.3V - A Buck converter which converts the 5V supply to 3.3V, which is essential for the proper working of the MCU.
* LSM6DSO - A 6-axis Inertial Measurement Unit measuring movement in X,Y,Z direction along with Roll, Pitch, Yaw movement.
* MLPF-WB55 - A low pass filter package required for RF BLE Antenna.
* RFANT3216120A5T - The Antenna for BLE Transmission.
* DMG3415U-7 - A PMOS Transistor for power efficient power rails.
* SWD Tag Connector - For programming the device.
* USBLC6-2SCC6Y - Connecting USB connector.
* C-type USB Connector
* Decoupling Capacitors
* Resistors
* LEDs
* Ferrite Bead and Inductors
* 32kHz and 32MHz Crystal Oscillator

### PCB - Front Layer
<img width="247" height="541" alt="image" src="https://github.com/user-attachments/assets/e749c581-a6e0-4621-be9a-57e3c8baffd4" />

### PCB - Back Layer
<img width="227" height="532" alt="image" src="https://github.com/user-attachments/assets/71c0f53d-45e7-4530-99ae-6f87ee145d9f" />

### Current progress:
<img width="442" height="804" alt="image" src="https://github.com/user-attachments/assets/65dbf63c-3a74-452d-83e2-bf5905b3130b" />






