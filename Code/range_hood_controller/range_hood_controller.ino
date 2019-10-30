
/*
   for Arduino Uno
   Stove Top Fan control system
   Features:
   - Hands free fan control
   - Timer
   - Light control
   - Temp and humidity sensor
*/

// Set variables



int irLeft = 4;
int irRight = 8;

int ch1 = 9;
int ch2 = 10;
int ch3 = 11;
int ch4 = 12;

int handOnEnd = 0;
unsigned long handOnLeftStart = 0;
unsigned long handOnRightStart = 0;
unsigned long handOnLeftTime = 0;
unsigned long handOnRightTime = 0;
float tempTrigger = 0;
unsigned long handOnLeftEnd = 0;
unsigned long handOnRightEnd = 0;
unsigned long handOnLeftTotal;
unsigned long handOnRightTotal;
unsigned long lightsLastToggleTime;
unsigned long fanEndTime;
const long minute = 1000 * 60;
int fanState = 0;
boolean fanIsOn = false;
boolean handsOffReset = true;
boolean lightsOn = false;
unsigned long lastMillis;

unsigned long currentMillis;
unsigned long lastTempCheck;

// Settings
unsigned long fanTimer = 10 * 60000; // Set timer to shutdown fan automatically
long handOnTimeStep = 400; // How long hand needs to stay before toggling lights (ms)
float humidityTrigger = 90; // Trigger level for humidity to start fans
float heatTriggerMultiplier = 1.02; // How many procents temperature needs to raise in 5s to trigger fan
boolean HeatTriggerOn = false;


void setup() {
  Serial.begin(9600);
  Serial.println("System started.");

  Serial.println(fanTimer);
  pinMode(ch1, OUTPUT);
  pinMode(ch2, OUTPUT);
  pinMode(ch3, OUTPUT);
  pinMode(ch4, OUTPUT);

  pinMode(irLeft, INPUT);
  pinMode(irRight, INPUT);
  digitalWrite(ch1, LOW);
  digitalWrite(ch2, LOW);
  digitalWrite(ch3, LOW);
  digitalWrite(ch4, LOW);

}


// To control the fan we will cycle speeds 1 - 2 - 3 - 0 - 1 etc.

void cycleFanSpeed() {
  int i = (fanState + 1) % 2;
  setFanSpeed(i);

}
void cycleFanSpeedReverse() {
  int i = (fanState + 3) % 4;

  setFanSpeed(i);
}
void setFanSpeed(int s) {
  Serial.print("Setting fan speed to ");
  Serial.println(s);
  if (s == 0) {
    fanEndTime = 0;
  } else {
    fanEndTime = currentMillis + fanTimer;

  }
  fanState = s;
  if (s == 1) {
    digitalWrite(ch1, HIGH);
    digitalWrite(ch2, LOW);
    digitalWrite(ch3, LOW);
  } else if (s == 2) {
    digitalWrite(ch1, LOW);
    digitalWrite(ch2, HIGH);
    digitalWrite(ch3, LOW);
  }
  else if (s == 3) {
    digitalWrite(ch1, LOW);
    digitalWrite(ch2, LOW);
    digitalWrite(ch3, HIGH);
  }
  else {
    digitalWrite(ch1, LOW);
    digitalWrite(ch2, LOW);
    digitalWrite(ch3, LOW);
  }

}
boolean readLeft = true;
void loop() {

  currentMillis = millis();

  //Serial.println("LEFT");
  if (digitalRead(irLeft) == LOW) {
    // If left sensor is on record start time or if already started, hand on time
    if (handOnLeftStart == 0)  {
      handOnLeftStart = currentMillis;
    }
    handOnLeftTime = currentMillis - handOnLeftStart;


  } else {
    if (handOnLeftStart != 0) {
      handOnLeftEnd = currentMillis;
      handOnLeftTotal = handOnLeftEnd - handOnLeftStart;
    }

    handOnLeftStart = 0;
  }
  if (digitalRead(irRight) == LOW) {
    if (handOnRightStart == 0) {
      handOnRightStart = currentMillis;
    }
    handOnRightTime = currentMillis - handOnRightStart;


  } else {
    if (handOnRightStart != 0) {
      handOnRightEnd = currentMillis;
      handOnRightTotal = handOnRightEnd - handOnRightStart;
    }
    handOnRightStart = 0;


  }



  if (digitalRead(irLeft) == LOW && digitalRead(irRight) == LOW) {

    if (handOnLeftTime > handOnTimeStep && handOnRightTime > handOnTimeStep) {
      if (handsOffReset == true) {
        toggleLights();
        handOnLeftTime = 0;
        handOnRightTime = 0;
        handsOffReset = false;
        handOnRightEnd = 0;
        handOnLeftEnd = 0;
        handOnLeftStart = 0;
        handOnRightStart = 0;
        //lightsLastToggleTime = currentMillis;
        while (digitalRead(irLeft) == LOW && digitalRead(irRight == LOW)) {
          delay(100);
          //Serial.println("Waiting for lift off...");
        }
        //delay(600);
      }


    }
  }

  if  (handsOffReset == true && handOnLeftEnd != 0) {


    if (handOnLeftTime > 5 && handOnLeftTime < 200) {
      cycleFanSpeed();
      handOnLeftTime = 0;
      handOnRightTime = 0;
      handsOffReset = false;
      handOnRightEnd = 0;
      handOnLeftEnd = 0;
      handOnLeftStart = 0;
      handOnRightStart = 0;
      delay(600);

    }
  }

  if (digitalRead(irLeft) == HIGH && digitalRead(irRight) == HIGH ) {
    if (handsOffReset == false) {
      lightsLastToggleTime = currentMillis;
    }
    handsOffReset = true;
  }



  lastMillis = currentMillis;


  // timer for fan
  if ((currentMillis > fanEndTime)  && fanEndTime > 1)  {

    Serial.print("Timer turns off fan, fanendtime:");

    Serial.print(fanEndTime);

    Serial.print(" / ");
    Serial.println(currentMillis);

    setFanSpeed(0);

  }



}

void toggleLights() {

  if (lightsOn == false) {
    Serial.println("Toggle Lights ON");
    digitalWrite(ch4, HIGH);
    lightsOn = true;
  } else {
    Serial.println("Toggle Lights OFF");
    digitalWrite(ch4, LOW);
    lightsOn = false;
  }
}
