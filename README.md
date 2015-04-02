# EComp-Mouse
Programming a PIC to interact with an optical sensor allowing it to be used as a computer mouse.

This project was developed for the Computer Electronics course (1ºSemester - 2014/2015). To communicate between the PIC and the optical sensor the group adapted the SPI - Serial Peripheral Interface - from full duplex to half duplex. The PIC was used to control the data from the optical sensor by reading several registers that provide us different information as the new relative position to the previous, if it was changed, and the depth of the pixel read (in scale of 8bits - 265 levels of grey).

A user interface was also developed allowing the user to change the resolution of the optical sensor, check value of registers and use the sensor as a computer mouse. 

In a final approach we were able to use the sensor as a small resolution camera.

Project developed with João Bauto and Robert Serrão.
