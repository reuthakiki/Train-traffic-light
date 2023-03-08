#define MSIZE 8
#define LOGOSIZE 43
#include <EEPROM.h>

//every byte in the array represent a column that needs to be lighted for the display of the logo
const byte logoTrain[] = {0x7c, 0x04, 0x04, 0x00,
                          0x7c, 0x44, 0x44, 0x00,
                          0x40,0x7c, 0x44, 0x44,0x00,
                          0x7c, 0x04,0x7c,0x40,  0x00,
                          0x0c, 0x00,
                          0x7e,0x4e, 0x48,0x7e,0x00,
                          0x7c,0x04, 0x04, 0x00,
                          0x24, 0x24,0x7e,0x00,
                          0x78,0x08,0x0e,0x00,
                          0x10,0x18, 0x28,0x14,0x18,0x08};  

//pin numbers for the lights and buttons
const int mainGreenPin = 2;
const int mainYellowPin = 9;
const int mainRedPin = 8;
const int routineGreenPin = 4;
const int routineYellowPin = 3;
const int emergencyRedPin = 5;
const int routineButton = 7;
const int emergencyButton = 6;

//pin numbers for the led matrix
int DIN = 13;
int CS = 12;
int CLK = 11;

int sensorPin = A0; // select the input pin for the potentiometer

//global variables
int currState;
int lastState = HIGH;
int state = -1;  // the state increases
bool isEmergencyOn = false;
bool isDriveSlowlyOn = false;
bool buttonReleased = false;
bool goodTemperature = true;
int logoIndex = 0;
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by
int startTime = 0;
int endTime = 0;
int addr = 0;
int trainSpeed = EEPROM.read(addr);
long duration = 0;
int readVal = 0;
double temp = 0;
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long previousDisplayMillis = 0; //will store the last time a column has been displayed on the matrix
unsigned long totalLogoDisplayTime = 4000;

void DisplayLogo()
{
  const long interval = totalLogoDisplayTime / LOGOSIZE;
  unsigned long currentMillis = millis();
  if (currentMillis - previousDisplayMillis >= interval) {
    //save the last time you blinked the LED
    previousDisplayMillis = currentMillis;
    clearMatrix();
    for (int i = 8; i > 0; i--)
    {
      if (logoIndex - i >= 0) {
        byte temp = logoTrain[logoIndex - i];
        writeColumn(i, temp);
      }
      else
      {
        writeColumn(i, 0x00);
      }
    }
    logoIndex++;
  }
}

void wrapperLogoDisplay()
{
  if (state == 2)
  {
    if (logoIndex > LOGOSIZE)
    {
      delay(1000); //wait one second
      logoIndex = 0;
    }
    DisplayLogo();
  }
}

