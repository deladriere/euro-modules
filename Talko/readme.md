Arduino based Talkie Eurorack Module 
Have your modular speak out the CV
This arduino based module works thanks to a clever library : Talkie from https://github.com/going-digital/Talkie 

Jean-Luc Deladrière, Hanabi SPRL 2006-2014. Except where otherwise noted, content on this site is licensed under a cc-by-sa 3.0 license.

Numbers 

#Functions
##Bank
For the moment I have added 4 sound banks :
0:digits (from 0 to 9) male voice
1:digits (from 0 to 9) female voice
2:big numbers (from 0 to 9999999)
3:voltmeter (reading the CV voltage)
4:frequencemeter (just saying "Hertz" instead of "Volts")
5:Spell the alphabet
6:nato alphabet

I plan to add a 7th bank with a large vocabulary and a 8th bank with weird sounds
random mode


##CV

CV signal change the words or phrases to be said

##Gate

Level high start the complete sound in trigger mode. (regardless of it's length) 
If the trigger switch is off, the gate will start the sound and hold it as long as the gate level stays up. Very useful to create crazy rhythms.


##Trigger

Choose between trigger mode or gated mode

##Bend

If bend is on, the bend pot ... distords the sound.

##Speed

Change the speed

##Pitch

Change the pitch

## Todo.todo
* publish schematic @done(2014-12-30)
* publish pcb @done(2014-12-30)
* add picture of panel & pcb
* merge library in main code 
* add R/shottky in  with entries for protection @done(2014-11-30)
* add mousers ref to each part (mouser cart : link)
* Add female voice @done(2014-12-30)


more info here : http://elek101.blogspot.be/2014/10/talkie-eurorack-module-part-1-concept.html




