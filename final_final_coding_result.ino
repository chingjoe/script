// Include the libraries:
// LiquidCrystal_I2C.h: https://github.com/johnrickman/LiquidCrystal_I2C
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
#include <Servo.h>

// Wiring: SDA pin is connected to A4 and SCL pin to A5.
// Connect to LCD via I2C, default address 0x27 (A0-A2 not jumpered)
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x3F, 16, 2); // Change to (0x27,20,4) for 20x4 LCD.

 // pushbutton pin
 int up = 8;              //RESET button
 int down = 10;           //spray button

 const int servoPin = 9;
 Servo servo;            //Servo control on/off switch of spray
 const int servoPin2 = 11;
 Servo servo2;           //Servo2 control volume of spray

 int angle_counter = 1;
 int servo_angle = 0;   //Servo turning angle
 int servo2_angle = 0;

boolean current_up = LOW;   //reset switch (current data)      
boolean last_up= LOW;       //reset switch (last data)
boolean last_down = LOW;    //spray switch (current data)
boolean current_down = LOW; //spray switch (last data)

//Counter to change positions of pages
int page_counter = 1 ;       //To move between mode
int counter = 0;             //counter for rotary encoder switch
int counter1 = 0;
int counter2 = 0;
int counter3 = 0;
int counter4 = 0;
int counter5 = 0;
int counter6 = 0;
int submenu = 0;            
int button_counter = 0;
bool rotated = true;          //to control turning of rotary enconder switch
bool ButtonPressed = false;   //to control pressing button of rotary encoder switch

//pulse setting
int no_of_pulse = 0;
int pulse_duration = 0;
int pulse_duration_check = 0;
int pulse_delay = 0;
int pulse_delay_btw12 = 0;
int pulse_delay_btw23 = 0;
int pulse_delay_btw34 = 0;
int half_volume = 0;
int full_volume = 0;

const int PinCLK = 2;         //pin setting of rotary encoder switch
const int PinDT = 4;
const int PinSW = 3;


// Statuses
int CLKNow;   
int CLKPrevious;
int DTNow;
int DTPrevious;

//Timers
float TimeNow1;
float TimeNow2;


  //make custom arrow character
  byte left_arrow[] = {
    B00000,
    B00001,
    B00011,
    B00111,
    B01111,
    B00111,
    B00011,
    B00001
  };

  byte right_arrow[] = {
    B00000,
    B10000,
    B11000,
    B11100,
    B11110,
    B11100,
    B11000,
    B10000
  };
  
void setup() {
  Serial.begin(2000000);      //serial monitor setup
  servo.attach (servoPin);    //servo motor setup
  servo.write(30);
  servo2.attach (servoPin2);
  servo2.write(0);
    // Put current pins state in variables
  //CLKPrevious=digitalRead(PinCLK);
  //DTPrevious=digitalRead(PinDT);
    
  // Initiate the LCD:
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, left_arrow);
  lcd.createChar(1, right_arrow);
  
  lcd.setCursor(15,0);
  lcd.print("WELCOME!");
  for (int i = 0; i<11; i++) {
    lcd.scrollDisplayLeft();
    delay(500);
  }
  delay(100);
  lcd.clear();
  delay(100);

  lcd.setCursor(0,0);
  lcd.print("Please Select");
  lcd.setCursor(0,1);
  lcd.print("a mode");
  delay(1000);
  lcd.clear();
  delay(500);

  // Set the Switch pin to use Arduino PULLUP resistors
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);

  //Store states
  CLKPrevious = digitalRead(PinCLK);
  DTPrevious = digitalRead(PinDT);

  attachInterrupt(digitalPinToInterrupt(PinCLK), rotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PinSW), buttonPressed, FALLING);

  TimeNow1 = millis(); //Start timer 1 
  
  
}

//---- De-bouncing function for all buttons----//
boolean debounce(boolean last, int pin)
{
boolean current = digitalRead(pin);
if (last != current)
{
delay(5);
current = digitalRead(pin);
}
return current;
}

void loop() {

      //check for the clk pin and DT pin of rotary encoder
      Serial.print("CLK Pin: ");
      Serial.println(digitalRead(PinCLK));
      Serial.print("DT Pin: ");
      Serial.println(digitalRead(PinDT));
      delay(5);

  SubmenuChecker();
  Serial.print("page counter: ");   //check current page counter
  Serial.println(page_counter);
  //rotated = false; 
  Serial.print("submenu: ");        //check current submenu
  Serial.println(submenu);
  ButtonChecker();
  SubmenuChecker();
  Serial.println(no_of_pulse);      //check current no of pulse selected
  Serial.print("Counter1: ");       //check current counter1
  Serial.println(counter1);
  Serial.print("Pulse Duration: "); //check current pulse duration selected
  Serial.println(pulse_duration);
  durationcheck();
  durationcheck1();
  durationcheck2();
  delaycheck();
  eachdelaycheck1();

  current_up = debounce(last_up, up);         //Debounce for RESET button
  current_down = debounce(last_down, down);   //Debounce for SPRAY button

      //if the RESET button pressed
      if (last_up == LOW && current_up == HIGH){    
        submenu = 0;
        page_counter = 0;
        counter = 0;
        counter1 = 0;
        counter2 = 0;
        counter3 = 0;
        counter4 = 0;
        counter5 = 0;
        counter6 = 0;
        servo_angle = 30;
        //servo.write(0);
        //servo2.write(0);
        servo2_angle = 0;
        no_of_pulse = 0;
        pulse_duration = 0;
        pulse_delay = 0;
        pulse_delay_btw12 = 0;
        pulse_delay_btw23 = 0;
        pulse_delay_btw34 = 0;
        half_volume = 0;
        full_volume = 0;
        SubmenuChecker();
     }
  
    last_up = current_up;

    //if the SPRAY button pressed
    if (last_down== LOW && current_down == HIGH){    
      //spray();
      servocheck();
      /*if(page_counter != 1) {
        if(servo2_angle == 0) {
          servo2_angle = 180;
          servo2.write(servo2_angle);
        } else {
          servo2_angle = 0;
          servo2.write(servo2_angle);
        }
      }
      else if(page_counter == 1) {
        if(servo_angle == 0) {
          servo_angle = 180;
          servo.write(servo_angle);
        } else {
          servo_angle = 0;
          servo.write(servo_angle);
        }
      }*/
      //delay(1000);
    }
    last_down = current_down;
}

