Arduino based Talkie Eurorack Module 
This arduino based module works thanks to a clever library : Talkie from https://github.com/going-digital/Talkie 

Jean-Luc Deladrière, Hanabi SPRL 2006-2014. Except where otherwise noted, content on this site is licensed under a cc-by-sa 3.0 license.

#Functions
##Mode
For the moment I have added 4 modes or sound banks :

numbers (from minus 9 to 9)
voltmeter (reading the CV voltage)
frequencemeter (fake mode just saying "Hertz" instead of "Volts")
alphabet
nato alphabet
I plan to add a 5th mode with a large vocabulary and a 6th mode with weird sounds

##CV

CV signal change the words or phrase to be said

##Gate

Pressing the button start the complete sound in trigger mode. (regardless of it's length) 
If the trigger switch is off, the gate will start the sound and hold it as long as the gate level stays up. Very useful to create crazy rhythms.

If the cable is plugged in, the gate is triggered via an external signal

##Trigger

Choose between trigger mode or gated mode

##Bend

If bend is on, the bend pot ... distords the sound.

##Speed

Change the speed

##Pitch

Change the pitch

## Todo.todo
* publish schematic
* publish pcb
* publish panel inkscape & openscad code
* add picture of panel & pcb
* merge library in main code 
* calibrate full cv scale
* add R in series with entries for protection


more info here : http://elek101.blogspot.be/2014/10/talkie-eurorack-module-part-1-concept.html




