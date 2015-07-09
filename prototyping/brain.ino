#define LEFT_MOTOR_BACKWARD  3
#define LEFT_MOTOR_FORWARD   5
#define RIGHT_MOTOR_BACKWARD 6
#define RIGHT_MOTOR_FORWARD 10

#define RIGHT_SENSOR  0
#define MIDDLE_SENSOR 1
#define LEFT_SENSOR   2

#define THRESHOLD 850

#define LEFT_DIRECTION  0x4c
#define RIGHT_DIRECTION 0x52
#define UP_DIRECTION    0x55

#include <EEPROM.h>

unsigned char SHORTESTPATH[512];

bool isEEPROMempty(void)
{
  byte eepromValue;
  
  for(uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
    eepromValue = EEPROM.read(eepromAddress);
    if (eepromValue != 0)
    {
      return false; // This means EEPROM is not empty
    }    
  }
  return true; // This means we have our path
}

void readEEPROM(void)
{
  // TODO m: read and store shortest path
}

void clearEEPROM(void)
{
  // TODO m: clear the EEPROM
}
void setup() {
  Serial.begin(9600);
  /*
  bool shortestPath = isEEPROMempty();
  while(!ReadyToDrive)
  {
    if (deleteEEPROM)
    {
      clearEEPROM();
      shortestPath = false;
    }
  }
  
  if (shortestPath)
  {
    // Read EEPROM and use it whenever there is a junction
  }
  */
}

void turnRight(void)
{
  delay(350); // Just for positioning with sensors in front
  analogWrite(LEFT_MOTOR_FORWARD, 105);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_FORWARD, 0);
  analogWrite(RIGHT_MOTOR_BACKWARD, 100);
  delay(1280);
  Serial.println("TURNING RIGHT");
}

void turnLeft(void)
{
  delay(350); // Just for positioning with sensors in front
  analogWrite(LEFT_MOTOR_FORWARD, 0);
  analogWrite(LEFT_MOTOR_BACKWARD, 105);
  analogWrite(RIGHT_MOTOR_FORWARD, 100);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  delay(1280);
  Serial.println("TURNING LEFT");
}

void loop()
{
  int sensMid = analogRead(MIDDLE_SENSOR);
  int sensRight = analogRead(RIGHT_SENSOR);
  int sensLeft = analogRead(LEFT_SENSOR);
  
  Serial.print(sensLeft);
  Serial.print(" ");
  Serial.print(sensMid);
  Serial.print(" ");
  Serial.println(sensRight);
  
  if (sensRight > THRESHOLD)
  {
   turnRight(); 
  }
  else if ((sensLeft < THRESHOLD) && (sensMid > THRESHOLD))
  {
    // This is just go straight
    analogWrite(LEFT_MOTOR_FORWARD, 105);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, 100);
    analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  }
  else if (sensLeft > THRESHOLD)
  { 
   turnLeft(); 
  }
  else
  {
    // U turn baby
  }
  
  analogWrite(LEFT_MOTOR_FORWARD, 105);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_FORWARD, 100);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);
}
