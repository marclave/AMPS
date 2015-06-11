
#define PWM_OUTPUT_PIN_LS 3
#define PWM_OUTPUT_PIN_RS 5
#define MAX_DUTY_CYCLE 255

bool dir = 0;
void setup() {
  Serial.begin(9600);
}

void loop() {
  int time = millis();
  Serial.println(time);
  if (time > 5000)
  {
  analogWrite(PWM_OUTPUT_PIN_LS, 255);
  analogWrite(PWM_OUTPUT_PIN_RS, 0);
  }
  else
  {
    analogWrite(PWM_OUTPUT_PIN_LS, 0);
    analogWrite(PWM_OUTPUT_PIN_RS, 255);
  }
    
}

