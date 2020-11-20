/*   .___________________________________________________
    |   _     _     _     _     _     _     _     _     _|
    |  |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_ 
    | .                                                  |
    |          This is KuvA-Scanner-Code v.1.0 .         |
    |                                                    |
    |      description: code for 16mm telecine machine . | 
    |                                                    |
    |                  date: 20.11.2020                   |
    |                                                    |
    ^----------------------------------------------------^

    
 *                      NOTES
 * 
 * 
 * 
 * 
 *
 * 
 */


// A) Antava Stepperi 1
const byte PULSE_1 = 2; // STEP 1
const byte DIRECTION_1 = 3; // DIRECTION 1
const byte ENABLE_1 = 4; // ENABLE 1

// B) Ottava Stepperi 2
const byte PULSE_2     = 5; // STEP 2
const byte DIRECTION_2 = 6; // DIRECTION 2
const byte ENABLE_2    = 7; // ENABLE 2

// C) Gate/spro Stepper 3
const byte PULSE_3     = 8; // ENABLE 3
const byte DIRECTION_3 = 9; // ENABLE 3
const byte ENABLE_3    = 10; // STEP 3

// ARRAYS FOR PINS
const byte STEPPER_PULSE_PINS[] = {PULSE_1,PULSE_2,PULSE_3};
const byte STEPPER_DIRECTION_PINS[] = {DIRECTION_1,DIRECTION_2,DIRECTION_3};
const byte STEPPER_ENABLE_PINS[] = {ENABLE_1,ENABLE_2,ENABLE_3};

// Buttons/Switches
const byte PIN_PLAY_BUTTON = 50;
const byte PIN_FRAME_DETECTION_SWITCH = 52;
const byte PIN_STOP_BUTTON = 48; // TODO: Check wiring!
// TODO: Add some new buttons here!
// BUTTON_KELAUS_ETEEN
// BUTTON_KELAUS_TAAKSE
// BUTTON_PLAY_ETEEN
// BUTTON_PLAY_TAAKSE
// BUTTON_STOP
// BUTTON_REC

// modes (byte on 8 bittiä 0-255)
byte mode = 0; // 0=stopped, 1=rewind, 2=play, 3=stop

// booleans joko 0 tai 1
bool recording = false;

// running directions (true = forwards, false = backwards)
bool running_forward = true;

// other global variables
unsigned long saved_frames_count=0;


void setup() {
  
  Serial.begin(9600);
  
  setPinModes();
  
  enableAllMotors();

  // setup gate stepper according to the settings
  // THIS IS JUST FOR DEBUGGING PURPOSE
  if(mode == 1)
  {
    disableGateMotor();
  }

  // setup direction according to the settings
  // THIS IS JUST FOR DEBUGGING PURPOSE
  if(running_forward==true)
  {
    setDirectionForwards();
  }
  else
  {
    setDirectionBackwards();
  }
  
} // setup loppuu


void loop() 
{
  switch (mode) // tarkistetaan miten filmiä siirretään
  {
    case 1: // WINDING BOTH RW AND FFW
      winding(100);
      break;
    case 2: // PLAY BOTH BACKWARDS / FORWARDS, RECORDING
      move_one_frame(100);
      // check skipped frames here
      
      if(recording == true && running_forward==true)
      {
        Serial.println("ota kuva!");
        // TODO: send signal via lanc
        // TODO: wait until frame is saved. State switch for adjusting time. Read value only once when starting recording!
        Serial.println("Number of saved frames :"+String(saved_frames_count));
        // count the images  
        saved_frames_count=saved_frames_count + 1; // sama kuin -> saved_frames_count++; tai saved_frames_count+=1;
      }
      // pause always after single frame is moved
      delay(1000);
      break;
    case 3: // STOP
      // stopped, no need to move anywhere
      break;
    default: // STOP
      // stopped, no need to move anywhere
      break;
  }


  // read inputs from control panel
  
  switch(mode)
  {
    case 1: // WINDING
      // check stop button
      readStopButton();
      break;
    case 2: // PLAY RECORD
      // check stop button
      readStopButton();
      break;
    case 3: // STOP
      // CHECK ALL BUTTONS
      readAllButtons();
      break;
    default:
      break;
  }
    
  // print info
  //TODO: print info to lcd
  // mode
  // speed

} // loop funktio loppuu 

void readStopButton(){
        // read the state of the stop button value:
      int stop_pressed = digitalRead(PIN_STOP_BUTTON);
      
      if(stop_pressed == LOW)
      {   
          Serial.println("STOP PRESSED");
          recording=false; 
          mode=3;
      }
}

