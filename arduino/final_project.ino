#include <Servo.h>
#include <SPI.h>
#include <TFT_eSPI.h>

// face 
int led_R = 3;
int led_L = 3;
int fadeAmount = 5;

// connecting the screen 
#define TFT_CS   10  // Chip Select pin
#define TFT_DC   9   // Data/Command pin
#define TFT_RST  8   // Reset pin
TFT_eSPI tft = TFT_eSPI();


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
  pinMode(led_R, OUTPUT);
  
  // screen 
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  // arms 
  servo_R.attach(5); 
  servo_L.attach(6); 
  // Serial.begin(9600);
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
  for (int led_value=0; led_value <=255; led_value+=fadeAmount){
    analogWrite(led_R, led_value); 
    analogWrite(led_L, led_value); 
    delay(15); 
  }

  for (int led_value=255; led_value >= 0; led_value -= fadeAmount){
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

  // test blushing 
  // blush(); 

// draw face 
// eyes 
tft.fillRect(60, 60, 30, 30, TFT_GREEN);
tft.fillRect(150,60 ,30, 30, TFT_GREEN); 

// meh mouth 
tft.fillRect(20, 150, 200, 30, TFT_GREEN); 

// tongue out 
tft.fillRect(150, 180, 40, 40, TFT_RED); 

  // // write the position outputs of the pots
  // val_R = analogRead(pot_R);
  // val_L = analogRead(pot_L); 

  // Serial.print(val_R);
  // Serial.print(",");
  // Serial.println(val_L); 

  delay(3*1000); 
}