//rotary encoder press switch part (timer and debounce)
void buttonPressed() {
  TimeNow2 = millis();
  if(TimeNow2 - TimeNow1 > 500) {
    ButtonPressed = true;
  }
  TimeNow1 = millis();
}

void rotate() {
  CLKNow = digitalRead(PinCLK); //Read the state of the CLK pin

  if(CLKNow != CLKPrevious && CLKNow == 1) {
    if(submenu == 0){                     //in submenu 1--> page of choosing mode
    if(digitalRead(PinDT) != CLKNow) {
      if(page_counter < 4) {      //rotary encoder turn CW the page counter ++, max of page/mode = 4
        page_counter++;
      }else {
        page_counter = 4;
      }
    }else{
      if(page_counter > 1) {      //rotary encoder turn CCW page counter --, min = 1
        page_counter--;
      }else {
        page_counter = 1;
      }
    }
   }
   if(submenu == 1){             //submenu 1
   if(digitalRead(PinDT) != CLKNow) {
      if(counter < 6) {
        counter++;               //rotary encoder turn CW --> counter++
      }else {
        counter = 6;             //max counter = 6
      }
    }else{
      if(counter > 1) {          //rotary encoder turn CCW --> counter--
        counter--;
      }else {
        counter = 1;             //min counter = 1
      }
    }
   }
   if(submenu == 2){            //submenu 2
   if(digitalRead(PinDT) != CLKNow) {
      if(counter < 22) {        //rotary encoder turn CW --> counter++
        counter++;
      }else {
        counter = 22;           //max counter = 22
      }
      if(counter2 < 14) {
        counter2++;             //rotary encoder turn CW --> counter2++
      }else {
        counter2 = 14;          //max counter2 = 14
      }
      if(counter3 < 10) {
        counter3++;             //rotary encoder turn CW --> counter3++
      }else {
        counter3 = 10;          //max counter3 = 10
      }
    }else{
      if(counter > 1) {
        counter--;              //rotary encoder turn CCW --> counter--
      }else {
        counter = 1;            //min counter = 1
      }
      if(counter2 > 1) {        //rotary encoder turn CCW --> counter2--
        counter2--;
      }else {
        counter2 = 1;           //min counter2 = 1
      }
      if(counter3 > 1) {        //rotary encoder turn CCW --> counter3--
        counter3--;
      }else {
        counter3 = 1;           //min counter3 = 1
      }
    }
   }
   if(submenu == 3){            //submenu 3
   if(digitalRead(PinDT) != CLKNow) {
      if(counter1 < 14) {
        counter1++;             //rotary encoder turn CW --> counter1++
      }else {
        counter1 = 14;          //max counter1 = 14
      }
    }else{
      if(counter1 > 1) {
        counter1--;             //rotary encoder turn CCW --> counter1--
      }else {
        counter1 = 1;           //min counter1 = 1
      }
    }
   }
   if(submenu == 4){            //submenu 4
   if(digitalRead(PinDT) != CLKNow) {
      if(counter4 < 14) {
        counter4++;             //rotary encoder turn CW --> counter4++
      }else {
        counter4 = 14;          //max counter4 = 14
      }
    }else{
      if(counter4 > 1) {
        counter4--;             //rotary encoder turn CCW --> counter4--
      }else {
        counter4 = 1;           //min counter4 = 1
      }
    }
   }
   if(submenu == 5){            //submenu 5
   if(digitalRead(PinDT) != CLKNow) {
      if(counter5 < 14) {
        counter5++;             //rotary encoder turn CW --> counter5++
      }else {
        counter5 = 14;          //max counter5 = 14
      }
    }else{
      if(counter5 > 1) {
        counter5--;             //rotary encoder turn CCW --> counter5--
      }else {
        counter5 = 1;           //min counter5 = 1
      }
    }
   }
   if(submenu == 6){            //submenu 6
   if(digitalRead(PinDT) != CLKNow) {
      if(counter6 < 14) {
        counter6++;             //rotary encoder turn CW --> counter6++
      }else {
        counter6 = 14;          //max counter6 = 14
      }
    }else{
      if(counter6 > 1) {
        counter6--;             //rotary encoder turn CCW --> counter6--
      }else {
        counter6 = 1;           //min counter6 = 1
      }
    }
   }
  }
  CLKPrevious = CLKNow; //Store last CLK state
  rotated = true;
}

void ButtonChecker() {
  if(ButtonPressed == true) {
    if(submenu < 6) {
      submenu++;                //rotary encoder pressed --> submenu++
      Serial.println(submenu);
    }else{
      submenu--;
      Serial.println(submenu);
    }
  }
  ButtonPressed = false;
}

//servo motor control
void servocheck() {
  if(page_counter != 1) {             //when page counter not equal 1 (mode 2, 3, 4)
        servo2_angle = 0;
        if(servo2_angle == 0) {       //servo motor(control volume) = 0, then turn 60 degree
          delay(100);                 //delay 0.1s
          servo2_angle = 60;
          servo2.write(servo2_angle);
          spray();                    //call spray function
          servo2.write(0);
        } /*else {
          servo2_angle = 0;
          servo2.write(servo2_angle);
          spray();
        }*/
      }
      else if(page_counter == 1) {    //when page counter equal 1 (mode 1)
        if(servo_angle == 0) {        //servo motor(control spray switch) = 0, then turn 180 degree
          for(servo_angle = 0; servo_angle < 30; servo_angle++) {
              servo.write(servo_angle);
            }
          spray();                    //call spray function
        } else {
          servo_angle = 0;
          servo.write(servo_angle);
          spray();
        }
      }
}

