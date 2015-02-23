# LCD Display for a bench power supply

This program measures the current and voltage from a bench power supply
using 4 of the ADC channels on an AVR (an ATmega328p in my case) and 
displays the readings on a 2x16 character LCD based around the
Hitachi HD44780 LCD controller.

The display driver was implemented with only the most basic functions
of the device (setup, write to screen, and move around some). To save
on pins, 4-byte mode is selected in the initialization of the display.

In the circuit implementation, voltages were stepped down to 0-5V range
using op amps. The current measurements are based on the output of a 
difference amplifier taking input from either side of a test resistor.
