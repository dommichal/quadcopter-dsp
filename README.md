# Quadcopter design made from scratch

The goal of the project is to create a working model of a quadcopter UAV, which can be
controlled with a Xbox controller. The project utilises the MPU6050 inertial measurement
unit, which combines MEMS accelerometer and gyroscope into a single IC. The combination of
sensor readings from the IMU combined with an adequate mathematical model and a regulating
system allows to make a quadcopter with a stable control system.

![](https://github.com/Cyberdom123/quadcopter---documentation/tree/main/img/video.gif)


# Quadcopter project implementation

The MCU used in the project implementation is STM32F103C6T6, an ARM Cortex-M3 mi-
crocontroller, containing 32Kbytes of ROM. F1303 has built-in hardware support for I2C and
SPI busses used for data transmission between on-board peripherals.
The MPU6050 has been used as a sensor. It is an inertial measurement unit (IMU), a
MEMS integrated circuit combining in itself an accelerometer and a gyroscope. For easy and
intuitive reading of the sensor data at the code level, a library for initialising and handling the
MPU has been written. It uses the built-in I2C bus which is used to exchange data between the
IMU and MCU. The transmission is made in 400kHz mode. In order to save MCU execution
time, a DMA controller has been used, allowing for sending data from RAM to the peripheral.
Similarly, a library for nRF24L01 radio module has been written. nRF is being used for
digital radio transmission between the UAV and a PC. It is connected to the MCU by the
SPI bus and also uses the DMA mechanism. Another nRF connected to the PC is used to
communicate with the quadcopter. The transceivers are used in order to read telemetry data
and for piloting the UAV using an Xbox controller connected to the PC.

![](https://github.com/Cyberdom123/quadcopter---documentation/tree/main/img/dsp_sch.jpg)

To keep the UAV stable it is essential to keep it at a set (stable) angle during movement.
For instance, when we want the drone to fly up vertically, the roll and pitch angle have to be
set to 0 degrees. The IMU supplies the data about displacement and angular velocity and a
suitable mathematical model processes the data in order to estimate the angles.
Finally the integration of data from both sensors with the help of filtration algorithms,
such as complementary filter or Kalman filter allows us to gain precise information about
the angles and for effective stabilisation of the quadcopter during flight. In our project we
used the Kalman filter. It is an aimed at estimating the actual state of the system based
on the observer measurements with Gaussian noise. The algorithm works in two main steps:
estimation of the state of the system based on previous state and correcting it during the next
measurement. Kalman filter uses the mathematical model and data about Gaussian noise in
order to minimalise the estimates and measurement unceirtanties.
The angle estimates and yaw angular velocity is fed into the regulation system consisitng
of three PID regulators for every quantity. The PID regulator works by minimising the difference
between the actual state of the system and a desirable reference state. It contains three
components: proportional (P), which reacts proportionally to current deviateion, integral (I),
which accumulates the deviation as time passes and differential (D), which takes into account
the rate of change of the difference. The output of the PIDs is a duty cycle fed into a signal
mixer, which sets the duty cycle for each of the motors.

More detailed documentation has been placed in the project repository.

## Part list

- Frame: https://www.thingiverse.com/thing:1604440
- 7mm Motor Holder: https://www.thingiverse.com/thing:1675767
- 7x20mm 3.7V DC 46500RPM Motors CCW
- 7x20mm 3.7V DC 46500RPM Motors CW
- Propellers: 55mm
- Transceiver: 2.4GHz NRF24L01
- LiPo Battery: 520mAh 25C 12g Dualsky
- Gyroscope/Accelerometer: MPU6050
- Pressure Sensor: BMP280
- STM32-FC Board (located in the hardware folder)

![](https://github.com/Cyberdom123/quadcopter---documentation/tree/main/img/pcb.png)
