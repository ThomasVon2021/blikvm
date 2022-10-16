#!/usr/bin/env python
# encoding: utf-8

import RPi.GPIO
import time
RPi.GPIO.setwarnings(False)
RPi.GPIO.setmode(RPi.GPIO.BCM)
RPi.GPIO.setup(12, RPi.GPIO.OUT)
pwm = RPi.GPIO.PWM(12,100)
RPi.GPIO.setwarnings(False)


speed = 0
prv_temp = 0
target_temp = 55
target_max_speed = 80
fan_p = 10
fan_flag = False

try:


		while True:
		
				tmpFile = open( '/sys/class/thermal/thermal_zone0/temp' )
				cpu_temp = int(tmpFile.read())/1000
				tmpFile.close()
				print("temp:%d",cpu_temp)
				if (cpu_temp>=60) or (prv_temp > target_temp and fan_flag == True) :
					fan_flag = True
					if prv_temp<60 :
						pwm.start(0)
						pwm.ChangeDutyCycle(100)
						time.sleep(.1)
					temp_gap = cpu_temp - target_temp
					speed = fan_p * temp_gap
					#set_speed = min( speed , target_max_speed )
					set_speed = 85
					print("set_speed:%d",set_speed)
					pwm.ChangeDutyCycle(set_speed)
				else :
					fan_flag = False
					pwm.stop()
				prv_temp = cpu_temp
				
				time.sleep(5)
				
except KeyboardInterrupt:
		pass
pwm.stop()
