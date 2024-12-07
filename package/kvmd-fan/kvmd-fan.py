#!/usr/bin/env python
# encoding: utf-8

import RPi.GPIO
import time

#settings
fan_gpio=12 # Fan GPIO pin number (default: 12)
idle_speed = 0 # Fan speed when under min_temp(IDLE) (min: 0, max: 100)
min_temp = 60 # Fan starting temperature 
max_temp = 65 # Fan max speed temperature (max: 65)

#define GPIO
RPi.GPIO.setwarnings(False)
RPi.GPIO.setmode(RPi.GPIO.BCM)
RPi.GPIO.setup(fan_gpio, RPi.GPIO.OUT)
pwm = RPi.GPIO.PWM(fan_gpio,100)
running = False

#running code
try:
	while True:
		tmpFile = open( '/sys/class/thermal/thermal_zone0/temp' )
		cpu_temp = int(tmpFile.read())/1000
		tmpFile.close()
		print("Temp:",cpu_temp)
		if (cpu_temp >= max_temp) or (cpu_temp > min_temp): 
			if(not running): #if fan not running, start fan
				print("PWM_Start")
				pwm.start(0)
				running = True
			if (cpu_temp >= max_temp): #For safety, temp exceeds 60 degrees, the fan speed is always set to 100%.
				set_speed = 100
			else:
				temp_speed = int((cpu_temp - min_temp) / (max_temp - min_temp) * 100) # set 0~100% for min and max temp ranges
				set_speed = min(max(idle_speed, temp_speed), 100) # make speed not over 100 
			print("set_speed:",set_speed,"%") 
			pwm.ChangeDutyCycle(set_speed)
		else :
			if(running):
				running = False
				pwm.stop()
				print("PWM_Stop")

		time.sleep(5)
				
except KeyboardInterrupt:
		pass
pwm.stop()
