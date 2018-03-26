#include <ArduinoJson.h>
#include <BoodskapCommunicator.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define CONFIG_SIZE 512
#define REPORT_INTERVAL 10000 //10 Seconds
#define MESSAGE_ID 1000 //Message defined in the platform //Bin1
//#define MESSAGE_ID 2000  //Bin2
//#define MESSAGE_ID 3000  //Bin3

int trigPin = 14;    //Trig - Ultrasonic sensor connected to NodeMCU GPIO14 i.e.D5
int echoPin = 12;    //Echo - Ultrasonic sensor connected to NodeMCU GPIO12 i.e.D6
float garb_cm=0.0;     // Values read from Ultrasonic sensor


// Data wire is plugged into GPIOpin 2 (i.e.D4) on the NodeMCU Esp8266
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Assign the unique addresses of your 1-Wire temp sensors
DeviceAddress insideThermometer = { 0x28, 0xFF, 0x2B, 0x09, 0x16, 0x15, 0x03, 0x39 };
 
float temp_c=0.0;          // Values read from Temperature sensor

/**
 * ***** PLEASE CHANGE THE BELOW SETTINGS MATCHING YOUR ENVIRONMENT *****
*/
#define DEF_WIFI_SSID "devil"  //Your WiFi SSID
#define DEF_WIFI_PSK "123456" //Your WiFi password
#define DEF_DOMAIN_KEY "OXBLYXAPFK" //your DOMAIN Key
#define DEF_API_KEY "O9Eo7ZuBuXJt" //Your API Key
#define DEF_DEVICE_MODEL "BSKP-GSR" //Your device model
#define DEF_FIRMWARE_VER "1.0.0" //Your firmware version

BoodskapTransceiver Boodskap(UDP); //MQTT, UDP, HTTP
uint32_t lastReport = 0;

void sendReading();

void garbage_level()
{
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);

  //Start sensor reading
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  long duration = pulseIn(echoPin, HIGH);
 
  // convert the time into a distance
  garb_cm = (duration/2) / 29.1;
  Serial.print(" Garbage Level: ");
  Serial.print(garb_cm);
  Serial.print(" cm");
  Serial.println();
  //garb_cm = map(garb_cm, 0, 400, 0, 100);
}

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
    Serial.println("C");   }
}

void setup() {

  Serial.begin(115200);
  Serial.println("");
  Serial.println("-------------------------------Garbage Level-------------------------------------");
  sensors.begin();
  sensors.setResolution(insideThermometer, 12);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  StaticJsonBuffer<CONFIG_SIZE> buffer;
  JsonObject &config = buffer.createObject();

  config["ssid"] = DEF_WIFI_SSID;
  config["psk"] = DEF_WIFI_PSK;
  config["domain_key"] = DEF_DOMAIN_KEY;
  config["api_key"] = DEF_API_KEY;
  config["dev_model"] = DEF_DEVICE_MODEL;
  config["fw_ver"] = DEF_FIRMWARE_VER;
  config["dev_id"] = String("ESP8266-") + String(ESP.getChipId()); //Your unique device ID

  /**
     If you have setup your own Boodskap IoT Platform, then change the below settings matching your installation
     Leave it for default Boodskap IoT Cloud Instance
  */
  config["api_path"] = "https://api.boodskap.io"; //HTTP API Base Path Endpoint
  config["api_fp"] = "B9:01:85:CE:E3:48:5F:5E:E1:19:74:CC:47:A1:4A:63:26:B4:CB:32"; //In case of HTTPS enter your server fingerprint (https://www.grc.com/fingerprints.htm)
  config["udp_host"] = "udp.boodskap.io"; //UDP Server IP
  config["udp_port"] = 5555; //UDP Server Port
  config["mqtt_host"] = "mqtt.boodskap.io"; //MQTT Server IP
  config["mqtt_port"] = 1883; //MQTT Server Port
  config["heartbeat"] = 45; //seconds

  Boodskap.setup(config);
}

void loop() {

  Boodskap.loop();
  garbage_level();
  delay(5000);
  if ((millis() - lastReport) >= REPORT_INTERVAL) {
    sendReading();
    lastReport = millis();
  }
}

void sendReading() {

  StaticJsonBuffer<512> buffer;
  JsonObject &data = buffer.createObject();
  data["garbagelevel"] = garb_cm;
  data["temperature"] = temp_c;
  

  Boodskap.sendMessage(MESSAGE_ID, data);
}