void readAllButtons(){
        // FFW : setDirectionForwards(); mode=1; 
      // RW : setDirectionBackwards(); mode=1; 
      // REC : setDirectionForwards(); recording=true; mode=2; 
      // PLAY BACKWARDS : setDirectionBackwards(); mode=2; 
      // PLAY FORWARDS : setDirectionForwards(); mode=2; 
      // STOP : recording=false; mode=3;
}


void move_one_frame(int pulse_delay)
{
  //Forward 5000 steps? TODO: CHOOSE ANOTHER HARD CODED VALUE?
  for (int i=0; i<5000; i++)
  {
  
    move_one_step(pulse_delay);
    
    // Do not detect switch state until motor has rotated
    if(i>150)
    {
      // read the state of the frame detection switch value:
      int frame_detected = digitalRead(PIN_FRAME_DETECTION_SWITCH);
      
      // if switch is pressed break out from for loop
      if(frame_detected == LOW)
      {
          // break out from loop when frame is detected
          Serial.println("frame detected!");
          Serial.println("Number of steps:"+String(i));
          //number_of_pulses_between_frames = i;
          if(i>450)
          {
            mode=0;
            Serial.println("frame drop detected!");
          }
          i= 5000;
      }
    }
  
  }
}

void move_one_frame_option2(int pulse_delay)
{
  // ADD 10K RESISTOR TO SWITCH AND CHANGE INPUT_PULLUP TO INPUT
  // AND CHECK THE LOGIC "HIGH" OR "LOW
  do {
    move_one_step(pulse_delay);
  } while (digitalRead(PIN_FRAME_DETECTION_SWITCH) == HIGH);
}


void play_pulseHIGH()
{
  for(int i=0;i<3;i++){
    digitalWrite(STEPPER_PULSE_PINS[i],HIGH);
  }
}
 
void play_pulseLOW()
{
  for(int i=0;i<3;i++){
    digitalWrite(STEPPER_PULSE_PINS[i],LOW);
  }
}

void rewind_pulseHIGH()
{
  for(int i=0;i<2;i++){
    digitalWrite(STEPPER_PULSE_PINS[i],HIGH);
  }
}
 
void rewind_pulseLOW()
{
  for(int i=0;i<2;i++){
    digitalWrite(STEPPER_PULSE_PINS[i],LOW);
  }
}

void move_one_step(int pulse_delay)
{
  play_pulseHIGH();
  delayMicroseconds(50);
  play_pulseLOW();
  delayMicroseconds(pulse_delay);
}

void winding(int pulse_delay)
{
  rewind_pulseHIGH();
  delayMicroseconds(50);
  rewind_pulseLOW();
  delayMicroseconds(pulse_delay);
  
}

void setDirectionForwards()
{
  for(int i=0;i<3;i++){
    digitalWrite(STEPPER_DIRECTION_PINS[i],HIGH);
  }
}

void setDirectionBackwards()
{
  for(int i=0;i<3;i++){
    digitalWrite(STEPPER_DIRECTION_PINS[i],LOW);
  }
}

void enableAllMotors()
{
  for(int i=0;i<3;i++){
    digitalWrite(STEPPER_ENABLE_PINS[i],LOW);
  }
}

void disableGateMotor()
{
  digitalWrite(ENABLE_3,HIGH);
}

void readPlayControl()
{
  int play_state = digitalRead(PIN_PLAY_BUTTON);

 if(play_state == LOW)
    {
        Serial.println("Run!");
        mode = 2;
    }
    else
    {
      // TODO: THIS MIGHT BE BUGGY, USE 10K RESISTOR WITH SWITCH AND CHANGE INPUT_PULLUP TO INPUT
        Serial.println("Stop running!");
        mode = 0;
    }
}


void changeToRewindMode()
{
  mode=1;
  disableGateMotor();
  
  if(running_forward==true)
  {
    setDirectionForwards();
  }
  else
  {
    setDirectionBackwards();
  }
}

void changeToPlayMode()
{
  mode=2;
  enableAllMotors();
  
  if(running_forward==true)
  {
    setDirectionForwards();
  }
  else
  {
    setDirectionBackwards();
  }
}

void changeDirection()
{
  // stop the process just in case
  mode=0;
  recording=false;
  running_forward=!running_forward;
}



void setPinModes(){
  
  // stepper motors
  for(int i=0;i<3;i++){
    pinMode (STEPPER_PULSE_PINS[i], OUTPUT);
    pinMode (STEPPER_DIRECTION_PINS[i], OUTPUT);
    pinMode (STEPPER_ENABLE_PINS[i], OUTPUT);
  }

  // switches
  pinMode (PIN_FRAME_DETECTION_SWITCH, INPUT_PULLUP);

  // buttons
  pinMode (PIN_PLAY_BUTTON, INPUT_PULLUP);
  // ADD NEW BUTTONS HERE
}
  