//spray function
void spray() {
  if(page_counter == 1) {                 //page counter 1 / mode 1 
        //servo2.write(0);
        if(half_volume == 1) {            //if half volume selected
          if(servo2_angle == 0) {         //servo motor(control volume) = 0, then turn 90 degree when the spray button pressed
            for(servo2_angle = 0; servo2_angle < 60; servo2_angle++) {
              servo2.write(servo2_angle);
              //delay(100);
            }
            //servo2_angle = 90;
            //servo2.write(servo2_angle);
          }else {
            servo2_angle = 0;             //servo motor(control volume) = 90, then turn 0 degree when the spray button pressed
            servo2.write(servo2_angle);
          }
        }else if(full_volume == 1) {      //if full volume selected
          if(servo2_angle == 0) {         //servo motor(control volume) = 0, then turn 180 degree when the spray button pressed
            for(servo2_angle = 0; servo2_angle < 90; servo2_angle++) {
              servo2.write(servo2_angle);
              //delay(100);
            }
            //servo2_angle = 180;
            //servo2.write(servo2_angle);
          }else {
            servo2_angle = 0;             //servo motor(control volume) = 180, then turn 0 degree when the spray button pressed
            servo2.write(servo2_angle);
          }
        }
      }  
      else if(page_counter == 2) {        //page counter 2 / mode 2 (one pulse)
        spraycheck();                     //call spraycheck function
      }
      else if(page_counter == 3) {        //page counter 3 / mode 3 
        if(no_of_pulse == 1) {            //1 pulse selected(no of pulse = 1)
        spraycheck();
        }
        else if(no_of_pulse == 2) {       //2 pulses seleted 
          if(pulse_delay == 0) {          //pulse delay duration btw pulse is zero
            for(int i=0; i<2; i++) {
              spraycheck();
            }
          }
          else if(pulse_delay == 1) {     //pulse delay duration btw pulse is 0.1s
            for(int i=0; i<2; i++) {
              spraycheck();
              delay(100);
            }
          }
          else if(pulse_delay == 2) {     //pulse delay duration btw pulse is 0.5s
            for(int i=0; i<2; i++) {
              spraycheck();
              delay(500);
            }
          }
          else if(pulse_delay == 3) {     //pulse delay duration btw pulse is 1.0s
            for(int i=0; i<2; i++) {
              spraycheck();
              delay(1000);
            }
          }
          else if(pulse_delay == 4) {     //pulse delay duration btw pulse is 1.5s
            for(int i=0; i<2; i++) {
              spraycheck();
              delay(1500);
            }
          }
          else if(pulse_delay == 5) {     //pulse delay duration btw pulse is 2.0s
            for(int i=0; i<2; i++) {
              spraycheck();
              delay(2000);
            }
          }
          else if(pulse_delay == 6) {     //pulse delay duration btw pulse is 2.5s
            for(int i=0; i<2; i++) {
              spraycheck();
              delay(2500);
            }
          }
        }
        else if(no_of_pulse == 3) {       //3 pulses selected
          if(pulse_delay == 0) {
            for(int i=0; i<3; i++) {
              spraycheck();
            }
          }
          else if(pulse_delay == 1) {
            for(int i=0; i<3; i++) {
              spraycheck();
              delay(100);
            }
          }
          else if(pulse_delay == 2) {
            for(int i=0; i<3; i++) {
              spraycheck();
              delay(500);
            }
          }
          else if(pulse_delay == 3) {
            for(int i=0; i<3; i++) {
              spraycheck();
              delay(1000);
            }
          }
          else if(pulse_delay == 4) {
            for(int i=0; i<3; i++) {
              spraycheck();
              delay(1500);
            }
          }
          else if(pulse_delay == 5) {
            for(int i=0; i<3; i++) {
              spraycheck();
              delay(2000);
            }
          }
          else if(pulse_delay == 6) {
            for(int i=0; i<3; i++) {
              spraycheck();
              delay(2500);
            }
          }
        }
        else if(no_of_pulse == 4) {       //4 pulses selected
          if(pulse_delay == 0) {
            for(int i=0; i<4; i++) {
              spraycheck();
            }
          }
          else if(pulse_delay == 1) {
            for(int i=0; i<4; i++) {
              spraycheck();
              delay(100);
            }
          }
          else if(pulse_delay == 2) {
            for(int i=0; i<4; i++) {
              spraycheck();
              delay(500);
            }
          }
          else if(pulse_delay == 4) {
            for(int i=0; i<4; i++) {
              spraycheck();
              delay(1000);
            }
          }
          else if(pulse_delay == 4) {
            for(int i=0; i<4; i++) {
              spraycheck();
              delay(1500);
            }
          }
          else if(pulse_delay == 5) {
            for(int i=0; i<4; i++) {
              spraycheck();
              delay(2000);
            }
          }
          else if(pulse_delay == 6) {
            for(int i=0; i<4; i++) {
              spraycheck();
              delay(2500);
            }
          }
        }
        else if(no_of_pulse == 5) {       //5 pulses selected
          if(pulse_delay == 0) {
            for(int i=0; i<5; i++) {
              spraycheck();
            }
          }
          else if(pulse_delay == 1) {
            for(int i=0; i<5; i++) {
              spraycheck();
              delay(100);
            }
          }
          else if(pulse_delay == 2) {
            for(int i=0; i<5; i++) {
              spraycheck();
              delay(500);
            }
          }
          else if(pulse_delay == 3) {
            for(int i=0; i<5; i++) {
              spraycheck();
              delay(1000);
            }
          }
          else if(pulse_delay == 4) {
            for(int i=0; i<5; i++) {
              spraycheck();
              delay(1500);
            }
          }
          else if(pulse_delay == 5) {
            for(int i=0; i<5; i++) {
              spraycheck();
              delay(2000);
            }
          }
          else if(pulse_delay == 6) {
            for(int i=0; i<5; i++) {
              spraycheck();
              delay(2500);
            }
          }
        }
        else if(no_of_pulse == 6) {       //6 pulses selected
          if(pulse_delay == 0) {
            for(int i=0; i<6; i++) {
              spraycheck();
            }
          }
          else if(pulse_delay == 1) {
            for(int i=0; i<6; i++) {
              spraycheck();
              delay(100);
            }
          }
          else if(pulse_delay == 2) {
            for(int i=0; i<6; i++) {
              spraycheck();
              delay(500);
            }
          }
          else if(pulse_delay == 3) {
            for(int i=0; i<6; i++) {
              spraycheck();
              delay(1000);
            }
          }
          else if(pulse_delay == 4) {
            for(int i=0; i<6; i++) {
              spraycheck();
              delay(1500);
            }
          }
          else if(pulse_delay == 5) {
            for(int i=0; i<6; i++) {
              spraycheck();
              delay(2000);
            }
          }
          else if(pulse_delay == 6) {
            for(int i=0; i<6; i++) {
              spraycheck();
              delay(2500);
            }
          }
        }
        else if(no_of_pulse == 7) {       //7 pulses selected
          if(pulse_delay == 0) {
            for(int i=0; i<7; i++) {
              spraycheck();
            }
          }
          else if(pulse_delay == 1) {
            for(int i=0; i<7; i++) {
              spraycheck();
              delay(100);
            }
          }
          else if(pulse_delay == 2) {
            for(int i=0; i<7; i++) {
              spraycheck();
              delay(500);
            }
          }
          else if(pulse_delay == 3) {
            for(int i=0; i<7; i++) {
              spraycheck();
              delay(1000);
            }
          }
          else if(pulse_delay == 4) {
            for(int i=0; i<7; i++) {
              spraycheck();
              delay(1500);
            }
          }
          else if(pulse_delay == 5) {
            for(int i=0; i<7; i++) {
              spraycheck();
              delay(2000);
            }
          }
          else if(pulse_delay == 6) {
            for(int i=0; i<7; i++) {
              spraycheck();
              delay(2500);
            }
          }
        }
        else if(no_of_pulse == 8) {       //8 pulses selected
          if(pulse_delay == 0) {
            for(int i=0; i<8; i++) {
              spraycheck();
            }
          }
          else if(pulse_delay == 1) {
            for(int i=0; i<8; i++) {
              spraycheck();
              delay(100);
            }
          }
          else if(pulse_delay == 2) {
            for(int i=0; i<8; i++) {
              spraycheck();
              delay(500);
            }
          }
          else if(pulse_delay == 3) {
            for(int i=0; i<8; i++) {
              spraycheck();
              delay(1000);
            }
          }
          else if(pulse_delay == 4) {
            for(int i=0; i<8; i++) {
              spraycheck();
              delay(1500);
            }
          }
          else if(pulse_delay == 5) {
            for(int i=0; i<8; i++) {
              spraycheck();
              delay(2000);
            }
          }
          else if(pulse_delay == 6) {
            for(int i=0; i<8; i++) {
              spraycheck();
              delay(2500);
            }
          }
        }
        else if(no_of_pulse == 9) {       //9 pulses selected
          if(pulse_delay == 0) {
            for(int i=0; i<9; i++) {
              spraycheck();
            }
          }
          else if(pulse_delay == 1) {
            for(int i=0; i<9; i++) {
              spraycheck();
              delay(100);
            }
          }
          else if(pulse_delay == 2) {
            for(int i=0; i<9; i++) {
              spraycheck();
              delay(500);
            }
          }
          else if(pulse_delay == 3) {
            for(int i=0; i<9; i++) {
              spraycheck();
              delay(1000);
            }
          }
          else if(pulse_delay == 4) {
            for(int i=0; i<9; i++) {
              spraycheck();
              delay(1500);
            }
          }
          else if(pulse_delay == 5) {
            for(int i=0; i<9; i++) {
              spraycheck();
              delay(2000);
            }
          }
          else if(pulse_delay == 6) {
            for(int i=0; i<9; i++) {
              spraycheck();
              delay(2500);
            }
          }
        }
        else if(no_of_pulse == 10) {        //10 pulses selected
          if(pulse_delay == 0) {
            for(int i=0; i<10; i++) {
              spraycheck();
            }
          }
          else if(pulse_delay == 1) {
            for(int i=0; i<10; i++) {
              spraycheck();
              delay(100);
            }
          }
          else if(pulse_delay == 2) {
            for(int i=0; i<10; i++) {
              spraycheck();
              delay(500);
            }
          }
          else if(pulse_delay == 3) {
            for(int i=0; i<10; i++) {
              spraycheck();
              delay(1000);
            }
          }
          else if(pulse_delay == 4) {
            for(int i=0; i<10; i++) {
              spraycheck();
              delay(1500);
            }
          }
          else if(pulse_delay == 5) {
            for(int i=0; i<10; i++) {
              spraycheck();
              delay(2000);
            }
          }
          else if(pulse_delay == 6) {
            for(int i=0; i<10; i++) {
              spraycheck();
              delay(2500);
            }
          }
        }
      }
      else if(page_counter == 4) {        //page counter 4 / mode 4
         if(no_of_pulse == 1) {           //1 pulse selected
        spraycheck();
        }
        else if(no_of_pulse == 2) {       //2 pulses selected
          if(pulse_delay == 0) {
            for(int i=0; i<2; i++) {
              spraycheck();
            }
          }
          else if(pulse_delay == 1) {
            for(int i=0; i<2; i++) {
              spraycheck();
              delay(100);
            }
          }
          else if(pulse_delay == 2) {
            for(int i=0; i<2; i++) {
              spraycheck();
              delay(500);
            }
          }
          else if(pulse_delay == 3) {
            for(int i=0; i<2; i++) {
              spraycheck();
              delay(1000);
            }
          }
          else if(pulse_delay == 4) {
            for(int i=0; i<2; i++) {
              spraycheck();
              delay(1500);
            }
          }
          else if(pulse_delay == 5) {
            for(int i=0; i<2; i++) {
              spraycheck();
              delay(2000);
            }
          }
          else if(pulse_delay == 6) {
            for(int i=0; i<2; i++) {
              spraycheck();
              delay(2500);
            }
          }
        }
        else if(no_of_pulse == 3) {         //3 pulses selected
          spraydelaycheck();
        }
        else if(no_of_pulse == 4) {         //4 pulses selected
          if(pulse_delay_btw34 == 0) {      //delay duration btw pulse 3 & 4 = 0
            spraycheck();
            spraydelaycheck();
          }
          else if(pulse_delay_btw34 == 1) {     //delay duration btw pulse 3 & 4 = 0.1s
            spraydelaycheck();
            delay(100);
            spraycheck();
          }
          else if(pulse_delay_btw34 == 2) {     //delay duration btw pulse 3 & 4 = 0.5s
            spraydelaycheck();
            delay(500);
            spraycheck();
          }
          else if(pulse_delay_btw34 == 3) {     //delay duration btw pulse 3 & 4 = 1.0s
            spraydelaycheck();
            delay(1000);
            spraycheck();
          }
          else if(pulse_delay_btw34 == 4) {     //delay duration btw pulse 3 & 4 = 1.5s
            spraydelaycheck();
            delay(1500);
            spraycheck();
          }
          else if(pulse_delay_btw34 == 5) {     //delay duration btw pulse 3 & 4 = 2.0s
            spraydelaycheck();
            delay(2000);
            spraycheck();
          }
          else if(pulse_delay_btw34 == 6) {     //delay duration btw pulse 3 & 4 = 2.5s
            spraydelaycheck();
            delay(2500);
            spraycheck();
          }
        }
      }
}

