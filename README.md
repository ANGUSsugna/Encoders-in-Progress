# Encoders-in-Progress
Work in progress on the encoders sending and receiving

Hi Adam,

This is the code I've been working on for the controls.
It consists of a simplified array (5 rows by 8 columns).
The controls are assured by 2 rotary encoders, rotary1 and rotary2.
rotary1 controls the y axis position, rotary 2 controls the x position.
the "cursor" is the flickering of the sides of the square the X and Y positions
designate as they get drawn and erased. This is not very elegant but the sides have to be erased 
and I cannot use any delays (or millis... I think) because of the encoders. 
Pressing the switch on encoder1 (OWNSW1) leaves a little crosshair. 
The code for the switches is the next order of business. It "works" in that I've been able to get the 
crosshair to appear on the OTHER screen over wifi - OPP(onent's) but it is not working well because of the code
of the encoder library I'm using which appears to only record when the switch has been "released" and remains
in that "released" state thereafter.

