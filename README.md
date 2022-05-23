# pwm_fan_control

Very simple setup to control my EC fan.  
The EPS8266 uses PWM to send out 0-10V to the fan's input.  
It first connects to a wifi network and the uses MQTT for input/output.  
  
Components:  
https://opencircuit.shop/product/pwm-to-analog-0-10v-converter-module  
https://www.wemos.cc/en/latest/d1/d1_mini.html  
https://www.amazon.com/LM2577-Booster-Module-Supply-Adjustable/dp/B07LHCT4MB  
  
It does seem that the 0-10V converter only outputs 9.34V so it's not exactly ideal.  
But for my type of application it doesn't really matter that much.  

![alt text](https://github.com/BartLamboo/pwn_fan_control/blob/main/overview.png?raw=true)
