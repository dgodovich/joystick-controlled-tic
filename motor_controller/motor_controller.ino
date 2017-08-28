#include <Tic.h>

#define XHIGHLIM 4
#define XLOWLIM 1
#define YHIGHLIM 2
#define YLOWLIM 3
#define XJOYSTICK 22
#define YJOYSTICK 21
#define SELECTBUTTON 0 // all pins were arbitrarily chosen
#define THRESHOLD 20 //dead space for joystick zero

TicI2C ticx(14);
TicI2C ticy(15);

unsigned long last_energize_toggle = 0;
int x_zero = 0;
int y_zero = 0;
bool select_state = true;
bool select_serviced = false;

void tic_initialize(){
  //initialize all the tic things - start I2C communication and get the tic ready.
  Wire.begin();
  delay(20);
  ticx.reset();
  ticy.reset();
  delay(20);  // wait after reset as i2c becomes unresponsive.
}

void setup() {
  // set all input pins for limits, joystick. Also set zero value for joystick when it is in neutral position
pinMode(XHIGHLIM, INPUT);
pinMode(XLOWLIM, INPUT);
pinMode(YHIGHLIM, INPUT);
pinMode(YLOWLIM, INPUT);
pinMode(XJOYSTICK, INPUT);
pinMode(YJOYSTICK, INPUT);
pinMode(SELECTBUTTON, INPUT_PULLUP); //when select button is pressed, it shorts to ground so input_pullup is the best option
x_zero = analogRead(XJOYSTICK);
y_zero = analogRead(YJOYSTICK);
tic_initialize();
last_energize_toggle = millis();
}

void tic_toggle_energize(TicI2C tic){
  //change the energy state of the motor based on input from the tic and controlled by the joystick select button
  bool state = tic.getEnergized();
  if (state){
    tic.deenergize();
  }else {
    tic.energize();
    tic.exitSafeStart();
  }
}

void tic_toggle_energize(TicI2C tic_x, TicI2C tic_y){
  /* this will toggle the energize state of both tics. It will keep the two tics synchronized - ie if one
  tic deenergizes without the other due to an error, the function will recogize this and deenergize both tics to
  keep them in the same state and prevent them from alternating between opposite states in subsequent calls.. */

  bool tic_x_energized = tic_x.getEnergized();
  bool tic_y_energized = tic_y.getEnergized();
  if (!tic_y_energized && !tic_x_energized){  // if both are deactivated, activate them.
    tic_y.energize();
    tic_x.energize();
    tic_y.exitSafeStart();
    tic_x.exitSafeStart();
  }
  else if (tic_y_energized ^ tic_x_energized){  // only one is energized, deenergize both to sync them.
    if (tic_y_energized){
      tic_y.deenergize();
    }
    else if (tic_x_energized) {
      tic_x.deenergize();
    }
  }
  else{  // both are energized, so deenergize them.
    tic_x.deenergize();
    tic_y.deenergize();
  }
  return;
}


long transform(int analog_value){
  /* do math on the analog value from the joystick. subtract so that once threshold is crossed, number starts from 0
  and not threshold^2. */
  int scalar = 45;  // empirically derived
  return (-pow(scalar * analog_value, 2));
}

long joy_to_move(int joystick_pin, int h_lim_pin, int l_lim_pin, int zero){
  /* parameters are the pins that you read the limits from, the direction joystick pin, and the zero value for the neutral position of the joystick.
   * read the limits and the analog value from the joystick. if statements check if joystick value is enough over the neutral position and if the limit
   * has not been reached. Call math function on the joystick value to generate a number which you feed into the tic in the next function.
   */ 
  long result = 0;
//  bool high_lim = digitalRead(h_lim_pin);
//  bool low_lim = digitalRead(l_lim_pin);
  bool high_lim = false;
  bool low_lim = false;
  int analog_value = analogRead(joystick_pin);
  int analog_value_zeroed = analog_value - zero;

  if ((analog_value_zeroed > THRESHOLD) && (high_lim == false)){
    result = transform(analog_value_zeroed - THRESHOLD);
  }
  else if ((analog_value_zeroed < -THRESHOLD) && (low_lim == false)){
    result = -transform(analog_value_zeroed + THRESHOLD); //negative because joystick is moving in negative x/y
  }
  return (result);
}

void loop() {
  // function to feed number from joy_to_move into tic. 
  select_state = digitalRead(SELECTBUTTON);
  if (!select_state && !select_serviced && (millis() - last_energize_toggle > 50)){
    tic_toggle_energize(ticx, ticy);
    last_energize_toggle = millis();
    select_serviced = true;  // we've already serviced this button press, so don't service it again.
  }
  if (select_serviced && select_state){
    select_serviced = false;  // the button is unpressed, so next time it is pressed we will service it.
  }
  
  long velocity_x = joy_to_move(XJOYSTICK, XHIGHLIM, XLOWLIM, x_zero);
  long velocity_y = joy_to_move(YJOYSTICK, YHIGHLIM, YLOWLIM, y_zero);
  ticx.setTargetVelocity(velocity_x);
  ticy.setTargetVelocity(velocity_y);
}

