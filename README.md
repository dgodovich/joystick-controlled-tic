# Using a Joystick to Control a Motor

This code uses a [Teensy 3.2 microcontroller](https://www.pjrc.com/teensy/teensy31.html) as a base to recieve analog inputs from a 2 axis [joystick](https://www.adafruit.com/product/512) and send them via I2C communication to [Pololu Tic motor drivers](https://www.pololu.com/product/3130).

## Code
The code is based on code available on the Pololu github page [here](https://github.com/pololu/tic-arduino/tree/master/examples). It is slightly altered to allow for analog input from the 2-axis joystick. 

In order to run this code, you must have the latest version of the Arduino software avaiable [here](https://www.arduino.cc/en/Main/Software) and the latest version of the Teensyduino software add-on avaiable [here](https://www.pjrc.com/teensy/td_download.html). In addition, the Tic Arduino library must be installed. A description of how to install it can be found [here](https://github.com/pololu/tic-arduino/blob/master/README.md).

## Circuit
The entire circuit fits on a half-size breadboard. Complete circuit diagram coming soon.