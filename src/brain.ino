#define LEFT_MOTOR_BACKWARD  3
#define LEFT_MOTOR_FORWARD   5
#define RIGHT_MOTOR_BACKWARD 6
#define RIGHT_MOTOR_FORWARD 10

#define RIGHT_SENSOR   0
#define MIDDLE_SENSOR  1
#define LEFT_SENSOR    2

#define DELETE_EEPROM  12
#define READY_TO_DRIVE 7

#define THRESHOLD_LEFT 700
#define THRESHOLD_MIDDLE 500
#define THRESHOLD_RIGHT 700

#define LEFT_DIRECTION  0x4c
#define RIGHT_DIRECTION 0x52
#define UP_DIRECTION    0x55
#define U_TURN          0x54

#define LEFT_MOTOR_SPEED  145//105
#define RIGHT_MOTOR_SPEED 140//  100

#define LEFT_MOTOR_TURN_SPEED  255//205
#define RIGHT_MOTOR_TURN_SPEED 250//200

#define MAX_TIME_FOR_U_TURN 600

#include <EEPROM.h>
#include <elapsedMillis.h>

unsigned char SHORTESTPATH[512];
uint16_t PATH_ELEMENT = 0;
bool checkedFinish = false;

bool isEEPROMempty(void)
{
  unsigned char eepromValue;

  for (uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
    eepromValue = EEPROM.read(eepromAddress);
    if (((eepromValue & LEFT_DIRECTION) != 0x00) || ((eepromValue & RIGHT_DIRECTION) != 0x00) || ((eepromValue & UP_DIRECTION) != 0x00) || ((eepromValue & U_TURN) != 0x00))
    {
      return true; // This means EEPROM is empty

      Serial.println("EEPROM is empty");
    }
  }

  Serial.println("EEPROM is not empty");
  return false; // This means we have our path

}

void readEEPROM(void)
{
  for (uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
    SHORTESTPATH[eepromAddress] = EEPROM.read(eepromAddress);
  }

  Serial.println("EEPROM Read from");
}

void clearEEPROM(void)
{
  for (uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
    EEPROM.write(eepromAddress, '\0');
  }

  Serial.println("EEPROM Cleared");
}

void writeShortestPath(void)
{
  for (uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
    EEPROM.write(eepromAddress, SHORTESTPATH[eepromAddress]);
  }

  Serial.println("Shortest path written to EEPROM!");
}


void setup() {
  Serial.begin(9600);
  delay(3000);
  pinMode(READY_TO_DRIVE, INPUT);

  while (!digitalRead(READY_TO_DRIVE))
  {
    /**
    if (digitalRead(DELETE_EEPROM))
    {
      clearEEPROM();
    }
    **/
  }
  Serial.print("R2D BRUH");
  delay(1000);

  // Now we wait for the next push to enable movement
  while (!digitalRead(READY_TO_DRIVE))
  {
    int sensMid = analogRead(MIDDLE_SENSOR);
    int sensRight = analogRead(RIGHT_SENSOR);
    int sensLeft = analogRead(LEFT_SENSOR);

    Serial.print(sensLeft);
    Serial.print(" ");
    Serial.print(sensMid);
    Serial.print(" ");
    Serial.print(sensRight);
    Serial.println("");
  }
  Serial.println("Driving");
  delay(1000);
  /**
  if (isEEPROMempty)
  {
    // Read EEPROM and use it whenever there is a junction
  }
  **/


}

void performUturn(void)
{
  analogWrite(LEFT_MOTOR_FORWARD, 0);
  analogWrite(LEFT_MOTOR_BACKWARD, LEFT_MOTOR_SPEED);
  analogWrite(RIGHT_MOTOR_FORWARD, 0);
  analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_SPEED);
  Serial.println("Turning around");

  while (true)
  {
    int sensMid = analogRead(MIDDLE_SENSOR);
    int sensRight = analogRead(RIGHT_SENSOR);
    int sensLeft = analogRead(LEFT_SENSOR);

    if (sensMid > THRESHOLD_MIDDLE)
    {
      Serial.println("TIME TO TURN AROUND");
      turnRightBackwards();
      return;
    }
  }
}

void turnRightBackwards(void)
{
  analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_TURN_SPEED);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_FORWARD, 0);
  analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_TURN_SPEED);

  delay(200);
  while (analogRead(MIDDLE_SENSOR) < THRESHOLD_MIDDLE)
  {
    analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_TURN_SPEED);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, 0);
    analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_TURN_SPEED);
  }

  Serial.println("TURNING RIGHT");
  checkedFinish = true;
  //SHORTESTPATH[PATH_ELEMENT] = RIGHT_DIRECTION;
  //PATH_ELEMENT++;
}

void turnRight(void)
{
  //delay(100);
  analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_TURN_SPEED);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_FORWARD, 0);
  analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_TURN_SPEED);

  delay(150); // Just for positioning with sensors in front
  while (analogRead(MIDDLE_SENSOR) < THRESHOLD_MIDDLE)
  {
    analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_TURN_SPEED);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, 0);
    analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_TURN_SPEED);
  }

  Serial.println("TURNING RIGHT");
  checkedFinish = true;
  //SHORTESTPATH[PATH_ELEMENT] = RIGHT_DIRECTION;
  //PATH_ELEMENT++;
}

