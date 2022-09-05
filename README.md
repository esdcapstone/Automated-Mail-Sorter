# Automated-Mail-Sorter
Sort mail envelopes according to handwritten alpha-2 province codes and display count on a webapp.

# Hardware
- STM32F411RE
- Raspberry Pi 4B
- Pi Camera
- HC04

# Project Components
- Embedded: STM32F411RE used to rotate stepper and servo motors for running a carboard conveyor belt.
- Webapp: To display logistics such as number of counts for each province on a client (such as a web browser).
- CV: Service running on a Raspberry Pi 4B to capture images of envelopes and recoginize handwritten characters.

# Quick Demo
![Single Letter Sort](demo.gif)

# Purpose
This project was worked on by students from Embedded Systems Development program at Conestoga College as a part of their final semester.