void Blink(int ledPin, const long interval)
{
  bool ledState = digitalRead(ledPin);
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(routineButton, INPUT_PULLUP);
  pinMode(emergencyButton, INPUT_PULLUP);
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
  digitalWrite(mainGreenPin, LOW);

  digitalWrite(mainYellowPin, LOW);
  digitalWrite(mainRedPin, LOW);
  digitalWrite(routineGreenPin, LOW);
  digitalWrite(emergencyRedPin, LOW);
  digitalWrite(routineYellowPin, LOW);

  //setup for led matrix:
  pinMode(DIN, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(CLK, OUTPUT);

  writeColumn(0x0c, 0x01);
  writeColumn(0x0b, 0x07);

  clearMatrix();
}

double Thermistor(int RawADC) {
  double Temp;
  Temp = log(10000.0 * ((1024.0 / RawADC - 1)));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
  Temp = Temp - 273.15;            // Convert Kelvin to Celcius
  return Temp;
}

void displaySpeed (int num)
{
  switch (num) {
    case 0: {
        writeColumn(6, 0x7c);
        writeColumn(7, 0x44);
        writeColumn(8, 0x7c);
        delay(100);
        break;
      }

    case 1: {
        writeColumn(1, 0x7c);
        delay(100);
        break;

      }
    case 2: {
        writeColumn(1, 0x74);
        writeColumn(2, 0x54);
        writeColumn(3, 0x5c);
        delay(100);
        break;
      }

    case 3: {
        writeColumn(1, 0x54);
        writeColumn(2, 0x54);
        writeColumn(3, 0x7c);
        delay(100);
        break;
      }

    case 4: {
        writeColumn(1, 0x1c);
        writeColumn(2, 0x10);
        writeColumn(3, 0x7c);
        delay(100);
        break;
      }

    case 5: {
        writeColumn(1, 0x5c);
        writeColumn(2, 0x54);
        writeColumn(3, 0x74);
        delay(100);
        break;
      }

    case 6: {
        writeColumn(1, 0x7c);
        writeColumn(2, 0x54);
        writeColumn(3, 0x74);
        delay(100);
        break;
      }

    case 7: {
        writeColumn(1, 0x04);
        writeColumn(2, 0x04);
        writeColumn(3, 0x7c);
        delay(100);
        break;
      }

    case 8: {
        writeColumn(1, 0x7c);
        writeColumn(2, 0x54);
        writeColumn(3, 0x7c);
        delay(100);
        break;
      }
    case 9: {
        writeColumn(1, 0x5c);
        writeColumn(2, 0x54);
        writeColumn(3, 0x7c);
        delay(100);
        break;
      }
    case 10: {
        writeColumn(1, 0x7c);
        writeColumn(3, 0x7c);
        writeColumn(4, 0x44);
        writeColumn(5, 0x7c);
        delay(100);
        break;
      }

    case 11: {
        writeColumn(1, 0x7c);
        writeColumn(3, 0x7c);
        delay(100);
        break;

      }

    case 12: {
        writeColumn(1, 0x7c);
        writeColumn(3, 0x74);
        writeColumn(4, 0x54);
        writeColumn(5, 0x5c);
        delay(100);
        break;
      }
  }
}

void clearMatrix()
{
  for (int i = 1; i <= 8; i++)
  {
    writeColumn(i, 0);
  }
}
int getBit(byte b, int bit_index)
{
  return (b & (1 << bit_index)) != 0;
}

void writeBit(bool b) // Write 1 bit to the buffer
{
  digitalWrite(DIN, b);
  digitalWrite(CLK, LOW);
  digitalWrite(CLK, HIGH);
}
void writeColumn(int col, byte value)
{
  writeByte(col);
  writeByte(value);
  latchBuf();
}
void writeByte(byte b)
{
  for (int i = sizeof(byte) * 8 - 1; i >= 0; i--)
  {
    writeBit( (b & (1 << i)) != 0);
  }
}
void latchBuf() // Latch the entire buffer (sends the entire buffer to the matrix)
{
  digitalWrite(CS, LOW);
  digitalWrite(CS, HIGH);

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

long waitforpress() {
  bool firstPress = true;
  long pressDuration = 0 ;
  while (pressDuration < 2000)
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
    if (lastState == HIGH && digitalRead(emergencyButton) == LOW )
    {
      Serial.println(digitalRead(emergencyButton));
      // button is pressed
      startTime = millis();
      lastState = LOW;
      firstPress = false;
    }
    else if (lastState == LOW && digitalRead(emergencyButton) == HIGH && firstPress == false)
    {
      // button is released
      endTime = millis();
      pressDuration = endTime - startTime;
      lastState = HIGH;
    }
  }

  return pressDuration;
}

void loop() {
  Serial.println(temp);
  if (EEPROM.read(addr) > 12 || EEPROM.read(addr) < 1) {
    trainSpeed = 1;
    EEPROM.write(addr, trainSpeed);
  }

  if (state != 2)
  {

    displaySpeed(trainSpeed);
  }

  readVal = analogRead(sensorPin);
  temp =  Thermistor(readVal);

  wrapperLogoDisplay();

  if (temp > 30)
  {
    if (goodTemperature)
    {
      EEPROM.write(addr, trainSpeed);
      clearMatrix();
    }
    if (trainSpeed > 8)
    {
      trainSpeed = 8;
    }
    Blink(emergencyRedPin, 500);
    goodTemperature = false;
  }
  else if (!goodTemperature) {
    digitalWrite(emergencyRedPin, LOW);
    trainSpeed = EEPROM.read(addr);
    goodTemperature = true;
    clearMatrix();
  }

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (digitalRead(emergencyButton) != HIGH) {
    digitalWrite(emergencyRedPin, HIGH);
    digitalWrite(mainGreenPin, LOW);
    digitalWrite(mainYellowPin, LOW);
    digitalWrite(mainRedPin, HIGH);
    digitalWrite(routineGreenPin, LOW);
    digitalWrite(routineYellowPin, HIGH);
    isEmergencyOn = true;
    clearMatrix();
    displaySpeed(trainSpeed);
    long pressDuration = waitforpress();
    if (pressDuration >= 2000) {
      isEmergencyOn = false;
      digitalWrite(emergencyRedPin, LOW);
      digitalWrite(mainRedPin, LOW);
      digitalWrite(routineYellowPin, LOW);
      routineLight(state);
    }
  }
  if (digitalRead(routineButton) == LOW && !isEmergencyOn)
  {
    while (digitalRead(routineButton) == HIGH)
    {}
    // button is pressed
    startTime = millis();

    while (digitalRead(routineButton) == LOW)// && firstPress == false)
    {}
    // button is released
    endTime = millis();
    duration = endTime - startTime;

    if (duration >= 2000 && isDriveSlowlyOn == false && state == 1)
    {
      isDriveSlowlyOn = true;
    }
    else if (duration >= 2000 && isDriveSlowlyOn == true && state == 1)
    {
      isDriveSlowlyOn = false;
      EEPROM.write(addr, trainSpeed);
    }
    else if ((duration < 2000 && isDriveSlowlyOn == false) || state != 1) {
      state++;
      state %= 3;
      routineLight(state);
      clearMatrix();
    }
    else if (duration < 2000 && isDriveSlowlyOn == true && state == 1)
    {
      trainSpeed ++;
      if (trainSpeed > 12) {
        trainSpeed = 1;
      }
    }
  }

  if (isDriveSlowlyOn == true)
  {
    clearMatrix();
    delay(60);
    displaySpeed(trainSpeed);
    delay(60);
  }
}
