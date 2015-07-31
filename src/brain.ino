/*** MOSFET Motor PWM pins ***/
#define LEFT_MOTOR_BACKWARD  3
#define LEFT_MOTOR_FORWARD   5
#define RIGHT_MOTOR_BACKWARD 6
#define RIGHT_MOTOR_FORWARD 10

/*** Sensor pins ***/
#define RIGHT_SENSOR       0
#define MIDDLE_SENSOR      1
#define LEFT_SENSOR        2

#define OUTER_LEFT_SENSOR  3
#define OUTER_RIGHT_SENSOR 4

/*** Push Button pins ***/
#define DELETE_EEPROM  12
#define READY_TO_DRIVE 7

/*** Arduino Timer signal ***/
#define PERIPHERAL_ARDUINO_SIGNAL 8

/*** IR Thresholds ***/
#define THRESHOLD_LEFT   700
#define THRESHOLD_MIDDLE 500
#define THRESHOLD_RIGHT  750

#define CORRECTION_LEFT_THRESHOLD   300
#define CORRECTION_MIDDLE_THRESHOLD 200
#define CORRECTION_RIGHT_THRESHOLD  200

/*** EEPROM Hexadecimal defines for direction ***/
#define LEFT_DIRECTION  0x4c
#define RIGHT_DIRECTION 0x52
#define STRAIGHT        0x53
#define U_TURN          0x54
#define FINISH          0x46

/*** Forward driving Motor Speed (PWM duty cycle) ***/
#define LEFT_MOTOR_SPEED  145
#define RIGHT_MOTOR_SPEED 140

#define LEFT_MOTOR_CORRECTION_SPEED  125
#define RIGHT_MOTOR_CORRECTION_SPEED 120

/*** Turning Motor Speed (PWM duty cycle) ***/
#define LEFT_MOTOR_TURN_SPEED  255
#define RIGHT_MOTOR_TURN_SPEED 250

#define LEFT_MOTOR_UTURN_SPEED 170

/*** Time Limits ***/
#define U_TURN_TIME      370
#define FINISH_LINE_TIME 150

#include <EEPROM.h>
#include <elapsedMillis.h>

unsigned char SHORTESTPATH[512];
uint16_t PATH_ELEMENT = 0;

/*
* This procedure checks if the EEPROM is empty or not
* return:
*       true if empty
*       false if not empty
*/
bool isEEPROMempty(void)
{
  unsigned char eepromValue;

  for (uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
    eepromValue = EEPROM.read(eepromAddress);
    if (((eepromValue & LEFT_DIRECTION) != 0x00) || ((eepromValue & RIGHT_DIRECTION) != 0x00) || ((eepromValue & STRAIGHT) != 0x00) || ((eepromValue & U_TURN) != 0x00))
    {
      Serial.println("EEPROM is empty");
      return true; // This means EEPROM is empty
    }
  }

  Serial.println("EEPROM is not empty");
  return false; // This means we have our path

}

/*
* This procedure reads from the EEPROM and saves it to the SHORTESTPATH
*/
void readEEPROM(void)
{
  for (uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
    SHORTESTPATH[eepromAddress] = EEPROM.read(eepromAddress);
  }

  Serial.println("EEPROM Read from");
}

/*
* This procedure writes the to the EEPROM all zero
*/
void clearEEPROM(void)
{
  for (uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
    EEPROM.write(eepromAddress, '\0');
  }

  Serial.println("EEPROM Cleared");
}

/*
* This procedure writes the saved shortest path to EEPROM to be used later
*/
void writeShortestPath(void)
{
  for (uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
    EEPROM.write(eepromAddress, SHORTESTPATH[eepromAddress]);
  }

  Serial.println("Shortest path written to EEPROM!");
}

void debugShortestPath(void)
{
  for (uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
    Serial.println(SHORTESTPATH[eepromAddress], HEX);
  }
  Serial.println("Shortest path read!");
}

