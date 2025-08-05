#define X_AXIS A4
#define Y_AXIS A5
#define BUTTON_PIN 2

int x_value = 0;
int y_value = 0;
int button_value = 0;

struct __attribute__((packed)) serial_message {
    uint16_t x_axis;
    uint16_t y_axis;
    uint16_t button_pushed;
}; 


void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600); 


}

void loop() {
  // put your main code here, to run repeatedly:

  button_value = digitalRead(BUTTON_PIN);
  x_value = analogRead(X_AXIS);
  y_value = analogRead(Y_AXIS);
  struct serial_message msg_t;


  msg_t.x_axis = x_value;
  msg_t.y_axis = y_value; 
  msg_t.button_pushed = button_value;

  Serial.write((const uint8_t*)&msg_t, sizeof(struct serial_message));
  // char *msg = "hello_world\n";
  // Serial.write(msg, 28);
  delay(10);

}
