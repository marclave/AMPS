#define IR_EMITTER_PIN 2
#define IR_RECIEVER_PIN 0
#define LED_SIGNAL_PIN 13

volatile int val = 0;
void setup() {
  pinMode(IR_EMITTER_PIN, OUTPUT);
  pinMode(LED_SIGNAL_PIN, OUTPUT);
  digitalWrite(IR_EMITTER_PIN, HIGH);
  Serial.begin(9600);
}

void loop() {
  val = analogRead(IR_RECIEVER_PIN);
  Serial.print("Value is: ");
  Serial.println(val);
  delay(500);
}
