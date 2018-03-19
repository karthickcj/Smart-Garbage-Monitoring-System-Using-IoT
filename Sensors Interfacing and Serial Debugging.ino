#include <OneWire.h>
#include <DallasTemperature.h>

/********************************************Ultrasonic sensors******************************************************
Esp8266 GPIO pin 12 connected to Echo Pin of HC-SR04
Esp8266 GPIO pin 14 connected to Trigger pin of HC-SR04

Since echo output is 5v 
We have coonected a 2.2K resistor to it then a 4.7K Resistor 
4.7K resistor is grounded
The signal is taken out from the junction of the two resistor
*********************************************************************************************************************/
#define echoPin 12 // Echo Pin
#define trigPin 14 // Trigger Pin

long duration, distance; // Duration used to calculate distance



/********************************************DS18B20 Digital Temperature Sensor***************************************
<OneWire.h> and  <DallasTemperature.h> Library required for DS18B20
DS18B20 Data wire is plugged into GPIOpin 2 on the Esp8266
*********************************************************************************************************************/
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Assign the unique addresses of your 1-Wire temp sensors
DeviceAddress insideThermometer = { 0x28, 0xFF, 0x2B, 0x09, 0x16, 0x15, 0x03, 0x39 };
 
float temp_c=0.0;          // Values read from Temperature sensor


void getTemperature(DeviceAddress deviceAddress)
{
    sensors.requestTemperatures();
    temp_c = sensors.getTempC(deviceAddress);
    if (temp_c == -127.00) {
    temp_c=0.0;
    Serial.println("Error getting temperature");
    } 
    else {
    Serial.print("Inside temperature is: ");
    Serial.print(temp_c);
    Serial.print( char(176)); 
    Serial.println("C.");   }
}

void getGarbageLevel(void)
{
/********************************************Ultrasonic sensors******************************************************
  Speed of sound = 340 m/s
  Now,         1 metre = 100 centimetre            and              1 seconds = 1000000 microseconds
  Speed of sound = 340 * 100 cm/(1000000 microseconds) = 0.034 cm per us = (1/29.412) cm per us 
  The Ultrasonic burst travels out & back.So to we have to divide the time the echo pin was high by 2
  Distance = (Time echo pin was high/2) * speed of sound
           = (Time echo pin was high/2) * (1/29.412)
           = (Time echo pin was high/58.82) 
*********************************************************************************************************************/  

digitalWrite(trigPin, LOW);      // Give a short LOW pulse beforehand to ensure a clean HIGH pulse
delayMicroseconds(2);

digitalWrite(trigPin, HIGH);     // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

/**************************************************************
Read the signal from the sensor (Echo pin): a HIGH pulse whose duration is the time (in microseconds) from the sending
of the ping to the reception of its echo off of an object.

pulseIn function reads a pulse (either HIGH or LOW) on a pin. For example, if value is HIGH, pulseIn() waits for the pin 
to go HIGH, starts timing, then waits for the pin to go LOW and stops timing. Returns the length of the pulse in microseconds 
or 0 if no complete pulse was received within the timeout.
Works on pulses from 10 microseconds to 3 minutes in length. Please also note that if the pin is already high when the function
is called, it will wait for the pin to go LOW and then HIGH before it starts counting.
**************************************************************/
duration = pulseIn(echoPin, HIGH);

//Calculate the distance (in cm) based on the speed of sound.
distance = duration/58.82;
Serial.print("Garbage Level: ");
Serial.print(distance);  
Serial.println("cm");
}

void setup()
{
Serial.begin (115200);
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
 // Start up the library
sensors.begin();
// set the resolution to 10 bit (good enough?)
sensors.setResolution(insideThermometer, 12);
}
 
void loop()
{
getGarbageLevel();

getTemperature(insideThermometer);

//Delay 50ms before next reading.
delay(2500);
}
