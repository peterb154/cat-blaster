// pins
const int buttonPin = 12;  // Delay button
const int buzzPin = 11;
const int trigPin = 10; // TRIG Pin on HC-SR04 Ultrasonic Module  
const int echoPin = 9; // ECHO Pin on HC-SR04 Ultrasonic Module 
const int relay = 8;
const int greenLed = 7;
const int blueLed = 6;
const int redLed = 5; 

// config
const int maxBlasts = 12; //Change this to how many blasts your air tank is good for
const int airBurst = 75; // How long solenoid fires for
const int zone1 = 30; // Getting this close (inches) to sensor triggers slow LED blink
const int zone2 = 20; // Getting this close (inches)to sensor triggers faster LED blink and buzzer
const int zone3 = 12; // At this distance (inches) there is a steady warning tone and then the solenoid fires
const int noObject = 467; // the value where nothing in range

// vars
float distance;
int blastCount; // Used to keep track of how many times air is fired so you know when to refill tank
int blinkRate;
int buttonState; // variable to store button state

void setup() {
  Serial.begin(9600);
  blastCount = 0; // Everytime arduino is reset to keep track of air bursts
  pinMode(buttonPin, INPUT);
  pinMode(buzzPin, OUTPUT);
  digitalWrite(buzzPin, HIGH);
  digitalWrite(buttonPin, HIGH); // turn on pull up resistors. Wire button so that press shorts pin to ground
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT); 
}

void loop() {
  noTone(buzzPin);
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(blueLed, LOW);

  buttonState = digitalRead(buttonPin);   // read the button state and store
  while (buttonState == LOW) {
    // Pressing the button will not allow solenoid to fire 
    // Handy for allowing you to get into "banned" area
    digitalWrite(greenLed, HIGH);
    digitalWrite(redLed, HIGH);
    digitalWrite(blueLed, HIGH);
    delay(500);
//    digitalWrite(greenLed, LOW); // Doesn't actually blink but was needed to stay in this button pressed loop
    buttonState = digitalRead(buttonPin);
  }
  
  distance = getDistance();

  // Not in any zone
  if (distance == noObject) {
    blinkRate = 1000;
    digitalWrite(greenLed, HIGH); // armed 
    delay(blinkRate);
    digitalWrite(greenLed, LOW);
    delay(blinkRate);
  }
  else if (distance > zone1) {
    blinkRate = 100;
    digitalWrite(greenLed, HIGH); // armed  
    delay(blinkRate);
    digitalWrite(greenLed, LOW);
    delay(blinkRate);
  }

  // IN zone1
  else if (distance <= zone1 && distance > zone2) { // Somethng enters zone1 - Slow LED blink no beeping
     blinkRate = 500; 
     digitalWrite(redLed, HIGH);
     tone(buzzPin, 500);
     delay(blinkRate);
     noTone(buzzPin);
     digitalWrite(redLed, LOW);
     delay(blinkRate);
  }

  // IN zone 2
  else if (distance <= zone2 && distance > zone3){ //Something enters zone2 - Faster LED blink and beeping
    blinkRate = 250; 
    digitalWrite(redLed, HIGH);
    tone(buzzPin, 1000);
    delay(blinkRate);
    noTone(buzzPin);
    digitalWrite(redLed, LOW);
    delay(blinkRate);
  }

 // IN zone 3
 else if (distance <= zone3) { // Something enters zone3, first give a warning tone
    digitalWrite(redLed, HIGH); // Green LED Comes on
    tone(buzzPin, 2000);
    delay(1100); // give tone before air blast - increase delay for longer warning
    // noTone(buzzPin);
    // digitalWrite(redLed, LOW);
    
    // Now check again to see if something is still in zone 3
    distance = getDistance();
    while (distance <= zone3 ) {
      tone(buzzPin, 5000);
      digitalWrite(blueLed, HIGH);
      digitalWrite(relay, HIGH); // Solenoid Fires
      
      delay(airBurst); // How long Solenoid fires for
      blastCount ++; // Add 1 to the firing count
      
      digitalWrite(relay, LOW);
      digitalWrite(blueLed, LOW);
      tone(buzzPin, 2000);
      
      delay(1500);
      distance = getDistance();
    }
  }
  
  if (blastCount >= maxBlasts) {
    digitalWrite(redLed, HIGH); //Turns on Red LED when maxBlasts is reached
  }
}

// Read the HC-SR04 Ultrasonic Module, return distance in inches
long getDistance(){
  long duration, distance;
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/58.138) * .39; //Calculates distance in inches
  Serial.print(distance);
  Serial.print('\n');
  return distance;
}