void stopForTesting(void)
{
  analogWrite(LEFT_MOTOR_FORWARD, 0);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_FORWARD, 0);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  writeShortestPath();
  while (1)
  {
    delay(10000);
    readEEPROM();
  }
}

void turnLeftBackwards(void)
{
  analogWrite(LEFT_MOTOR_FORWARD, 0);
  analogWrite(LEFT_MOTOR_BACKWARD, LEFT_MOTOR_TURN_SPEED);
  analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_TURN_SPEED);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);

  while (analogRead(MIDDLE_SENSOR) < THRESHOLD_MIDDLE)
  {
    analogWrite(LEFT_MOTOR_FORWARD, 0);
    analogWrite(LEFT_MOTOR_BACKWARD, LEFT_MOTOR_TURN_SPEED);
    analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_TURN_SPEED);
    analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  }
  Serial.println("TURNING LEFT");
  checkedFinish = true;
}

void turnLeft(void)
{
  //delay(100);
  analogWrite(LEFT_MOTOR_FORWARD, 0);
  analogWrite(LEFT_MOTOR_BACKWARD, LEFT_MOTOR_TURN_SPEED);
  analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_TURN_SPEED);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);

  delay(150); // Just for positioning with sensors in front

  while (analogRead(MIDDLE_SENSOR) < THRESHOLD_MIDDLE)
  {
    analogWrite(LEFT_MOTOR_FORWARD, 0);
    analogWrite(LEFT_MOTOR_BACKWARD, LEFT_MOTOR_TURN_SPEED);
    analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_TURN_SPEED);
    analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  }
  Serial.println("TURNING LEFT");
  checkedFinish = true;
}

void finished(void)
{
  while (1)
  {
    analogWrite(LEFT_MOTOR_FORWARD, 0);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, 0);
    analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  }
}

bool finishLine(void)
{
  analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_SPEED);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_SPEED);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  delay(150);

  int sensMid = analogRead(MIDDLE_SENSOR);
  int sensRight = analogRead(RIGHT_SENSOR);
  int sensLeft = analogRead(LEFT_SENSOR);

  if ((sensRight > THRESHOLD_RIGHT) && (sensMid > THRESHOLD_MIDDLE) && (sensLeft > THRESHOLD_LEFT))
  {
    Serial.println("FINISH");
    return true;
  }
  else
  {
    return false;
  }
}

void moveBack(void)
{
  analogWrite(LEFT_MOTOR_FORWARD, 0);
  analogWrite(LEFT_MOTOR_BACKWARD, LEFT_MOTOR_SPEED);
  analogWrite(RIGHT_MOTOR_FORWARD, 0);
  analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_SPEED);
  delay(100);

}

void loop()
{
  int sensMid = analogRead(MIDDLE_SENSOR);
  int sensRight = analogRead(RIGHT_SENSOR);
  int sensLeft = analogRead(LEFT_SENSOR);

  if (sensRight > THRESHOLD_RIGHT)
  {
    Serial.println("Gunna turn right");
    turnRight();
  }
  else if ((sensLeft > THRESHOLD_LEFT) && (sensMid > THRESHOLD_MIDDLE))
  {
    // This is just go straight
    analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_SPEED);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_SPEED);
    analogWrite(RIGHT_MOTOR_BACKWARD, 0);
    //checkedFinish = true;
  }
  else if (sensLeft > THRESHOLD_LEFT)
  {
    Serial.println("Turning Left");
    turnLeft();
  }
  else if ((sensLeft < THRESHOLD_LEFT) && (sensMid < THRESHOLD_MIDDLE) && (sensRight < THRESHOLD_RIGHT))
  {
    bool turnt = false;
    Serial.println("Might  U turn around");

    elapsedMillis timeElapsed;
    while (timeElapsed < MAX_TIME_FOR_U_TURN)
    {
      sensMid = analogRead(MIDDLE_SENSOR);
      sensRight = analogRead(RIGHT_SENSOR);
      sensLeft = analogRead(LEFT_SENSOR);

      if (sensRight > THRESHOLD_RIGHT)
      {
        Serial.println("U turn right");
        turnRight();
        turnt = true;
      }
      else if (sensLeft > THRESHOLD_LEFT)
      {
        Serial.println("U Turning Left");
        turnLeft();
        turnt = true;
      }
      if (turnt)
      {
        break;
      }
    }
    Serial.println(timeElapsed);
    if (!turnt)
    {
      sensMid = analogRead(MIDDLE_SENSOR);
      sensRight = analogRead(RIGHT_SENSOR);
      sensLeft = analogRead(LEFT_SENSOR);

      if ((sensLeft < THRESHOLD_LEFT) && (sensMid < THRESHOLD_MIDDLE) && (sensRight < THRESHOLD_RIGHT))
      {
        Serial.println("U Turning around");
        performUturn();
      }
    }
  }
  else if ((sensLeft > THRESHOLD_LEFT) && (sensMid > THRESHOLD_MIDDLE) && (sensRight > THRESHOLD_RIGHT))
  {
    Serial.println("Finishing!!!!!!!!!!!!!");
    Serial.println("Finishing!!!!!!!!!!!!!");
    Serial.println("Finishing!!!!!!!!!!!!!");
    Serial.println("Finishing!!!!!!!!!!!!!");
  }

  analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_SPEED);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_SPEED);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);
}
