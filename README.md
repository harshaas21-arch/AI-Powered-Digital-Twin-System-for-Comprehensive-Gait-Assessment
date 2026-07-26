# AI-Powered-Digital-Twin-System-for-Comprehensive-Gait-Assessment

## Introduction
Human gait is a complex, rhythmic pattern of locomotion that emerges from the coordinated interaction of the musculoskeletal and central nervous systems. The quantitative study of gait – termed gait analysis – encompasses the measurement of spatial and temporal parameters such as stride length, step cadence, stance and swing phase durations, plantar pressure distribution, joint kinematics, and postural sway. Clinically, gait analysis is indispensable for assessing the progress of patients recovering from orthopaedic interventions such as total knee arthroplasty (TKA) and hip replacement, neurological conditions including Parkinson's disease and post-stroke hemiplegia, and for elderly fall prevention programmes.

In this project, we are building a wearable device that continuously acquires sensor data which is from the 16 FSRs (Force Sensitive Resistors) and a 6 - axis IMU (Inertial Measurement Unit) and sends it over the BLE (Bluetooth Low Energy) to a custom mobile app. The computation is performed by dual core STM32WB55 MCU, which has a ARM Cortex M0+ and M4. A trained model determines the various metrics of the person such as step length, stride length, cadence etc and this data is sent to the Mobile Application.

## My Role:
In this project, my role was to mainly design the PCB of the wearable device and to develop a quick prototype of the working concept.
I have used KiCAD to design the schematic along with placement and routing of all the componenents.


