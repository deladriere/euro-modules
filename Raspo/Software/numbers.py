#!/usr/bin/env python

# Author: Jean-Luc Deladriere
# Description: 
# -p Adjusts the pitch in a range of 0 to 99. The default is 50.
#-s <integer>
#Sets the speed in words-per-minute (approximate values for the default English voice, others may differ slightly). The default value is 175. I generally use a faster speed of 260. The lower limit is 80. There is no upper limit, but about 500 is probably a practical maximum.


import subprocess,time,os,signal
from random import randint
import spidev

# Open SPI bus
spi = spidev.SpiDev()
spi.open(0,0)
 
# Function to read SPI data from MCP3008 chip
# Channel must be an integer 0-7
def ReadChannel(channel):
  adc = spi.xfer2([1,(8+channel)<<4,0])
  data = ((adc[1]&3) << 8) + adc[2]
  return data

def rescale(val, in_min, in_max, out_min, out_max):
    return out_min + (val - in_min) * ((out_max - out_min) / (in_max - in_min))




while True:
	subprocess.call('clear')
	#time.sleep(0.8) # time beetween blocks
	text=""
	for x in xrange(1,6):
		text=text+" "+str(randint(0,9))
	text=text+'.'
	print 'text =' ,
	print text	
	level= ReadChannel(0)
	#print str(level)
	level = int(rescale(level,0.0,1023.0,0,99.0))
	print str(level)
	cmd='espeak -vde+f5 -g10 -s100 -p'+str(level)+' "' + text +'" --stdout | sox  -t wav - -d phaser flanger pitch 400 tremolo 500 rate 8k overdrive 25'
	pro = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid) 
	#talk=subprocess.Popen('espeak -vde+m3 -s80 '+ text + ' -g20 -p0   --stdout | sox -t wav - -d  ', shell=True) # non blocking vs Popen call ? enlever shell = true
	#talk=subprocess.Popen('speak -vde+f5 -g10 -s100 -p50 "' + text +'" --stdout | sox  -t wav - -d phaser flanger pitch 400 tremolo 500 rate 8k overdrive', shell=True) # nice !
	#subprocess.call('flite -voice kal16 "' + text +'"  | sox  -t wav - -d phaser flanger pitch 400 tremolo 500 rate 8k overdrive', shell=True) # nice !
	time.sleep(5)
	os.killpg(pro.pid, signal.SIGTERM)