#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into GPIOpin 2 (i.e.D4) on the NodeMCU Esp8266
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Assign the unique addresses of your 1-Wire temp sensors
DeviceAddress insideThermometer = { 0x28, 0xFF, 0x2B, 0x09, 0x16, 0x15, 0x03, 0x39 };
 
float temp_c=0.0;          // Values read from Temperature sensor

void printTemperature(DeviceAddress deviceAddress)
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

void setup(void)
{
  // You can open the Arduino IDE Serial Monitor window to see what the code is doing
  Serial.begin(115200);  // Serial connection from ESP-01 via 3.3v console cable
   // Start up the library
  sensors.begin();
  // set the resolution to 10 bit (good enough?)
  sensors.setResolution(insideThermometer, 12);
}
 
void loop(void)
{
    printTemperature(insideThermometer);
    delay(5000);
} 
 
