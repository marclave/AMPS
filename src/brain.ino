#define LEFT_MOTOR_BACKWARD  3
#define LEFT_MOTOR_FORWARD   5
#define RIGHT_MOTOR_BACKWARD 6
#define RIGHT_MOTOR_FORWARD 10

#define RIGHT_SENSOR   0
#define MIDDLE_SENSOR  1
#define LEFT_SENSOR    2
#define READY_TO_DRIVE 12

#define THRESHOLD 900

#define LEFT_DIRECTION  0x4c
#define RIGHT_DIRECTION 0x52
#define UP_DIRECTION    0x55
#define U_TURN          0x54

#define LEFT_MOTOR_SPEED  105
#define RIGHT_MOTOR_SPEED 100

#include <EEPROM.h>

unsigned char SHORTESTPATH[512];
uint16_t PATH_ELEMENT = 0;

bool isEEPROMempty(void)
{
  unsigned char eepromValue;
  
  for (uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
    eepromValue = EEPROM.read(eepromAddress);
    if (((eepromValue & LEFT_DIRECTION) != 0x00) || ((eepromValue & RIGHT_DIRECTION) != 0x00) || ((eepromValue & UP_DIRECTION) != 0x00) || ((eepromValue & U_TURN) != 0x00))
    {
      return false; // This means EEPROM is not empty
    }    
  }
  return true; // This means we have our path
}

void readEEPROM(void)
{
  for (uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
     SHORTESTPATH[eepromAddress] = EEPROM.read(eepromAddress);
  } 
}

void clearEEPROM(void)
{
  for (uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
     EEPROM.write(eepromAddress, '\0');
  }
}

void writeShortestPath(void)
{
  for (uint16_t eepromAddress = 0; eepromAddress < 512; eepromAddress++)
  {
     EEPROM.write(eepromAddress, SHORTESTPATH[eepromAddress]);
  }
}


void setup() {
  Serial.begin(9600);
  pinMode(READY_TO_DRIVE, INPUT);

  while(!digitalRead(READY_TO_DRIVE))
  {
    /**
    if (deleteEEPROM)
    {
      clearEEPROM();
    }
    **/
  }
  Serial.print("R2D BRUH");
  delay(1000);
  
  // Now we wait for the next push to enable movement
  while(!digitalRead(READY_TO_DRIVE))
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
  
  /**
  if (isEEPROMempty)
  {
    // Read EEPROM and use it whenever there is a junction
  }
  **/


}

void performUturn(void)
{
 while(analogRead(MIDDLE_SENSOR) < THRESHOLD)
  {
    analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_SPEED);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, 0);
    analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_SPEED);
  }
  
  Serial.println("TURNING AROUND");
  //SHORTESTPATH[PATH_ELEMENT] = LEFT_DIRECTION;
  //PATH_ELEMENT++;
}

void turnRight(void)
{
  delay(100); // Just for positioning with sensors in front
  while(analogRead(MIDDLE_SENSOR) < THRESHOLD)
  {
    analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_SPEED);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, 0);
    analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_SPEED);
  }
  
  Serial.println("TURNING RIGHT");
  //SHORTESTPATH[PATH_ELEMENT] = RIGHT_DIRECTION;
  //PATH_ELEMENT++;
}

void debugShortestPath(void)
{
  for (uint16_t elem; elem < 512; elem++)
  {
     Serial.print(SHORTESTPATH[elem]);
     Serial.print(" ");
  } 
  Serial.println("");
}

void turnLeft(void)
{
  delay(100); // Just for positioning with sensors in front

  while(analogRead(MIDDLE_SENSOR) < THRESHOLD)
  {
      analogWrite(LEFT_MOTOR_FORWARD, 0);
      analogWrite(LEFT_MOTOR_BACKWARD, LEFT_MOTOR_SPEED);
      analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_SPEED);
      analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  }
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
  Serial.print(sensRight);
  Serial.println("");
  
  if (sensRight > THRESHOLD)
  {
   turnRight(); 
  }
  else if ((sensLeft < THRESHOLD) && (sensMid > THRESHOLD))
  {
    // This is just go straight
    analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_SPEED);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_SPEED);
    analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  }
  else if (sensLeft > THRESHOLD)
  { 
   turnLeft(); 
  }
  else
  {
    //performUturn();
  }
  
  analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_SPEED);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_SPEED);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  
  //debugShortestPath();
}
