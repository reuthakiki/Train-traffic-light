const int mainGreenPin = 2;
const int mainYellowPin = 9;
const int mainRedPin = 8;

const int routineGreenPin = 4;
const int routineYellowPin = 3;

const int routineButton = 5;
const int emergencyButton = 6;

const int emergencyRedPin = 7;


int state = -1;  // the state increases
bool isEmergencyOn = false;

int brightness = 0;    // how bright the LED is
int fadeAmount = 15;    // how many points to fade the LED by
int timerRunning = 0 ;
int startTime = 0;
int endTime = 0;



void setup() {
  pinMode(routineButton, INPUT);
  pinMode(emergencyButton, INPUT);
  pinMode(mainGreenPin, OUTPUT);
  pinMode(mainYellowPin, OUTPUT);
  pinMode(mainRedPin, OUTPUT);
  pinMode(routineGreenPin, OUTPUT);
  pinMode(routineYellowPin, OUTPUT);
  pinMode(emergencyRedPin, OUTPUT);

  digitalWrite(mainGreenPin, HIGH);
  digitalWrite(mainYellowPin, HIGH);
  digitalWrite(mainRedPin, HIGH);
  digitalWrite(routineGreenPin, HIGH);
  digitalWrite(emergencyRedPin, HIGH);
  digitalWrite(routineYellowPin, HIGH);
  digitalWrite(routineButton, HIGH);
  digitalWrite(emergencyButton, HIGH);
  delay(2000);
  digitalWrite(mainGreenPin, LOW);
  digitalWrite(mainYellowPin, LOW);
  digitalWrite(mainRedPin, LOW);
  digitalWrite(routineGreenPin, LOW);
  digitalWrite(emergencyRedPin, LOW);
  digitalWrite(routineYellowPin, LOW);
}

void routineLight(int state) {
  switch (state) {
    case 0://stop
      {
        digitalWrite(routineYellowPin, HIGH);
        digitalWrite(mainRedPin, HIGH);
        digitalWrite(routineGreenPin, LOW);
        digitalWrite(mainGreenPin, LOW);
        delay(250);
        break;

      }
    case 1://drive slowly
      {
        digitalWrite(routineYellowPin, HIGH);
        digitalWrite(routineGreenPin, HIGH);
        digitalWrite(mainYellowPin, HIGH);
        digitalWrite(mainRedPin, LOW);
        delay(250);
        break;
      }
    case 2://drive
      {
        digitalWrite(routineGreenPin, HIGH);
        digitalWrite(mainGreenPin, HIGH);
        digitalWrite(routineYellowPin, LOW);
        digitalWrite(mainYellowPin, LOW);
        delay(250);
        break;
      }
  }
}

void loop() {


  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (digitalRead(emergencyButton) != HIGH) {
      digitalWrite(emergencyRedPin, HIGH);
      digitalWrite(mainGreenPin, LOW);
      digitalWrite(mainYellowPin, LOW);
      digitalWrite(mainRedPin, HIGH);
      digitalWrite(routineGreenPin, LOW);
      digitalWrite(routineYellowPin, HIGH);
    isEmergencyOn = true;
  }
    if (digitalRead(routineButton) != HIGH && !isEmergencyOn)
  {
    state++;
    state %= 3;
    routineLight(state);
  }
  
  else if (isEmergencyOn)
  {

    analogWrite(emergencyRedPin, brightness);

    // change the brightness for next time through the loop:
    brightness = brightness + fadeAmount;

    // reverse the direction of the fading at the ends of the fade:
    if (brightness <= 0 || brightness >= 255) {
      fadeAmount = -fadeAmount;
    }
    // wait for 30 milliseconds to see the dimming effect
    delay(20);
  }


  if (timerRunning == 0 && digitalRead(emergencyButton) == LOW) { // button pressed & timer not running already

    startTime = millis();
    timerRunning = 1;
  }
  if (timerRunning == 1 && digitalRead(emergencyButton) == HIGH) { // timer running, button released

    endTime = millis();
    timerRunning = 0;
    int duration = endTime - startTime;
    if (duration >= 2000) {
      isEmergencyOn = false;
      digitalWrite(emergencyRedPin, LOW);
      digitalWrite(mainRedPin, LOW);
      digitalWrite(routineYellowPin, LOW);
      routineLight(state);
    }

  }
}
