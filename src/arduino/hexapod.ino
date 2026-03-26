#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_VL53L0X.h>

Adafruit_PWMServoDriver pwm(0x40);
Adafruit_VL53L0X lox;

#define SERVOMIN 150
#define SERVOMAX 600

int command = 0;

// ===== SERVO =====
void setServo(int ch, int angle){
  int pulse = map(angle,0,180,SERVOMIN,SERVOMAX);
  pwm.setPWM(ch,0,pulse);
}

// ===== MOVEMENT =====
void forward(){
  setServo(0,120);
  setServo(1,100);
  delay(200);
}

void left(){
  setServo(0,140);
  delay(200);
}

void right(){
  setServo(0,60);
  delay(200);
}

void stopAll(){
  for(int i=0;i<16;i++){
    pwm.setPWM(i,0,0);
  }
}

// ===== SETUP =====
void setup(){
  Serial.begin(115200);

  pwm.begin();
  pwm.setPWMFreq(50);

  if(!lox.begin()){
    while(1);
  }
}

// ===== LOOP =====
void loop(){

  // read command
  if(Serial.available()){
    char c = Serial.read();

    if(c=='F') command=1;
    else if(c=='L') command=2;
    else if(c=='R') command=3;
    else if(c=='S') command=0;
  }

  // distance
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure,false);

  float dist = measure.RangeMilliMeter/10.0;

  if(dist>0 && dist<30){
    stopAll();
    return;
  }

  // execute
  if(command==1) forward();
  else if(command==2) left();
  else if(command==3) right();
  else stopAll();
}
