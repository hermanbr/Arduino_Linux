#define X_AXIS A4
#define Y_AXIS A5
#define BUTTON_PIN 2

int x_value = 0;
int y_value = 0;
int button_value = 0;



#define TRESHOLD 50
#define LOW_END 512-TRESHOLD
#define HIGH_END 512+TRESHOLD

struct __attribute__((packed)) serial_message {
    int16_t x_axis;
    int16_t y_axis;
    uint16_t button_pushed;
}; 


void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600); 


}

//int map_to_axis(int )

void loop() {
  // put your main code here, to run repeatedly:

  button_value = digitalRead(BUTTON_PIN);
  x_value = analogRead(X_AXIS);
  y_value = analogRead(Y_AXIS);
  struct serial_message msg_t;

  // Now change to only 5pixel values
  /* TODO 
    Have higher levels if higher voltage  
  */

  if(x_value < LOW_END){
    /* Map to value between -10 - 0 */
    long mapped_value = map(x_value, 0, LOW_END, -20, 0);
    msg_t.x_axis =  (int)mapped_value;
  }
  else if (x_value > HIGH_END) { /*Range 0-10*/
    long mapped_value = map(x_value, HIGH_END, 1024, 0, 20);
    msg_t.x_axis = (int)mapped_value;
  }
  else{ /* In range for 0*/
    msg_t.x_axis = 0;
  }

 
  /*Y VA*/
    if(y_value < LOW_END){
    /* Map to value between -10 - 0 */
    long mapped_value = map(y_value, 0, LOW_END, 20, 0);
    msg_t.y_axis =  (int)mapped_value;
  }
  else if (y_value > HIGH_END) { /*Range 0-10*/
    long mapped_value = map(y_value, HIGH_END, 1024, 0, -20);
    msg_t.y_axis = (int)mapped_value;
  }
  else{ /* In range for 0*/
    msg_t.y_axis = 0;
  }
  


 // msg_t.x_axis = x_value;
 
  msg_t.button_pushed = button_value;
  // Serial.print("x: ");
  // Serial.println(x_value);
  // Serial.print("y: ");;
  // Serial.println(y_value);
  Serial.write((const uint8_t*)&msg_t, sizeof(struct serial_message));
  // char *msg = "hello_world\n";
  // Serial.write(msg, 28);
  delay(10);

}
