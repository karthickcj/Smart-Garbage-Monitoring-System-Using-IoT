int trigPin = 14;    //Trig - Ultrasonic sensor connected to NodeMCU GPIO14 i.e.D5

int echoPin = 12;    //Echo - Ultrasonic sensor connected to NodeMCU GPIO12 i.e.D6

float garb_cm=0.0;     // Values read from Ultrasonic sensor

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
  
  Serial.print(garb_cm);
  Serial.print(" cm");
  Serial.println();
  garb_cm = map(garb_cm, 0, 400, 0, 100);
}



void setup()
{
    Serial.begin(115200);
    Serial.println("");
    Serial.println("-------------------------------Garbage Level-------------------------------------");

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

void loop()
{
garbage_level();
delay(5000);
}
