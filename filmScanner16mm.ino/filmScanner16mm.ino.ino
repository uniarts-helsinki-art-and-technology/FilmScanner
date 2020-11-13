/*   .___________________________________________________
    |   _     _     _     _     _     _     _     _     _|
    |  |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_ 
    | .                                                  |
    |          This is KuvA-Scanner-Code v.1.0 .         |
    |                                                    |
    |      description: code for 16mm telecine machine . | 
    |                                                    |
    |                  date: 13.11.2020                   |
    |                                                    |
    ^----------------------------------------------------^

    
 *                      NOTES
 *       Check out the pinouts and cabeling!
 * 
 * 
 * 
 * 
 *
 * 
 */


// A) Antava Stepperi 1
const int PULSE_1 = 2; // STEP 1
const int DIRECTION_1 = 3; // DIRECTION 1
const int ENABLE_1 = 4; // ENABLE 1

// B) Ottava Stepperi 2
const int PULSE_2     = 5; // STEP 2
const int DIRECTION_2 = 6; // DIRECTION 2
const int ENABLE_2    = 7; // ENABLE 2

// C) Gate/spro Stepper 3
const int PULSE_3     = 8; // ENABLE 3
const int DIRECTION_3 = 9; // ENABLE 3
const int ENABLE_3    = 10; // STEP 3

// Buttons/Switches
const int PIN_PLAY_BUTTON = 50;
const int PIN_FRAME_DETECTION_SWITCH = 52; 
// TODO: Add some new buttons here!
// BUTTON_KELAUS_ETEEN
// BUTTON_KELAUS_TAAKSE
// BUTTON_PLAY_ETEEN
// BUTTON_PLAY_TAAKSE
// BUTTON_STOP
// BUTTON_REC

// modes
int mode = 0; // 0=stopped, 1=rewind, 2=play
bool recording = false;

// running directions (true = forwards, false = backwards)
bool running_forward = true;

// other global variables
int saved_frames_count=0;


void setup() {
  
  Serial.begin(9600);
  
  setPinModes();
  
  enableAllMotors();

  if(mode == 1)
  {
    disableGateMotor();
  }
  
  if(running_forward==true)
  {
    setDirectionForwards();
  }
  else
  {
    setDirectionBackwards();
  }
  
}


void loop() 
{
  switch (mode)
  {
    case 1:
      rewinding(100);
      break;
    case 2:
      move_one_frame(100);
      if(recording == true && running_forward==true)
      {
        Serial.println("ota kuva!");
        // TODO: send signal via lanc
        // TODO: wait until frame is saved. How long time? Knob for adjusting time?
        Serial.print("Number of saved frames :"+String(saved_frames_count));
        // count the images  
        saved_frames_count++;
      }
      // pause always after single frame is moved
      delay(1000);
      break;
    default:
      // stopped, no need to move anywhere
      break;
  }

  // read inputs from control panel
  readPlayControl();
  // TODO: readRewindControl(); > set mode
  // TODO: readDirectionControl(); > set direction
  
  // print info
  //TODO: print info to lcd

}


void move_one_frame(int pulse_delay)
{
  for (int i=0; i<5000; i++)    //Forward 5000 steps
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
        i= 5000;
    }
  }
  
  }
}

void play_pulseHIGH()
{
  digitalWrite(PULSE_1,HIGH);
  digitalWrite(PULSE_2,HIGH);
  digitalWrite(PULSE_3,HIGH);
}
 
void play_pulseLOW()
{
  digitalWrite(PULSE_1,LOW);
  digitalWrite(PULSE_2,LOW);
  digitalWrite(PULSE_3,LOW);
}

void rewind_pulseHIGH()
{
  digitalWrite(PULSE_1,HIGH);
  digitalWrite(PULSE_2,HIGH);
}
 
void rewind_pulseLOW()
{
  digitalWrite(PULSE_1,LOW);
  digitalWrite(PULSE_2,LOW);
}

void move_one_step(int pulse_delay)
{
  play_pulseHIGH();
  delayMicroseconds(50);
  play_pulseLOW();
  delayMicroseconds(pulse_delay);
}

void rewinding(int pulse_delay)
{
  rewind_pulseHIGH();
  delayMicroseconds(50);
  rewind_pulseLOW();
  delayMicroseconds(pulse_delay);
  
}

void setDirectionForwards()
{
  digitalWrite(DIRECTION_1,HIGH);
  digitalWrite(DIRECTION_2,HIGH);
  digitalWrite(DIRECTION_3,HIGH);
}

void setDirectionBackwards()
{
  digitalWrite(DIRECTION_1,LOW);
  digitalWrite(DIRECTION_2,LOW);
  digitalWrite(DIRECTION_3,LOW);
}

void enableAllMotors()
{
  digitalWrite(ENABLE_1,LOW);
  digitalWrite(ENABLE_2,LOW);
  digitalWrite(ENABLE_3,LOW);
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
  recording_mode=false;
  running_forward=!running_forward;
}



void setPinModes(){
  pinMode (PULSE_1, OUTPUT);
  pinMode (DIRECTION_1, OUTPUT);
  pinMode (ENABLE_1, OUTPUT);
  
  pinMode (PULSE_2, OUTPUT);
  pinMode (DIRECTION_2, OUTPUT);
  pinMode (ENABLE_2, OUTPUT);
  
  pinMode (PULSE_3, OUTPUT);
  pinMode (DIRECTION_3, OUTPUT);
  pinMode (ENABLE_3, OUTPUT);

  pinMode (PIN_FRAME_DETECTION_SWITCH, INPUT_PULLUP);
  pinMode (PIN_PLAY_BUTTON, INPUT_PULLUP);
}
  
