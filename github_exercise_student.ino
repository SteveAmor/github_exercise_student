#include <NewPing.h>
#include <ArduinoUnit.h>

#define Forward_Trigger_Pin 4
#define Forward_Echo_Pin 5
#define Left_Motor_IN2 7
#define Left_Motor_IN1 8
#define Left_Motor_Enable 9
#define Right_Motor_Enable 10
#define Right_Motor_IN4 11
#define Right_Motor_IN3 12

//#define RUN_UNIT_TESTS // comment out to bypass unit tests and run your program instead
#define SONAR_DELAY 50 // must wait 50msecs between sonar pings or you get spurious results
#define FORWARD_MAX_DISTANCE 200 // units are cm
#define Forward_Min_Distance 40  // units are cm
#define MOTOR_SPEED 150  // number between 0 and 255. Doesn't really move until you get above 130 (ish) 

unsigned int lastForwardPingTime = 0;
unsigned long lastForwardDistance = 0;

enum states { 
  moveForwards,
  halt,
  turnLeft
};

states currentState = halt;

enum lunch { 
  hamSandwich,
  readySaltedCrisps,
  apple
};

NewPing sonarFoward(Forward_Trigger_Pin, Forward_Echo_Pin, FORWARD_MAX_DISTANCE);

#ifdef RUN_UNIT_TESTS

test(ReadyToPingForward)
{
  lastForwardPingTime = millis();  
  assertTrue(isSonarForwardReadyToPing());  // no need to wait 50msec for first ever Ping
  lastForwardPingTime = millis();
  delay(SONAR_DELAY + 1);  // delay(50) = 49msecs so we add 1!!
  assertTrue(isSonarForwardReadyToPing());  
  lastForwardPingTime = millis();  // should wait 50msec so following assert is False
  assertFalse(isSonarForwardReadyToPing()); 
}

test(returnPreviousForwardDistance)
{
  lastForwardDistance = FORWARD_MAX_DISTANCE + 1;
  lastForwardPingTime = millis(); 
  assertEqual(sonicForwardDistance(), FORWARD_MAX_DISTANCE + 1)
}

test(returnNewForwardDistance)
{
  lastForwardDistance = FORWARD_MAX_DISTANCE + 1;
  lastForwardPingTime = millis(); 
  delay(SONAR_DELAY + 1);  // delay(50) = 49msecs so we add 1!!
  assertNotEqual(sonicForwardDistance(), FORWARD_MAX_DISTANCE + 1);
  assertNotEqual(lastForwardDistance, FORWARD_MAX_DISTANCE + 1);
  lastForwardDistance = FORWARD_MAX_DISTANCE + 1;
  assertEqual(sonicForwardDistance(), FORWARD_MAX_DISTANCE + 1);
}

#endif

void setup()
{
#ifdef RUN_UNIT_TESTS
  Serial.begin(115200);
#endif
}

void loop()
{
#ifdef RUN_UNIT_TESTS
  Test::run();  // unit tests only run once eventhough they are in loop()
#endif

#ifndef RUN_UNIT_TESTS //  insert your loop() program code after this line
  FSM();
#endif
}

void FSM()
{
  switch (currentState) { 
    case turnLeft:
      LEFT(150);  // go left
      if (isOkToMoveForwards()) {
        currentState = halt;  // state = halt
      }
      break;
    case moveForwards:
      forward(150);  // go forwards
      if (!isOkToMoveForwards()) {
        currentState = halt;  // state = halt
      }
      break;
    case halt:
    default:
      brakesOn();  // stop
      if (isOkToMoveForwards()) {
        currentState = moveForwards; // state = move forwards
      }
      else {
        currentState = turnLeft;  // state = turn left
      }
      break;
  }
}

boolean isSonarForwardReadyToPing()
{
  static boolean firstForwardPing = true;
  
  if (firstForwardPing == true) {
    firstForwardPing = false;
    return(true);  
  }  
  if (millis() - lastForwardPingTime >= SONAR_DELAY) {
    return(true); 
  }  
  return(false);
}

unsigned long sonicForwardDistance()
{
  if (isSonarForwardReadyToPing()) {
    lastForwardDistance = sonarFoward.ping_cm();
    lastForwardPingTime = millis();
    return(lastForwardDistance);
  }
  return(lastForwardDistance); 
}

boolean isOkToMoveForwards()
{
unsigned long currentForwardDistance = sonicForwardDistance();
if (currentForwardDistance==0||currentForwardDistance>Forward_Min_Distance) {
return(true);
}
return(false);
}

void forward(int speed) {
  digitalWrite(Left_Motor_IN1,HIGH);
  digitalWrite(Left_Motor_IN2,LOW);
  digitalWrite(Right_Motor_IN3,HIGH);
  digitalWrite(Right_Motor_IN4,LOW);
  analogWrite(Left_Motor_Enable,speed);
  analogWrite(Right_Motor_Enable,speed);  
}

void LEFT(int speed) {
  digitalWrite(Left_Motor_IN1,LOW);
  digitalWrite(Left_Motor_IN2,HIGH);
  digitalWrite(Right_Motor_IN3,HIGH);
  digitalWrite(Right_Motor_IN4,LOW);
  analogWrite(Left_Motor_Enable,speed);
  analogWrite(Right_Motor_Enable,speed);   
}

void brakesOn() {
  digitalWrite(Left_Motor_IN1,LOW);
  digitalWrite(Right_Motor_IN3,LOW);
  digitalWrite(Left_Motor_IN2,LOW);
  digitalWrite(Right_Motor_IN4,LOW);
  analogWrite(Left_Motor_Enable,255);
  analogWrite(Right_Motor_Enable,255); 
}
