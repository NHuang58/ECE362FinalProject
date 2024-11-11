This is a Mini Piano using the STM32 Microcontroller for the ECE 362 Final Project

Team Members:
-Amir Mokhtarpour\n
-Nathan Huang\n
-Jackson Hu\n
-Nolan Jones\n

Full Project Proposal and Description Found Below:
Group Name: The Piano Guys


Comments:


Approved, contingent on the following below.  You are team 57.

You have not explicitly mentioned it, but I assume you mean the TFT LCD 2.2in color 240x320 display, and not the OLED you have used in lab 6 (which would probably be impossible for displaying notes as you've described anyway).

Your "visual feedback" PWM LED must be in sync with the notes being played, and cannot be a randomly pulsing LED.


Project Proposal:


Project Description: In this project, we aim to design a 12-note piano keyboard with the ability to set the octave on the keypad and record the played notes.
The four main objectives are:
- playing different notes with the DAC
- controlling the volume using the ADC and controlling the log and the octaves using the keypad
- Utilizing LCD, SD card and serial monitor for recording the played notes
- Utilizing PWM for the visual feedback


Main Features: 1. playing 7 white and 5 black notes of one octave piano with the push pin switches and DAC.
2. utilizing an LCD to show the last played note
3. utilizing an SD card to save the played note on a text file.
4. utilizing the keypad to change the octave, delete the played note , reset the log, and save the log on the sd card.
5. utilizing ADC to adjust the volume using a potentiometer
6. utilizing pwm to make visual effects on an RGB led.
7. utilizing the Seven segment to display the frequency played of the last pushed note
8. utilizing the serial port to log the pushed button + frequency of each notes
9. utilizing an external amplifier for the DAC to utilize a speaker



External Interfaces: SPI for the SD reader
UART for the serial monitor
Analog for the Speaker


Internal Peripherals: GPIO for the keys and the keypad
Timers for the kaypad
DAC for playing the notes
ADC for reading the potentiometer to set the volume
DMA for setting the value on the Seven segments
PWM for visuals on the RGB LED


Timeline: By 11/3:
- Complete push pin switch configuration for 12 notes (7 white, 5 black)
- Finish DAC initialization code for audio output
- Complete ADC initialization code for potentiometer
- Implement volume control function using ADC readings
- Complete LCD driver code to display last played note

By 11/9:
- Finish SD card initialization code
- Complete function to write a single line of text to SD card
- Complete keypad driver code
- Implement octave changing function
- Implement function to delete last played note from memory

11/17:
- Finish PWM configuration code for RGB LED and Implement visual effect functions for RGB LED
- Complete seven-segment display driver code
- Implement function to calculate and display frequency of last played note
- Complete UART initialization code and Implement function to log pushed button and frequency via serial port

11/21:
- Finish external amplifier integration code
- Complete speaker output testing
- Finish all integration testing
- Final Test

11/22:
- Demo day ! :)