void calibration(void)
{
  // Now we wait for the next push to enable movement
  // This is important for manual calibration
  while (!digitalRead(READY_TO_DRIVE))
  {
    uint16_t sensMid = analogRead(MIDDLE_SENSOR);
    uint16_t sensRight = analogRead(RIGHT_SENSOR);
    uint16_t sensLeft = analogRead(LEFT_SENSOR);
    uint16_t sensOuterLeft = analogRead(OUTER_LEFT_SENSOR);
    uint16_t sensOuterRight = analogRead(OUTER_RIGHT_SENSOR);

    Serial.print(sensOuterLeft);
    Serial.print(" ");
    Serial.print(sensLeft);
    Serial.print(" ");
    Serial.print(sensMid);
    Serial.print(" ");
    Serial.print(sensRight);
    Serial.print(" ");
    Serial.print(sensOuterRight);
    Serial.println(" ");
  }
}

void idle(void)
{
  digitalWrite(PERIPHERAL_ARDUINO_SIGNAL, HIGH);
  // Waiting for ready to drive button to be pushed
  while (!digitalRead(READY_TO_DRIVE))
  {
    /**
    if (digitalRead(DELETE_EEPROM))
    {
      clearEEPROM();
    }
    **/
  }
  Serial.println("Ready To Drive Enabled");
  delay(1000);
  Serial.println("Calibration");

  calibration();

  Serial.println("Driving");
  delay(1000);

  driving();
  
  /**
  // If the EEPROM is not empty, will use best path
  if (isEEPROMempty)
  {
    // Read EEPROM and use it whenever there is a junction
  }
  **/
}

void setup() {
  Serial.begin(9600);
  delay(3000);
  pinMode(READY_TO_DRIVE, INPUT);
  pinMode(PERIPHERAL_ARDUINO_SIGNAL, OUTPUT);
  idle();
}

/*
* This procedure propels the A.M.P.S machine in the backwards direction, then turns the
* machine around once back on track
*/
void reverse(void)
{
  analogWrite(LEFT_MOTOR_FORWARD, 0);
  analogWrite(LEFT_MOTOR_BACKWARD, LEFT_MOTOR_SPEED);
  analogWrite(RIGHT_MOTOR_FORWARD, 0);
  analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_SPEED);
  Serial.println("Reversing");

  while (true)
  {
    uint16_t sensMid = analogRead(MIDDLE_SENSOR);
    uint16_t sensRight = analogRead(RIGHT_SENSOR);
    uint16_t sensLeft = analogRead(LEFT_SENSOR);

    if ((sensMid > THRESHOLD_MIDDLE) || (sensLeft > CORRECTION_LEFT_THRESHOLD) || (sensRight > THRESHOLD_RIGHT))
    {
      turnAround();
      return;
    }
  }
}

/*
* This procedure turns the A.M.P.S machine around
*/
void turnAround(void)
{
  Serial.println("TURNING AROUND");
  analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_SPEED);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_FORWARD, 0);
  analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_TURN_SPEED);

  delay(200);
  while (analogRead(MIDDLE_SENSOR) < THRESHOLD_MIDDLE)
  {
    analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_UTURN_SPEED);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, 0);
    analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_TURN_SPEED);

    if (analogRead(RIGHT_SENSOR) > CORRECTION_RIGHT_THRESHOLD)
    {
      moveForward();
      delay(50);
      return;
    }
  }

  SHORTESTPATH[PATH_ELEMENT] = U_TURN;
  PATH_ELEMENT++;
}

/*
* This procedure corrects the A.M.P.S machine position in the right direction
*/
void correctPositionRight(void)
{
  Serial.println("Correcting RIGHT");
  analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_SPEED);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_FORWARD, 0);
  analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_SPEED);

  while (analogRead(MIDDLE_SENSOR) < THRESHOLD_MIDDLE)
  {
    if (analogRead(OUTER_RIGHT_SENSOR) > THRESHOLD_RIGHT)
    {
      turnRight();
      return;
    }
    else if ((analogRead(MIDDLE_SENSOR) > THRESHOLD_MIDDLE) && (analogRead(OUTER_LEFT_SENSOR) > THRESHOLD_LEFT))
    {
      moveForward();
      return;
    }
    else if (analogRead(OUTER_LEFT_SENSOR) > THRESHOLD_LEFT)
    {
      turnLeft();
      return;
    }
    analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_TURN_SPEED);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, 0);
    analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_TURN_SPEED);
  }
}

