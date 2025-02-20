#include <Servo.h>

// face 
int led_R = 2;
int led_L = 4;
int fadeAmount = 5;
// connecting the screen 


// arms 
Servo servo_R;
Servo servo_L; 
int pot_R = A1; 
int pot_L = A0; 
int val_R; 
int val_L; 

int neutral_degree = 90;
int current_L = 90; 
int current_R = 90; 

// torso sound 


void setup() {
  // face 
  // pinMode(led_R, OUTPUT);
  // pinMode(led_L, OUTPUT);

  // arms 
  servo_R.attach(10); 
  servo_L.attach(11); 
  Serial.begin(9600);

}

void one_hand_wave(int times = 2){
  // wave the right hand up and down x times 
  for (int t = 0; t <= times; t+=1){
    for (int out_degrees=0; out_degrees <= 20 ; out_degrees +=2){
      current_R = current_R + out_degrees; 
      servo_R.write(current_R); 
      delay(100); 
    }

    for (int out_degrees=20; out_degrees <= 0 ; out_degrees -=2){
      current_R = current_R - out_degrees; 
      servo_R.write(current_R); 
      delay(100); 
    }
    current_R = neutral_degree;
  }
}

void two_hand_wave(int times = 2){
// wave both hands up and down x times
  for (int t = 0; t <= times; t+=1){
    for (int out_degrees=0; out_degrees <= 20 ; out_degrees +=2){
      current_R = current_R + out_degrees; 
      current_L = current_L + out_degrees; 
      servo_R.write(current_R); 
      delay(15);
      servo_L.write(current_L); 
      delay(100); 
    }

    // back to neutral 
    for (int out_degrees=20; out_degrees <= 0 ; out_degrees -=2){
      current_R = current_R - out_degrees; 
      current_L = current_L - out_degrees; 
      servo_R.write(current_R); 
      delay(15); 
      servo_L.write(current_L); 
      delay(100); 
    }
    current_R = neutral_degree;
    current_L = neutral_degree;  
  }
}

void blush(){
  int led_value; 
  for (led_value=0; led_value <=255; led_value+=fadeAmount){
    analogWrite(led_R, led_value); 
    analogWrite(led_L, led_value); 
    delay(15); 
  }

  for (led_value=255; led_value >= 0; led_value -= fadeAmount){
    analogWrite(led_R, led_value); 
    analogWrite(led_L, led_value); 
    delay(15); 
  }

}


void loop() {

  // set initial values for servo
  servo_R.write(neutral_degree); 
  delay(15); 
  servo_L.write(neutral_degree); 
  delay(15); 

  // test some waving 
  // two_hand_wave(); 
  // one_hand_wave();

  // receive serial communication from the client

  // write the position outputs of the pots
  val_R = analogRead(pot_R);
  val_L = analogRead(pot_L); 

  Serial.print(val_R);
  Serial.print(",");
  Serial.println(val_L); 

  delay(3*1000); 
}