//spraycheck function
void spraycheck() {
  if(pulse_duration == 1) {                     //pulse duration 1 selected (0.1s)
              servo.write(0);                 //movement of servo motor
              delay(200);
              servo.write(30);
          }else if(pulse_duration == 2) {       //pulse duration 2 selected (0.5s)
            servo.write(0);
            delay(300);
            servo.write(30);
          }else if(pulse_duration == 3) {       //pulse duration 3 selected (1.0s)
            servo.write(0);
            delay(400);
            servo.write(30);
          }else if(pulse_duration == 4) {       //pulse duration 4 selected (1.5s)
            servo.write(0);
            delay(500);
            servo.write(30);
          }else if(pulse_duration == 5) {       //pulse duration 5 selected (2.0s)
            servo.write(0);
            delay(600);
            servo.write(30);
          }else if(pulse_duration == 6) {       //pulse duration 6 selected (2.5s)
            servo.write(0);
            delay(700);
            servo.write(30);
          }
}

//spraydelaycheck function
void spraydelaycheck() {
  if(pulse_delay_btw12 == 0) {                  //pulse delay btw pulse 1 & 2 = 0 selected
            if(pulse_delay_btw23 == 0) {        //pulse delay btw pulse 2 & 3 = 0 selected
              for(int i=0; i<3; i++) {
                spraycheck();
              }
            }
            else if(pulse_delay_btw23 == 1) {   //pulse delay btw pulse 2 & 3 = 1 selected
              spraycheck();
              spraycheck();
              delay(100);                        //pulse delay 0.1s btw pulse 2&3
              spraycheck();
            }
            else if(pulse_delay_btw23 == 2) {    //pulse delay btw pulse 2 & 3 = 2 selected
              spraycheck();
              spraycheck();
              delay(500);                        //pulse delay 0.5s btw pulse 2&3
              spraycheck();
            }
            else if(pulse_delay_btw23 == 3) {    //pulse delay btw pulse 2 & 3 = 3 selected
              spraycheck();
              spraycheck();
              delay(1000);                       //pulse delay 1.0s btw pulse 2&3
              spraycheck();
            }
            else if(pulse_delay_btw23 == 4) {    //pulse delay btw pulse 2 & 3 = 4 selected
              spraycheck();
              spraycheck();
              delay(1500);                       //pulse delay 1.5s btw pulse 2&3
              spraycheck();
            }
            else if(pulse_delay_btw23 == 5) {    //pulse delay btw pulse 2 & 3 = 5 selected
              spraycheck();
              spraycheck();
              delay(2000);                       //pulse delay 2.0s btw pulse 2&3
              spraycheck();
            }
            else if(pulse_delay_btw23 == 6) {     //pulse delay btw pulse 2 & 3 = 6 selected
              spraycheck();
              spraycheck();
              delay(2500);                        //pulse delay 2.5s btw pulse 2&3
              spraycheck();
            }
          }
          else if(pulse_delay_btw12 == 1) {       
            if(pulse_delay_btw23 == 0) {
              spraycheck();
              delay(100);
              spraycheck();
              spraycheck();
            }
            else if(pulse_delay_btw23 == 1) {
              spraycheck();
              delay(100);
              spraycheck();
              delay(100);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 2) {
              spraycheck();
              delay(100);
              spraycheck();
              delay(500);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 3) {
              spraycheck();
              delay(100);
              spraycheck();
              delay(1000);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 4) {
              spraycheck();
              delay(100);
              spraycheck();
              delay(1500);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 5) {
              spraycheck();
              delay(100);
              spraycheck();
              delay(2000);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 6) {
              spraycheck();
              delay(100);
              spraycheck();
              delay(2500);
              spraycheck();
            }
          }
          else if(pulse_delay_btw12 == 2) {
            if(pulse_delay_btw23 == 0) {
              spraycheck();
              delay(500);
              spraycheck();
              spraycheck();
            }
            else if(pulse_delay_btw23 == 1) {
              spraycheck();
              delay(500);
              spraycheck();
              delay(100);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 2) {
              spraycheck();
              delay(500);
              spraycheck();
              delay(500);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 3) {
              spraycheck();
              delay(500);
              spraycheck();
              delay(1000);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 4) {
              spraycheck();
              delay(500);
              spraycheck();
              delay(1500);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 5) {
              spraycheck();
              delay(500);
              spraycheck();
              delay(2000);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 6) {
              spraycheck();
              delay(500);
              spraycheck();
              delay(2500);
              spraycheck();
            }
          }
          else if(pulse_delay_btw12 == 3) {
            if(pulse_delay_btw23 == 0) {
              spraycheck();
              delay(1000);
              spraycheck();
              spraycheck();
            }
            else if(pulse_delay_btw23 == 1) {
              spraycheck();
              delay(1000);
              spraycheck();
              delay(100);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 2) {
              spraycheck();
              delay(1000);
              spraycheck();
              delay(500);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 3) {
              spraycheck();
              delay(1000);
              spraycheck();
              delay(1000);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 4) {
              spraycheck();
              delay(1000);
              spraycheck();
              delay(1500);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 5) {
              spraycheck();
              delay(1000);
              spraycheck();
              delay(2000);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 6) {
              spraycheck();
              delay(1000);
              spraycheck();
              delay(2500);
              spraycheck();
            }
          }
          else if(pulse_delay_btw12 == 4) {
            if(pulse_delay_btw23 == 0) {
              spraycheck();
              delay(1500);
              spraycheck();
              spraycheck();
            }
            else if(pulse_delay_btw23 == 1) {
              spraycheck();
              delay(1500);
              spraycheck();
              delay(100);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 2) {
              spraycheck();
              delay(1500);
              spraycheck();
              delay(500);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 3) {
              spraycheck();
              delay(1500);
              spraycheck();
              delay(1000);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 4) {
              spraycheck();
              delay(1500);
              spraycheck();
              delay(1500);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 5) {
              spraycheck();
              delay(1500);
              spraycheck();
              delay(2000);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 6) {
              spraycheck();
              delay(1500);
              spraycheck();
              delay(2500);
              spraycheck();
            }
          }
          else if(pulse_delay_btw12 == 5) {
            if(pulse_delay_btw23 == 0) {
              spraycheck();
              delay(2000);
              spraycheck();
              spraycheck();
            }
            else if(pulse_delay_btw23 == 1) {
              spraycheck();
              delay(2000);
              spraycheck();
              delay(100);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 2) {
              spraycheck();
              delay(2000);
              spraycheck();
              delay(500);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 3) {
              spraycheck();
              delay(2000);
              spraycheck();
              delay(1000);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 4) {
              spraycheck();
              delay(2000);
              spraycheck();
              delay(1500);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 5) {
              spraycheck();
              delay(2000);
              spraycheck();
              delay(2000);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 6) {
              spraycheck();
              delay(2000);
              spraycheck();
              delay(2500);
              spraycheck();
            }
          }
          else if(pulse_delay_btw12 == 6) {
            if(pulse_delay_btw23 == 0) {
              spraycheck();
              delay(2500);
              spraycheck();
              spraycheck();
            }
            else if(pulse_delay_btw23 == 1) {
              spraycheck();
              delay(2500);
              spraycheck();
              delay(100);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 2) {
              spraycheck();
              delay(2500);
              spraycheck();
              delay(500);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 3) {
              spraycheck();
              delay(2500);
              spraycheck();
              delay(1000);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 4) {
              spraycheck();
              delay(2500);
              spraycheck();
              delay(1500);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 5) {
              spraycheck();
              delay(2500);
              spraycheck();
              delay(2000);
              spraycheck();
            }
            else if(pulse_delay_btw23 == 6) {
              spraycheck();
              delay(2500);
              spraycheck();
              delay(2500);
              spraycheck();
            }
          }
}

