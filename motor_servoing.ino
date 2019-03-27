#include <SPI.h>

#define CLOCKWISE -1
#define COUNTERCLOCKWISE 1
#define PINA 5
#define PINB 11
#define serial 0
#define ir_2   6
#define ir_1   12
#define delayInterval 10
#define ticks_per_revolution 8
#define degrees_per_tick    0.6

//tracking current state 
enum statelist {NONE, TOP, BOTTOM, BOTH};
uint8_t state = NONE; 
uint8_t lastState = NONE; 

// track position of encoder
volatile long encoderPosition = 0; 
volatile long tickCount = 0; 
float currentAngle = 0; 

//track direction of wheel 
int currentDirection = CLOCKWISE; 
float desiredPosition;

//used for tracking motor speed
float vel = 0.0; 

//Proportional constant for feedback loop
int kp = 2; 

//timers for waiting time to pass before next interrupt
unsigned long tick = 0; 
unsigned long tock = 0; 
unsigned long timelapsed = tick - tock; 
void setup() {
  Serial.begin(9600);
  SPI.begin();
  pinMode(PINA, OUTPUT); 
  pinMode(PINB, OUTPUT); 
  attachInterrupt(digitalPinToInterrupt(ir_1), tickInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ir_2), tickInterrupt, RISING);
  Serial.print("Enter a value from -1.0 - 1.0 to set motor speed\n");
  Serial.print("-1.0 - full backwards speed \n0 - stops motor \n1.0 - full forward speed");
  Serial.print("Inputting an invalid character will result in 0 value being set\n");
}
  
int setSpeed(float s){
  if(s > 1.0) s = 1.0; 
  if(s < 0.0) s = 0.0; 
  int out = (int) (s*255.0); 
  return out; 
}

int getSpeed(){
  
}

void setVelocity(float velocity){
  float s = velocity; 
  if(s < 0) s = -s; 
  int sp = setSpeed(s); 
    
  if(velocity >= 0){ //forward
    analogWrite(PINB, 0);
    //digitalWrite(PINB, LOW); //important to set low first to avoid shorting
    analogWrite(PINA, sp);
  }
  else { //backward
    analogWrite(PINA, 0);
    //digitalWrite(PINA, LOW);
    analogWrite(PINB, sp);    
   
  }
}

void tickInterrupt()
{
  //ir1 is MSB
  tick = millis();
  int ir1 = digitalRead(ir_1);
  int ir2 = digitalRead(ir_2); 
  Serial.println("IR 1: " + String(ir1));
  Serial.println("IR 2: " + String(ir2));
  if(ir1 == '1' && ir2 == '1') state = BOTH;
  else if(ir1 == 1 && ir2 == 0) state = TOP;
  else if(ir1 == 0 && ir2 == 1) state = BOTTOM; 
  else state = NONE; 

  switch(state){ //determine direction from previous state
    tick = millis();
    case NONE:
      if(lastState == TOP) currentDirection = CLOCKWISE; 
      else currentDirection = COUNTERCLOCKWISE; 
      break;
    case TOP:
      if(lastState == BOTH) currentDirection = CLOCKWISE; 
      else currentDirection = COUNTERCLOCKWISE;
      break; 
    case BOTTOM:
      if(lastState == NONE) currentDirection = CLOCKWISE; 
      else currentDirection = COUNTERCLOCKWISE;  
      break;
    case BOTH: 
      if(lastState == BOTTOM) currentDirection = CLOCKWISE; 
      else currentDirection = COUNTERCLOCKWISE; 
      break; 
  }
  lastState = state; //update last state
  currentAngle += currentDirection*degrees_per_tick; //calculate angle traveled
  Serial.print("Calculated angle ");
  Serial.println(currentAngle);
  if((2*degrees_per_tick + desiredPosition) >= currentAngle && currentAngle >= (desiredPosition - 2*degrees_per_tick))
      {
        setVelocity(0); 
        Serial.println("Stop");
 
      } 
  tock = millis();
}

void loop() { 
  boolean in_range = false; 
  while(Serial.available()){
    desiredPosition = Serial.parseFloat(); 
    Serial.print("Desired position ");
    Serial.println(desiredPosition);

    //if desired position is behind current position
    if(desiredPosition - currentAngle < 0) setVelocity(-0.6); 
    else setVelocity(0.6); 
    
    }
  }
  
