#define LEFT_MOTOR_BACKWARD  3
#define LEFT_MOTOR_FORWARD   5
#define RIGHT_MOTOR_BACKWARD 6
#define RIGHT_MOTOR_FORWARD 10

#define RIGHT_SENSOR   0
#define MIDDLE_SENSOR  1
#define LEFT_SENSOR    2

#define READY_TO_DRIVE 12
#define DELETE_EEPROM  13   

#define THRESHOLD 930
#define THRESHOLD_MIDDLE 840

#define LEFT_DIRECTION  0x4c
#define RIGHT_DIRECTION 0x52
#define UP_DIRECTION    0x55
#define U_TURN          0x54

#define LEFT_MOTOR_SPEED  155
#define RIGHT_MOTOR_SPEED 150

#include <EEPROM.h>

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
  pinMode(READY_TO_DRIVE, INPUT);

  while(!digitalRead(READY_TO_DRIVE))
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
 while(analogRead(MIDDLE_SENSOR) < THRESHOLD_MIDDLE)
  {
    analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_SPEED);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, 0);
    analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_SPEED);
  }
  
  Serial.println("TURNING AROUND");
  checkedFinish = true;
  //SHORTESTPATH[PATH_ELEMENT] = U_TURN;
  //PATH_ELEMENT++;
}

void turnRight(void)
{
  //delay(50); // Just for positioning with sensors in front
  while(analogRead(MIDDLE_SENSOR) < THRESHOLD_MIDDLE)
  {
    //int mid = analogRead(MIDDLE_SENSOR);
//Serial.println(mid);
    analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_SPEED);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, 0);
    analogWrite(RIGHT_MOTOR_BACKWARD, RIGHT_MOTOR_SPEED);
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
    while(1)
    {
       delay(10000);
       readEEPROM();
    }
}

void turnLeft(void)
{
  //delay(50); // Just for positioning with sensors in front

  while(analogRead(MIDDLE_SENSOR) < THRESHOLD_MIDDLE)
  {
      analogWrite(LEFT_MOTOR_FORWARD, 0);
      analogWrite(LEFT_MOTOR_BACKWARD, LEFT_MOTOR_SPEED);
      analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_SPEED);
      analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  }
    Serial.println("TURNING LEFT");
    checkedFinish = true;
}

void finished(void)
{
  while(1)
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
  delay(100);
  
  int sensMid = analogRead(MIDDLE_SENSOR);
  int sensRight = analogRead(RIGHT_SENSOR);
  int sensLeft = analogRead(LEFT_SENSOR);
  
  if ((sensRight > THRESHOLD) && (sensMid > THRESHOLD_MIDDLE) && (sensLeft > THRESHOLD))
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
  
  /**
  Serial.print(sensLeft);
  Serial.print(" ");
  Serial.print(sensMid);
  Serial.print(" ");
  Serial.print(sensRight);
  Serial.println("");
  **/
  
  if ((sensRight > THRESHOLD) && (sensMid > THRESHOLD_MIDDLE) && (sensLeft > THRESHOLD) && (checkedFinish == false))
  {
     if(finishLine())
     {
       finished();
     } 
     else
     {
       moveBack();
     }
  }
  else if (sensRight > THRESHOLD)
  {
   turnRight(); 
  }
  else if ((sensLeft < THRESHOLD) && (sensMid > THRESHOLD_MIDDLE))
  {
    // This is just go straight
    analogWrite(LEFT_MOTOR_FORWARD, LEFT_MOTOR_SPEED);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
    analogWrite(RIGHT_MOTOR_FORWARD, RIGHT_MOTOR_SPEED);
    analogWrite(RIGHT_MOTOR_BACKWARD, 0);
    checkedFinish = true;
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
  
  if (digitalRead(READY_TO_DRIVE))
  {
     stopForTesting();   
  }
 
}
