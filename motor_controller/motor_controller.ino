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

int x_zero = 0;
int y_zero = 0;

void tic_initialize(){
  //initialize all the tic things - start I2C communication and get the tic ready.
  Wire.begin();
  delay(20);
  ticx.exitSafeStart();
  ticy.exitSafeStart();
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
}

void tic_toggle_energize(TicI2C tic){
  //change the energy state of the motor based on input from the tic and controlled by the joystick select button
  bool state = tic.getEnergized();
  if (state){
    tic.deenergize();
  }else {
    tic.energize();
  }
}

long transform(int analog_value){
  //do math on the analog value from the joystick. subtract so that once threshold is crossed, number starts from 0 and not threshold^2.
  int threshold_removed = analog_value - THRESHOLD;
  return (threshold_removed*threshold_removed);
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
    result = transform(analog_value_zeroed);
  }
  else if ((analog_value_zeroed < -THRESHOLD) && (low_lim == false)){
    result = -transform(analog_value_zeroed); //negative because joystick is moving in negative x/y
  }
  return (result);
}

void loop() {
  // function to feed number from joy_to_move into tic. 
  if (!digitalRead(SELECTBUTTON)){
    tic_toggle_energize(ticx);
    tic_toggle_energize(ticy);
  }
  long velocity_x = joy_to_move(XJOYSTICK, XHIGHLIM, XLOWLIM, x_zero);
  long velocity_y = joy_to_move(YJOYSTICK, YHIGHLIM, YLOWLIM, y_zero);
  ticx.setTargetVelocity(velocity_x);
  ticy.setTargetVelocity(velocity_y);
}