/*
* This procedure turns the A.M.P.S machine in the right direction
*/
void turnRight(void)
{
  Serial.println("TURNING RIGHT");
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

  SHORTESTPATH[PATH_ELEMENT] = RIGHT_DIRECTION;
  PATH_ELEMENT++;
}

/*
* This procedure corrects the A.M.P.S machine position in the left direction
*/
void correctPositionLeft(void)
{
  Serial.println("correcting LEFT");
  analogWrite(LEFT_MOTOR_FORWARD, 0);
  analogWrite(LEFT_MOTOR_BACKWARD, LEFT_MOTOR_SPEED);
  analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_SPEED);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);

  while (analogRead(MIDDLE_SENSOR) < THRESHOLD_MIDDLE)
  {
    if (analogRead(OUTER_RIGHT_SENSOR) > THRESHOLD_RIGHT)
    {
      turnRight();
      return;
    }
    else if ((analogRead(MIDDLE_SENSOR) > THRESHOLD_MIDDLE) && (analogRead(OUTER_LEFT_SENSOR) > THRESHOLD_LEFT))
    {
      moveForward();
      return;
    }
    else if (analogRead(OUTER_LEFT_SENSOR) > THRESHOLD_LEFT)
    {
      turnLeft();
      return;
    }
    analogWrite(LEFT_MOTOR_FORWARD, 0);
    analogWrite(LEFT_MOTOR_BACKWARD, LEFT_MOTOR_TURN_SPEED);
    analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_TURN_SPEED);
    analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  }
}

/*
* This procedure turns the A.M.P.S machine in the left direction
*/
void turnLeft(void)
{
  Serial.println("TURNING LEFT");
  analogWrite(LEFT_MOTOR_FORWARD, 0);
  analogWrite(LEFT_MOTOR_BACKWARD, LEFT_MOTOR_TURN_SPEED);
  analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_TURN_SPEED);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);

  delay(125); // Just for positioning with sensors in front

  while (analogRead(MIDDLE_SENSOR) < THRESHOLD_MIDDLE)
  {
    analogWrite(LEFT_MOTOR_FORWARD, 0);
    analogWrite(LEFT_MOTOR_BACKWARD, LEFT_MOTOR_TURN_SPEED);
    analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_TURN_SPEED);
    analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  }
  SHORTESTPATH[PATH_ELEMENT] = LEFT_DIRECTION;
  PATH_ELEMENT++;
}

/*
* This procedure stops the A.M.P.S machine and waits to redo its driving
*/
void finished(void)
{
  SHORTESTPATH[PATH_ELEMENT] = FINISH;
  PATH_ELEMENT++;
  Serial.println("Finished");
  digitalWrite(PERIPHERAL_ARDUINO_SIGNAL, HIGH); // Starts the seperate arduino/LCD display to start timing
  while (true)
  {
    analogWrite(LEFT_MOTOR_FORWARD, 0);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, 0);
    analogWrite(RIGHT_MOTOR_BACKWARD, 0);

    if (digitalRead(READY_TO_DRIVE))
    {
      //debugShortestPath();
      //delay(3000);
      Serial.println("Going to idle");
      idle();
    }
  }
}

/*
* This procedure checks if the finish line is present, stops if it does, otherwise continues its search
*/
void checkFinish(void)
{
  Serial.println("Checking Finish");
  uint16_t sensMid;
  uint16_t sensRight;
  uint16_t sensLeft;

  elapsedMillis timeElapsed;
  while (timeElapsed < FINISH_LINE_TIME)
  {
  }

  sensMid = analogRead(MIDDLE_SENSOR);
  sensRight = analogRead(RIGHT_SENSOR);
  sensLeft = analogRead(LEFT_SENSOR);
  if ((sensLeft > THRESHOLD_LEFT) && (sensMid > THRESHOLD_MIDDLE) && (sensRight > THRESHOLD_RIGHT))
  {
    finished();
  }
}