//submenuchceker function --> different modes for LCD display
void SubmenuChecker() {
  if(rotated == true) {     //when the rotary encoder turning
    //Ready = 1;
    if(submenu == 0){
      switch (page_counter) {
   
       case 1:{     //Design of home page 1
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("MODE 1");
          lcd.setCursor(11,0);
          lcd.print("Enter");
          lcd.setCursor(11,1);
          lcd.write(1);
          lcd.setCursor(12,1);
          lcd.print("Next");
          delay(1000);
      
       }
       break;

      case 2: { //Design of page 2 
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("MODE 2");
         lcd.setCursor(11,0);
         lcd.print("Enter");
         lcd.setCursor(0,1);
         lcd.print("Previous");
         lcd.setCursor(8,1);
         lcd.write(0);
         lcd.setCursor(11,1);
         lcd.write(1);
         lcd.setCursor(12,1);
         lcd.print("Next");
         delay(1000);
         //lcd.clear();
         //delay(500);
       
      }
      break;

      case 3: {   //Design of page 3 
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("MODE 3");
         lcd.setCursor(11,0);
         lcd.print("Enter");
         lcd.setCursor(0,1);
         lcd.print("Previous");
         lcd.setCursor(8,1);
         lcd.write(0);
         lcd.setCursor(11,1);
         lcd.write(1);
         lcd.setCursor(12,1);
         lcd.print("Next");
         delay(1000);
      }
      break; 

      case 4: { //Design of page 4 
         lcd.clear();
         lcd.setCursor(0,0);
         lcd.print("MODE 4");
         lcd.setCursor(11,0);
         lcd.print("Enter");
         lcd.setCursor(0,1);
         lcd.print("Previous");
         lcd.setCursor(8,1);
         lcd.write(0);
         //lcd.setCursor(11,1);
         //lcd.write(1);
         //lcd.setCursor(12,1);
         //lcd.print("Next");
         delay(1000);
         //lcd.clear();
         //delay(500);
      }
      break;
    }//switch end
  }//submenu0 end
  submenuinfo();
  rotated = false;
  }
}

