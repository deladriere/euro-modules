##Arduino based Talkie Eurorack Module 
**Have your modular speak out the CV**  

This arduino based module works thanks to a clever library : Talkie from https://github.com/going-digital/Talkie 


![Early Panel](https://raw.githubusercontent.com/deladriere/euro-modules/master/Talko/Img/TALKO_1_1.png)


####Intro 
Realtime lpc synthesis
no sampling 
realiable to produce intelligible voice or ... not : your choice
tempo aware via gate trigger

#### Technical details

format: eurorack
width:	10hp
depth: mm (“skiff friendly”)
power: + 12V:  65 ma

####Functions
####Bank Pot
For the moment I have added 4 sound banks :
0: digits (from 0 to 9) male voice  
1: digits (from 0 to 9) female voice  
2: Spell the alphabet  
3: nato alphabet  
4: vocabulary  
5: big numbers  
6: voltmeter (reading the CV voltage)  
7: frequencemeter (just saying "Hertz" instead of "Volts")  
8: counter (each gate trigger increments the counter while any cv change resets it)  
9: number radio (each gate trigger a 4 number sequence while any cv trigger a new "transmission")  
10: time since cv reset  (each gate triggers saying the time in minutes/seconds while any cv change resets the clock to zero)  
11: Random vocabulary


####Sound jack & pot

CV signal choose the words or phrases to be said

####Gate jack

Level high start the complete sound in trigger mode. (regardless of it's length)  
If the trigger switch is off, the gate will start the sound and hold it as long as the gate level stays up. Very useful to create crazy rhythms.


####Trigger Switch

Choose between trigger mode or loop mode
In trigger mode the speech has the priority will complete before starting again on a new gate going high signal.  
In loop mode the gate has the piority and the speech can be shopped by a low gate level or can be looped with a high gate level.  

#####Bend Switch

If bend is on, the bend pot ... well, distords the sound.

####Speed jack & pot

Change the speed of the voice

####Pitch jack & pot

Change the pitch of the voice

### Progress

July 2015 : 

- assembled pcb 1.1 with smd's
- printed panel

May 2015 : version 1.1

- removed audio filter 
- fix grounds errors
- swap Pitch & Speed pots 
- change switches 
- increase LED resistors value

January 2015
- first pcb version

October 2014 
- very early prototyping
- breadboard version


### [Building the module](https://github.com/deladriere/euro-modules/wiki/Assemble-Talko)
### [Uploading the code](https://github.com/deladriere/euro-modules/wiki/Install-Talko)


### Todo

- merge library in main code 


more info here : http://elek101.blogspot.be/2014/10/talkie-eurorack-module-part-1-concept.html

http://elek101.blogspot.be/2014/12/talkie-eurorack-module-part-2-schematic.html

####Licence

Jean-Luc Deladrière, Hanabi SPRL 2006-2015.  
Except where otherwise noted, content on this site is licensed under a cc-by-sa 3.0 license.


