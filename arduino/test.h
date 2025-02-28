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

void drawMehFace() {
  // eyes 
  tft.fillRect(60, 60, 30, 30, TFT_GREEN);
  tft.fillRect(150,60 ,30, 30, TFT_GREEN); 

  // meh mouth 
  tft.fillRect(20, 150, 200, 30, TFT_GREEN); 

  // tongue out 
  tft.fillRect(150, 180, 40, 40, TFT_RED); 
}