//submenuinfo function ---> different submenu LCD display for each mmode
void submenuinfo() {
  if(rotated == true) {
  if(submenu == 1 && page_counter == 1) {
    if(0 <= counter && counter < 3) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Manual Spray");
      lcd.setCursor(0,1);
      lcd.print("Volume >Half  Full");
    }
    if(3 < counter && counter < 6) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Manual Spray");
      lcd.setCursor(0,1);
      lcd.print("Volume  Half >Full");
    }
  }
  if(submenu == 1 && page_counter == 2) {
    if(0 <= counter && counter < 6) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Pulse Duration");
      lcd.setCursor(0,1);
      lcd.print(">press to set Dur");
    }
  }
  if(submenu == 1 && page_counter == 3) {
    if(0 <= counter && counter < 6) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Vary Pulse");
      lcd.setCursor(0,1);
      lcd.print(">press: set pulse");
    }
  }
  if(submenu == 1 && page_counter == 4) {
    if(0 <= counter && counter < 6) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Full Auto Custom");
      lcd.setCursor(0,1);
      lcd.print(">press: set pulse");
    }
  }
  if(submenu == 2 && page_counter == 1 && (0 <= counter && counter < 3)) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Half Volume");
      lcd.setCursor(0,1);
      lcd.print("press to spray");
      half_volume = 1;
  }
  if(submenu == 2 && page_counter == 1 && (3 <= counter && counter < 6)) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Full Volume");
      lcd.setCursor(0,1);
      lcd.print("press to spray");
      full_volume = 1;
  }
  if(submenu == 2 && page_counter ==2) {
    no_of_pulse = 1;
    if(0 <= counter2 && counter2 < 2) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("one pulse");
      lcd.setCursor(0,1);
      lcd.print(">Duration: 0.0sec");
    }
    if(2 < counter2 && counter2 < 4) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("one pulse");
      lcd.setCursor(0,1);
      lcd.print(">Duration: 0.2sec");
      pulse_duration_check = 1;
    }
    if(4 < counter2 && counter2 < 6) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("one pulse");
      lcd.setCursor(0,1);
      lcd.print(">Duration: 0.3sec");
      pulse_duration_check = 2;
    }
    if(6 < counter2 && counter2 < 8) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("one pulse");
      lcd.setCursor(0,1);
      lcd.print(">Duration: 0.4sec");
      pulse_duration_check = 3;
    }
    if(8 < counter2 && counter2 < 10) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("one pulse");
      lcd.setCursor(0,1);
      lcd.print(">Duration: 0.5sec");
      pulse_duration_check = 4;
    }
    if(10 < counter2 && counter2 < 12) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("one pulse");
      lcd.setCursor(0,1);
      lcd.print(">Duration: 0.6sec");
      pulse_duration_check = 5;
    }
    if(12 < counter2 && counter2 < 14) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("one pulse");
      lcd.setCursor(0,1);
      lcd.print(">Duration: 0.7sec");
      pulse_duration_check = 6;
    }
  }
  if(submenu == 2 && page_counter == 3) {
    if(0 <= counter && counter < 2) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 0");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 0;
    }
    if(2 < counter && counter < 4) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 1");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 1;
      durationcheck();
      delaycheck();
    }
    if(4 < counter && counter < 6) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 2");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 2;
      durationcheck();
      delaycheck();
    }
    if(6 < counter && counter < 8) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 3");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 3;
      durationcheck();
      delaycheck();
    }
    if(8 < counter && counter < 10) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 4");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 4;
      durationcheck();
      delaycheck();
    }
    if(10 < counter && counter < 12) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 5");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 5;
      durationcheck();
      delaycheck();
    }
    if(12 < counter && counter < 14) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 6");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 6;
      durationcheck();
      delaycheck();
    }
    if(14 < counter && counter < 16) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 7");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 7;
      durationcheck();
      delaycheck();
    }
    if(16 < counter && counter < 18) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 8");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 8;
      durationcheck();
      delaycheck();
    }
    if(18 < counter && counter < 20) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 9");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 9;
      durationcheck();
      delaycheck();
    }
    if(20 < counter && counter < 22) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 10");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 10;
      durationcheck();
      delaycheck();
    }
  }
  if(submenu == 2 && page_counter == 4) {
    if(0 <= counter3 && counter3 < 2) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 0");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 0;
    }
    if(2 < counter3 && counter3 < 4) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 1");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 1;
      durationcheck1();
      delaycheck();
    }
    if(4 < counter3 && counter3 < 6) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 2");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 2;
      durationcheck1();
      delaycheck();
    }
    if(6 < counter3 && counter3 < 8) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 3");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 3;
      durationcheck1();
      delaycheck();
    }
    if(8 < counter3 && counter3 < 10) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("># of pulse: 4");
      lcd.setCursor(0,1);
      lcd.print(" Duration: 0.0sec");
      no_of_pulse = 4;
      durationcheck1();
      delaycheck();
    }
  }
  rotated = false;
 } 
}

