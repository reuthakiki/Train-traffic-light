#define MSIZE 8
#include <EEPROM.h>

const int mainGreenPin = 2;
const int mainYellowPin = 9;
const int mainRedPin = 8;

const int routineGreenPin = 4;
const int routineYellowPin = 3;

const int routineButton = 5;
const int emergencyButton = 6;

const int emergencyRedPin = 7;

int DIN = 13;
int CS = 12;
int CLK = 11;

int currState;
int lastState = HIGH;
int state = -1;  // the state increases
bool isEmergencyOn = false;
bool isDriveSlowlyOn = false;
bool buttonReleased = false;

int brightness = 0;    // how bright the LED is
int fadeAmount = 15;    // how many points to fade the LED by
int timerRunning = 1 ;
int timerRunningNew = 0;
int startTime = 0;
int endTime = 0;
//int trainSpeed = 1;
int addr = 0;
int trainSpeed = EEPROM.read(addr);
long duration = 0;

int readVal = 0;//=analogRead(sensorPin);
double temp = 0;//  Thermistor(readVal);

int sensorPin = A0; // select the input pin for the potentiometer

unsigned long previousMillis = 0;        // will store last time LED was updated

bool goodTemperature = true;


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
  //Serial.begin(115200);
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
  // Serial.begin(115200);

  writeColumn(0x0c, 0x01);
  writeColumn(0x0b, 0x07);

  clearMatrix();
}
void displayZero()
{
  writeColumn(6, 0x7c);
  writeColumn(7, 0x44);
  writeColumn(8, 0x7c);
}

double Thermistor(int RawADC) {
  double Temp;
  Temp = log(10000.0*((1024.0/RawADC-1))); 
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 273.15;            // Convert Kelvin to Celcius
   //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
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
byte createColumnByte(byte  matrix [MSIZE], int col)
{
  byte column_byte = 0;
  for (int i = 7; i >= 0; i--)
  {
    column_byte = (column_byte << 1) | getBit(matrix[i], 7 - col);
  }

  return column_byte;
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
  if (EEPROM.read(addr) > 12 || EEPROM.read(addr) < 1){
    trainSpeed = 1;
    EEPROM.write(addr, trainSpeed);
  }

displaySpeed(trainSpeed);

readVal=analogRead(sensorPin);
temp =  Thermistor(readVal);


if (temp > 25)
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
else if (!goodTemperature){
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
      //firstPress = false;

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