/*
* This procedure propels the A.M.P.S. machine forward
*/
void moveForward(void)
{
  if (analogRead(OUTER_LEFT_SENSOR) > THRESHOLD_LEFT)
  {
    SHORTESTPATH[PATH_ELEMENT] = STRAIGHT;
    PATH_ELEMENT++;
  }
  analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_SPEED);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_SPEED);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);
}

/*
* Polls direction after a certain period of time, checking if any sensors go high, correction if its true
* If no sensors are high it then performs a U-Turn
*/
void checkUturn(void)
{
  uint16_t sensMid;
  uint16_t sensRight;
  uint16_t sensLeft;
  bool correctedPosition = false;
  Serial.println("Might turn around");

  elapsedMillis timeElapsed;
  while (timeElapsed < U_TURN_TIME)
  {
    sensMid = analogRead(MIDDLE_SENSOR);
    sensRight = analogRead(RIGHT_SENSOR);
    sensLeft = analogRead(LEFT_SENSOR);
    if (sensRight > CORRECTION_RIGHT_THRESHOLD)
    {
      Serial.println("Correcting position, turning right");
      correctPositionRight();
      correctedPosition = true;
    }
    else if (sensLeft > CORRECTION_LEFT_THRESHOLD)
    {
      Serial.println("Correcting position, turning Left");
      correctPositionLeft();
      correctedPosition = true;
    }
    else if (sensMid > CORRECTION_MIDDLE_THRESHOLD)
    {
      correctedPosition = true;
    }
    if (correctedPosition)
    {
      return;
    }
  }

  if (!correctedPosition)
  {
    sensMid = analogRead(MIDDLE_SENSOR);
    sensRight = analogRead(RIGHT_SENSOR);
    sensLeft = analogRead(LEFT_SENSOR);

    if ((sensLeft < THRESHOLD_LEFT) && (sensMid < THRESHOLD_MIDDLE) && (sensRight < THRESHOLD_RIGHT))
    {
      reverse();
    }
  }
}

void driving(void)
{
  digitalWrite(PERIPHERAL_ARDUINO_SIGNAL, LOW); // Starts the seperate arduino/LCD display to start timing
  while (true)
  {
    uint16_t sensMid = analogRead(MIDDLE_SENSOR);
    uint16_t sensRight = analogRead(RIGHT_SENSOR);
    uint16_t sensLeft = analogRead(LEFT_SENSOR);

    uint16_t sensOuterRight = analogRead(OUTER_RIGHT_SENSOR);
    uint16_t sensOuterLeft = analogRead(OUTER_LEFT_SENSOR);

    if ((sensLeft > THRESHOLD_LEFT) && (sensMid > THRESHOLD_MIDDLE) && (sensRight > THRESHOLD_RIGHT))
    {
      checkFinish();
    }
    else if (sensOuterRight > THRESHOLD_RIGHT)
    {
      turnRight();
    }
    else if ((sensOuterLeft > THRESHOLD_LEFT) && (sensMid > THRESHOLD_MIDDLE))
    {
      moveForward();
    }
    else if (sensOuterLeft > THRESHOLD_LEFT)
    {
      turnLeft();
    }
    else if ((sensLeft < CORRECTION_LEFT_THRESHOLD) && (sensMid < CORRECTION_MIDDLE_THRESHOLD) && (sensRight < CORRECTION_RIGHT_THRESHOLD))
    {
      checkUturn();
    }
    else if ((sensRight > CORRECTION_RIGHT_THRESHOLD) && (sensRight < THRESHOLD_RIGHT))
    {
      Serial.println("Correcting position, turning right");
      correctPositionRight();
    }
    else if ((sensLeft > CORRECTION_LEFT_THRESHOLD) && (sensLeft < THRESHOLD_LEFT))
    {
      Serial.println("Correcting position, turning Left");
      correctPositionLeft();
    }
    else
    {
      moveForward(); // This is the default
    }
  }
}

void loop()
{
}
