# Dagor Brushless Controller
The Dagor controller is an ESP32 based brushless controller that has an on-board magnetic encoder, a three-phase MOSFET driver, three MOSFET half-bridges, a temperature sensor and current sensing resistors.

There will be an alpha run of the board soon, if you're interested please fill this forms: https://forms.gle/S7gtZfyXZeQ7XbT29

![DagorBoard](Images/Dagor2.1.png)

## Key specs
| Specification    | Rating          |
| ------------- |:-------------:|
| Dimensions      | 44 x 44mm |
| Mass    | 12g |
| Power source voltage      | 5-24V |
| Peak current   | up to 40A |
| Magnetic sensor resolution | 14 bits |
| Temperature sensor range | -10 ~ 120°C |

## 1. The board

### 1.1 The microcontroller
The Dagor board uses either a ESP32-WROOM-32D or a ESP32-WROOM-32U. The first one has an antenna that allows ESP-NOW, wifi or bluetooth communication, the second one has a u.fl connector to attach an external antenna. See section 2.2 to see how to setup your Arduino IDE to be able to flash the ESP32. 

### 1.2 The three-phase gate driver
The DRV8305 is a three-phase gate driver that can drive high and low-side N-channel MOSFETS. What makes this driver special is the bunch of programable functions and the protection included.
With the Dagor Controller you can use the DRV8305 in two operation modes: 3-PWM input or 1-PWM input. The [firmware](Firmware/D021F022/D021F022.ino) works with the driver set through SPI to operate in the 3-PWM mode.
One feature I like is its fault diagnostics, if the nFault pin pulses it means there is a *warning* that can be read through SPI. If the pin is pulled low it means the driver detected a *fault*, which can also be read through SPI, and the output MOSFETs will be placed in their high impedance state.
The Dagor Controller board has a red indicator LED that will turn on if a *warning* or a *fault* is detected.
A few of the faults that the driver reports are the following: high temperature flags, source under or over-voltage, VDS over current monitors, gate drive fault, etc.
The picture below shows the simplified schematic of the driver that can be found in the [DRV8305 datasheet](https://www.ti.com/lit/ds/symlink/drv8305.pdf?ts=1593641896221&ref_url=https%253A%252F%252Fwww.google.com%252F). Please refer to it if you want to learn more about all the features of this driver IC.
![DRV8305](Images/DRV8305Schematic.PNG)

### 1.3 The magnetic sensor

### 1.4 The buck converter
The Dagor board has a buck converter that regulates the input voltage to 3.3V to power the ESP32 and the magnetic sensor and to be able to power external periferals, such as: an external magnetic sensor, encoder, display, etc.

## 2. Getting started
The firmware was done in the Arduino IDE, running the [SimpleFOC](https://simplefoc.com) Arduino Library. By changing a few simple parameter, this code allows you to drive a brushless motor in three control modes: position, velocity and voltage. Make sure you follow the next steps to make sure the code will compile.

### 2.1 Arduino IDE/ ESP32 set-up
The ESP32 is supported using [arduino-esp32 package](https://github.com/espressif/arduino-esp32); open source software provided by the espressif. You can download the support package through the Arduino Board Manager by searching for ESP32 or follow the instructions on the [package installation webpage](https://github.com/espressif/arduino-esp32#installation-instructions). Once the package is installed make sure you can compile one of the examples.

### 2.2 ESP32 flash-mode
To put the Dagor board in flash mode press and hold the Reset Button, press once the Boot Button and then release the Reset Button. You should be able to read on the Serial monitor that the board is waiting for download. Press the upload button and the ESP32 will begin flashing, after it's done press the Reset Button once and the code should start running. 

### 2.3 Running the encoder example
Download the [Encoder Tester](JC01F05/JC01F05.ino) and open it on your Arduino IDE. Upload it to the ESP32 and open the Serial terminal and make sure the measurement is accurate. Move the motor's rotor one complete rotation and make sure the terminal prints 360 degrees. 

### 2.4 SimpleFOC library installation
The [SimpleFOC](https://github.com/simplefoc) repository has an extremely detailed explanation on how to install the library. The easiest way is to install it through the library manager as shown in the picture. 
<img src="Images/LibraryManager.PNG" width=400>

Once installed there's a file that should be replace in order to avoid [this](https://github.com/espressif/arduino-esp32/issues/3743) bug with the ESP32's MCPWM unit (necesarry for smooth motor control). In order to resolve this bug you need to replace one header file in the arduino-esp32 package, called "mcpwm.h".
This file is usually placed in (Windows): 

>C:\Users\(you user name)\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\tools\sdk\include\driver\driver

Navigate to this directory and replace the file with [this](Dependencies/mcpwm.h).

### 2.4 Running the firmware
Download the [firmware](D021F022/D021F022.ino) and open it on your Arduino IDE; there are a few parameters that you will have to tweak for your set-up, the main ones are:
    BLDCMotor motor = BLDCMotor(7); //Write in the parenthesis the number of pole pairs your motor has.
    driver.voltage_power_supply = 12;   //Voltage of your power source
    motor.voltage_limit = voltageLimit;   //Voltage limit
    
To understand the rest of the parameters I really recommend reading the [SimpleFOC](https://docs.simplefoc.com/) documentation. 

## 3.0 Firmware Roadmap
- [ ] Torque control mode
- [ ] PWM input control
- [ ] Full fault report
- [ ] Deep-sleep mode

## Contact 
davidglzrys@gmail.com  
[Linkedin](https://www.linkedin.com/in/david-g-reyes/)  
[YouTube](https://www.youtube.com/channel/UC4gsPZan2T4v5LpJ5J_t7sQ/featured)