//durationcheck function --> different LCD display for different choices of pulse and duration of pulse for mode 3 
void durationcheck() {
  if(submenu == 3 && page_counter == 3) {
        if(0 <= counter1 && counter1 < 2) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.0sec");
        }
        if(2 < counter1 && counter1 < 4) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.2sec");
          pulse_duration = 1;
        }
        if(4 < counter1 && counter1 < 6) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.3sec");
          pulse_duration = 2;
        }
        if(6 < counter1 && counter1 < 8) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.4sec");
          pulse_duration = 3;
        }
        if(8 < counter1 && counter1 < 10) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.5sec");
          pulse_duration = 4;
        }
        if(10 < counter1 && counter1 < 12) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.6sec");
          pulse_duration = 5;
        }
        if(12 < counter1 && counter1 < 14) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.7sec");
          pulse_duration = 6;
        } 
    }
}

//durationcheck1 function --> different LCD display for different choices of pulse and duration of pulse for mode 4 
void durationcheck1() {
  if(submenu == 3 && page_counter == 4) {
        if(0 <= counter1 && counter1 < 2) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.0sec");
        }
        if(2 < counter1 && counter1 < 4) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.2sec");
          pulse_duration = 1;
        }
        if(4 < counter1 && counter1 < 6) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.3sec");
          pulse_duration = 2;
        }
        if(6 < counter1 && counter1 < 8) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.4sec");
          pulse_duration = 3;
        }
        if(8 < counter1 && counter1 < 10) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.5sec");
          pulse_duration = 4;
        }
        if(10 < counter1 && counter1 < 12) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.6sec");
          pulse_duration = 5;
        }
        if(12 < counter1 && counter1 < 14) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(String(" # of pulse: ") + String(no_of_pulse));
          lcd.setCursor(0,1);
          lcd.print(">Duration: 0.7sec");
          pulse_duration = 6;
        }
    }
}

//durationcheck2 function --> different LCD display for different choices of pulse and duration of pulse for mode 2 
void durationcheck2() {
  if(submenu == 3 && page_counter == 2) {
    if(pulse_duration_check == 1) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Duration: 0.2sec");
      lcd.setCursor(0,1);
      lcd.print(">press to spray");
      pulse_duration = 1;
    }
    else if(pulse_duration_check == 2) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Duration: 0.3sec");
      lcd.setCursor(0,1);
      lcd.print(">press to spray");
      pulse_duration = 2;
    }
    else if(pulse_duration_check == 3) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Duration: 0.4sec");
      lcd.setCursor(0,1);
      lcd.print(">press to spray");
      pulse_duration = 3;
    }
    else if(pulse_duration_check == 4) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Duration: 0.5sec");
      lcd.setCursor(0,1);
      lcd.print(">press to spray");
      pulse_duration = 4;
    }
    else if(pulse_duration_check == 5) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Duration: 0.6sec");
      lcd.setCursor(0,1);
      lcd.print(">press to spray");
      pulse_duration = 5;
    }
    else if(pulse_duration_check == 6) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Duration: 0.7sec");
      lcd.setCursor(0,1);
      lcd.print(">press to spray");
      pulse_duration = 6;
    }
  }
}

//delaycheck function --> different choices of delay btw pulses for mode 3
void delaycheck() {
  if(submenu == 4 && page_counter == 3) {
    if(0 <= counter4 && counter4 < 2) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Delay btw pulse");
      lcd.setCursor(0,1);
      lcd.print(">Delay: 0.0sec");
      pulse_delay = 0;
    }
    if(2 < counter4 && counter4 < 4) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Delay btw pulse");
      lcd.setCursor(0,1);
      lcd.print(">Delay: 0.1sec");
      pulse_delay = 1;
    }
    if(4 < counter4 && counter4 < 6) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Delay btw pulse");
      lcd.setCursor(0,1);
      lcd.print(">Delay: 0.5sec");
      pulse_delay = 2;
    }
    if(6 < counter4 && counter4 < 8) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Delay btw pulse");
      lcd.setCursor(0,1);
      lcd.print(">Delay: 1.0sec");
      pulse_delay = 3;
    }
    if(8 < counter4 && counter4 < 10) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Delay btw pulse");
      lcd.setCursor(0,1);
      lcd.print(">Delay: 1.5sec");
      pulse_delay = 4;
    }
    if(10 < counter4 && counter4 < 12) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Delay btw pulse");
      lcd.setCursor(0,1);
      lcd.print(">Delay: 2.0sec");
      pulse_delay = 5;
    }
    if(12 < counter4 && counter4 < 14) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Delay btw pulse");
      lcd.setCursor(0,1);
      lcd.print(">Delay: 2.5sec");
      pulse_delay = 6;
    }
  }
}

