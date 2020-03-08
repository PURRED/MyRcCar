#include <ir_Lego_PF_BitStreamEncoder.h>
#include <boarddefs.h>
#include <IRremoteInt.h>
#include <IRremote.h>

#include <Servo.h>

#define EA 3  // 모터 드라이버 EA핀
#define EB 11   // 모터 드라이버 EB핀

#define M_IN1 4   // 모터 드라이버 IN1
#define M_IN2 5   // 모터 드라이버 IN2

#define M_IN3 13   // 모터 드라이버 IN1
#define M_IN4 12   // 모터 드라이버 IN2

#define ECHO 6
#define TIG 7
#define SERVO_MOTOR 2

int motorA_vector = 1;
int motorB_vector = 1;    // 모터의 회전 방향이 반대일 시 0을 1로, 1을 0으로 바꿔주면 모터의 회전 방향이 바뀜

int motor_speed = 255;    // 모터 스피드

int RECV_PIN = 14; // 적외선 수신 센서 모듈과 A0 연결

char character;

IRrecv irrecv(RECV_PIN);    // 적외선 수신 핀 지정

decode_results results;

Servo servo;

int actionMode = 0;

void setup() {
  // put your setup code here, to run once:

  pinMode(EA, OUTPUT);
  pinMode(EB, OUTPUT);

  pinMode(M_IN1, OUTPUT);
  pinMode(M_IN2, OUTPUT);
  pinMode(M_IN3, OUTPUT);
  pinMode(M_IN4, OUTPUT);

  pinMode(TIG, OUTPUT);
  pinMode(ECHO, INPUT);

  servo.attach(SERVO_MOTOR);
  servo.write(90);

  delay(1000);

  Serial.begin(9600);

  irrecv.enableIRIn();  // 적외선 수신센서 활성화
}

void loop() {
  if (irrecv.decode(&results))
  { 
    //Serial.println(results.value, "HEX");

    switch (results.value)
    {
      case 0x00FF6897:
        actionMode = 1;
        break;

      case 0x00FF30CF:
        actionMode = 2;
        break;
      
      case 0x00FF18E7:
        actionMode = 0;
        motorA_con(motor_speed);
        motorB_con(motor_speed);
        break;

      case 0x00FF10EF:
        actionMode = 0;
        motorA_con(motor_speed);
        motorB_con(0);
        break;

      case 0x00FF38C7:
        actionMode = 0;
        motorA_con(0);
        motorB_con(0);
        break;

      case 0x00FF5AA5:
        actionMode = 0;
        motorA_con(0);
        motorB_con(motor_speed);
        break;

      case 0x00FF4AB5:
        actionMode = 0;
        motorA_con(-motor_speed);
        motorB_con(-motor_speed);
        break;
    }


    irrecv.resume();
  }

  if (actionMode == 1)
    {

      if (read_ultrasonic() < 25)
      {
        motorA_con(0);
        motorB_con(0);
        delay(280);
        
        motorA_con(motor_speed);
        motorB_con(-(motor_speed));

        delay(280);
        motorA_con(0);
        motorB_con(0);
        delay(280);
      }
      else {
        motorA_con(motor_speed);
        motorB_con(motor_speed);
      }
    }
    else if (actionMode == 2)
    {
      if (read_ultrasonic() > 16)
      {
        motorA_con(motor_speed);
        motorB_con(motor_speed);
      }
      else if (read_ultrasonic() < 14)
      {
        motorA_con(-motor_speed);
        motorB_con(-motor_speed); 
      }
      else
      {
        motorA_con(0);
        motorB_con(0); 
      }
    }
}

int read_ultrasonic(void)
{
   
  float return_time, howlong; // 시간 값과 거리 값을 저장하는 변수

  // 초음파 센서는 초음파를 발사 후 돌아오는 시간을 역산하여 거리 값을 계산한다.
  digitalWrite(TIG, HIGH);  // 초음파 발사
//  delayMicroseconds(10);
  delay(5);
  digitalWrite(TIG, LOW);  // 초음파 발사 정지

  //unsigned long duration = pulseIn(ECHO, HIGH);

   // float distance = duration / 29.0 / 2.0;
 
  return_time = pulseIn(ECHO, HIGH);

  howlong = ((float)(340 * return_time) / 10000) /2;

  // return distance;
  return howlong;
}

void motorA_con(int M1)
{
  //Serial.println(M1, "HEX");
  if (M1 > 0) // M1 이 양수일 때, 모터 A 정회전
  {
    digitalWrite(M_IN1, motorA_vector);
    digitalWrite(M_IN2, !motorA_vector);
  }
  else if (M1 < 0)    // M1 이 음수일 떄, 모터 A 역회전
  {
    digitalWrite(M_IN1, !motorA_vector);
    digitalWrite(M_IN2, motorA_vector);
  }
  else // 모터 A 정지
  {
    digitalWrite(M_IN1, LOW);
    digitalWrite(M_IN2, LOW);
  }

  analogWrite(EA, abs(M1));   // M1의 절대값으로 모터 A 속더 제어
}

void motorB_con(int M1)
{
  if (M1 > 0) // M1 이 양수일 때, 모터 A 정회전
  {
    digitalWrite(M_IN3, motorB_vector);
    digitalWrite(M_IN4, !motorB_vector);
  }
  else if (M1 < 0)    // M1 이 음수일 떄, 모터 A 역회전
  {
    digitalWrite(M_IN3, !motorB_vector);
    digitalWrite(M_IN4, motorB_vector);
  }
  else // 모터 A 정지
  {
    digitalWrite(M_IN3, LOW);
    digitalWrite(M_IN4, LOW);
  }

  analogWrite(EB, abs(M1));   // M1의 절대값으로 모터 A 속더 제어
}
