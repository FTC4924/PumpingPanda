#include <Servo.h>

const int LEFT_LED = 13;
const int RIGHT_LED = 12;
const int PUMP = 8;
const int SERVO_PIN = 3;

const int FLASH_DELAY = 200;
const int FLASH_STATE_DURATION = 1500;
const int SOLID_LED_DURATION = 3000;
const int PUMP_ON_TIME = 6000;
const int PUMP_OFF_TIME = 4000;

Servo arm;
int arm_pos = 0;
int state_change_counter = 0;

int next_led_state_time = 0;
int next_pump_state_time = 0;
bool left_on = true;
int currentTime;
int left_side_on_time;
int right_side_on_time;
int individual_flash_time = 0;
bool flash_left = false;
bool toggle_lights = false;

enum ledstates  {
  LED_INIT,
  FLASHING, 
  SINGLE_SIDE_ON
};

enum pumpstates {
  PUMP_INIT,
  PUMPING, 
  RESTING
};

enum servostates {
  SERVO_INIT,
  SERVO_LEFT,
  SERVO_RIGHT
};

ledstates currentLEDState = LED_INIT;
pumpstates currentPumpState = PUMPING;
servostates currentServoState = SERVO_INIT;

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);
  pinMode(PUMP, OUTPUT);
  arm.attach(SERVO_PIN);

  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {

  Serial.println(String(currentPumpState) + " - " + String(currentLEDState) + " - " + state_change_counter);
  
  currentTime = millis();

  // pump state machine
  switch (currentPumpState) {
    case PUMP_INIT:
      next_pump_state_time = millis() + PUMP_ON_TIME;
      currentPumpState = PUMPING;
      break;
    case PUMPING:
      if (currentTime > next_pump_state_time) {
        currentPumpState = RESTING;
        next_pump_state_time = currentTime + PUMP_OFF_TIME;
        toggle_lights = true;
        Pump(false);
        arm.write(135);
        state_change_counter++;
      }
      break;
    case RESTING:
      if (currentTime > next_pump_state_time) {
        currentPumpState = PUMPING;
        next_pump_state_time = currentTime + PUMP_ON_TIME;
        toggle_lights = true;
        Pump(true);
        arm.write(45);        
        state_change_counter++;
      }
      break;
  }
  
  // led state machine
  switch (currentLEDState) {
    case LED_INIT:
      currentLEDState = FLASHING;
      next_led_state_time = millis() + FLASH_STATE_DURATION;
      individual_flash_time = 0;
      break;
    case FLASHING:
      if (currentTime > individual_flash_time) {
        flash_left = !flash_left;
        Left_LED(flash_left);
        Right_LED(!flash_left);
        individual_flash_time = millis() + FLASH_DELAY;
      }
      break;
      
    case SINGLE_SIDE_ON:
      Left_LED(left_on);
      Right_LED(!left_on);
      if (toggle_lights) {
        currentLEDState = FLASHING;
        left_on = !left_on;
        individual_flash_time = millis() + FLASH_DELAY;
        toggle_lights = false;
      }
      break;
  } 

}

void Left_LED(bool on) {
  if (on) {
    digitalWrite(LEFT_LED, HIGH);
  } else {
    digitalWrite(LEFT_LED, LOW);
  }
}

void Right_LED(bool on) {
  if (on) {
    digitalWrite(RIGHT_LED, HIGH);
  } else {
    digitalWrite(RIGHT_LED, LOW);
  }
}  

void Pump(bool on) {
  if (on) {
    digitalWrite(PUMP, HIGH);
  } else {
    digitalWrite(PUMP, LOW);
  }
}