//eachdelaycheck function --> different choices of each delay btw pulse after the no of pulses have selected for mode 4
void eachdelaycheck1() {
  if(submenu == 4 && page_counter == 4) {
    if(no_of_pulse == 2) {
      if(0 <= counter4 && counter4 < 2) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Delay btw pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.0sec");
        pulse_delay = 0;
      }
      if(2 < counter4 && counter4 < 4) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Delay btw pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.1sec");
        pulse_delay = 1;
      }
      if(4 < counter4 && counter4 < 6) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Delay btw pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.5sec");
        pulse_delay = 2;
      }
      if(6 < counter4 && counter4 < 8) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Delay btw pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 1.0sec");
        pulse_delay = 3;
      }
      if(8 < counter4 && counter4 < 10) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Delay btw pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 1.5sec");
        pulse_delay = 4;
      }
      if(10 < counter4 && counter4 < 12) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Delay btw pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 2.0sec");
        pulse_delay = 5;
      }
      if(12 < counter4 && counter4 < 14) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Delay btw pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 2.5sec");
        pulse_delay = 6;
      }
    }
    else if(no_of_pulse == 3) {
      if(0 <= counter4 && counter4 < 2) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.0sec");
        pulse_delay_btw12 = 0;
      }
      if(2 < counter4 && counter4 < 4) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.1sec");
        pulse_delay_btw12 = 1;
      }
      if(4 < counter4 && counter4 < 6) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.5sec");
        pulse_delay_btw12 = 2;
      }
      if(6 < counter4 && counter4 < 8) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 1.0sec");
        pulse_delay_btw12 = 3;
      }
      if(8 < counter4 && counter4 < 10) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 1.5sec");
        pulse_delay_btw12 = 4;
      }
      if(10 < counter4 && counter4 < 12) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 2.0sec");
        pulse_delay_btw12 = 5;
      }
      if(12 < counter4 && counter4 < 14) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 2.5sec");
        pulse_delay_btw12 = 6;
      }
    }
    else {
      if(0 <= counter4 && counter4 < 2) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.0sec");
        pulse_delay_btw12 = 0;
      }
      if(2 < counter4 && counter4 < 4) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.1sec");
        pulse_delay_btw12 = 1;
      }
      if(4 < counter4 && counter4 < 6) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.5sec");
        pulse_delay_btw12 = 2;
      }
      if(6 < counter4 && counter4 < 8) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 1.0sec");
        pulse_delay_btw12 = 3;
      }
      if(8 < counter4 && counter4 < 10) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 1.5sec");
        pulse_delay_btw12 = 4;
      }
      if(10 < counter4 && counter4 < 12) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 2.0sec");
        pulse_delay_btw12 = 5;
      }
      if(12 < counter4 && counter4 < 14) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 1&2pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 2.5sec");
        pulse_delay_btw12 = 6;
      }
    }
  }
  if(submenu == 5 && page_counter == 4) {
    if(no_of_pulse == 3) {
      if(0 <= counter5 && counter5 < 2) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.0sec");
        pulse_delay_btw23 = 0;
      }
      if(2 < counter5 && counter5 < 4) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.1sec");
        pulse_delay_btw23 = 1;
      }
      if(4 < counter5 && counter5 < 6) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.5sec");
        pulse_delay_btw23 = 2;
      }
      if(6 < counter5 && counter5 < 8) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 1.0sec");
        pulse_delay_btw23 = 3;
      }
      if(8 < counter5 && counter5 < 10) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 1.5sec");
        pulse_delay_btw23 = 4;
      }
      if(10 < counter5 && counter5 < 12) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 2.0sec");
        pulse_delay_btw23 = 5;
      }
      if(12 < counter5 && counter5 < 14) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 2.5sec");
        pulse_delay_btw23 = 6;
      }
    }
    else if(no_of_pulse == 4) {
      if(0 <= counter5 && counter5 < 2) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.0sec");
        pulse_delay_btw23 = 0;
      }
      if(2 < counter5 && counter5 < 4) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.1sec");
        pulse_delay_btw23 = 1;
      }
      if(4 < counter5 && counter5 < 6) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.5sec");
        pulse_delay_btw23 = 2;
      }
      if(6 < counter5 && counter5 < 8) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 1.0sec");
        pulse_delay_btw23 = 3;
      }
      if(8 < counter5 && counter5 < 10) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 1.5sec");
        pulse_delay_btw23 = 4;
      }
      if(10 < counter5 && counter5 < 12) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 2.0sec");
        pulse_delay_btw23 = 5;
      }
      if(12 < counter5 && counter5 < 14) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 2&3pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 2.5sec");
        pulse_delay_btw23 = 6;
      }
    }
  }
  if(submenu == 6 && page_counter == 4) {
      if(0 <= counter6 && counter6 < 2) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 3&4pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.0sec");
        pulse_delay_btw34 = 0;
      }
      if(2 < counter6 && counter6 < 4) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 3&4pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.1sec");
        pulse_delay_btw34 = 1;
      }
      if(4 < counter6 && counter6 < 6) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 3&4pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 0.5sec");
        pulse_delay_btw34 = 2;
      }
      if(6 < counter6 && counter6 < 8) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 3&4pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 1.0sec");
        pulse_delay_btw34 = 3;
      }
      if(8 < counter6 && counter6 < 10) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 3&4pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 1.5sec");
        pulse_delay_btw34 = 4;
      }
      if(10 < counter6 && counter6 < 12) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 3&4pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 2.0sec");
        pulse_delay_btw34 = 5;
      }
      if(12 < counter6 && counter6 < 14) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DLA btw 3&4pulse");
        lcd.setCursor(0,1);
        lcd.print(">Delay: 2.5sec");
        pulse_delay_btw34 = 6;
      }
  }
}
