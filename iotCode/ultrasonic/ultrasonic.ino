const int trig = 14; //ultrasonic sensor
const int echo = 34; //ultrasonic sensor
long dura; //duration
float dist; //distance

void setup() {
  Serial.begin(115200); 
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  digitalWrite(trig, LOW);
  delayMicroseconds(5);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  dura = pulseIn(echo, HIGH);
  dist = (dura * 0.0343) / 2;

  Serial.print("Dist:"); 
  Serial.print(dist);
  Serial.print("Cm\n");
  
  delay(1000);
}