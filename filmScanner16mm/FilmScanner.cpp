#include "Arduino.h"
#include "FilmScanner.h"

FilmScanner::FilmScanner(byte _mode) {
  mode = _mode;
}

void FilmScanner::enableDebugMode() {
  Serial.begin(9600);
  Serial.println("Debug mode enabled. Serial communication started (baud rate 9600)");
  debugMode = true;
}

void FilmScanner::setupMotor(StepperMotor &m, byte pulse_pin, byte direction_pin, byte enable_pin)
{
  m.pulse_pin = pulse_pin;
  m.direction_pin = direction_pin;
  m.enable_pin = enable_pin;
  m.enabled = ENABLED;
  pinMode (m.pulse_pin, OUTPUT);
  pinMode (m.direction_pin, OUTPUT);
  pinMode (m.enable_pin, OUTPUT);
  delay(1);
  digitalWrite(m.enable_pin, ENABLED);
  delay(1);
}

void FilmScanner::setCameraRemoteControlPin(byte capture)
{
  capture_output.pin = capture;
  pinMode(capture_output.pin, OUTPUT);
}

void FilmScanner::setSwingArmSensorsToPin(byte upper_arm_pin, byte lower_arm_pin)
{
  upper_swing_arm.pin = upper_arm_pin;
  upper_swing_arm.pressed = false;
  lower_swing_arm.pin = lower_arm_pin;
  lower_swing_arm.pressed = false;
  pinMode (upper_swing_arm.pin, INPUT);
  pinMode (lower_swing_arm.pin, INPUT);
  delay(1);
  
  if (debugMode == true)
  {
    Serial.println("Upper swing arm sensor set to pin " + String(upper_swing_arm.pin));
    Serial.println("Lower swing arm sensor set to pin " + String(lower_swing_arm.pin));
  }
}

void FilmScanner::setControlPanelButtonPins(byte stop_b, byte playb_b, byte play_b, byte rec_b, byte rw_b, byte ffw_b, byte reel_b)
{
  // remove this: setupButton(multi_jog, multi_jog_b);
  setupButton("Stop button", stop_button, stop_b);
  setupButton("Play backwards button", play_backwards_button, playb_b);
  setupButton("Play button", play_button, play_b);
  setupButton("Rec button", rec_button, rec_b);
  setupButton("RW button", rw_button, rw_b);
  setupButton("FFW button", ffw_button, ffw_b);
  setupButton("Reel-to-reel switch", reel_master_switch, reel_b);
}

void FilmScanner::setupButton(String t, button &b, byte pin)
{
  b.type = t;
  b.pin = pin;
  pinMode (b.pin, INPUT);
  delay(1);

  if (debugMode == true)
  {
    Serial.println(String(b.type)+" set to pin " + String(pin));
  }
}

void FilmScanner::interruptRewinding()
{
  is_rewinding = false;
}

void FilmScanner::setupSensor(sensor &s, byte pin)
{
  s.pin = pin;
  s.pressed = false;
  pinMode (s.pin, INPUT);
  delay(1);

  if (debugMode == true)
  {
    Serial.println("Sensor set to pin " + String(pin));
  }
}

void FilmScanner::setGateSensorToPin(byte pin) {
  gate_sensor.pin = pin;
  gate_sensor.pressed = false;
  pinMode (gate_sensor.pin, INPUT);
  delay(1);

  if (debugMode == true)
  {
    Serial.println("Gate sensor set to pin " + String(pin));
  }
}

void FilmScanner::setMode(byte m)
{
  mode = m;
  if (debugMode == true)
  {
    Serial.println("Mode set to " + String(m));
  }
}

byte FilmScanner::getMode()
{
  return mode;
}

void FilmScanner::setPulseDelay(int _p)
{
  custom_delay = _p;
}

int FilmScanner::getPulseDelay()
{
  return min_delay + abs(custom_delay);
}

void FilmScanner::moveOneStep(StepperMotor &m)
{
  digitalWrite(m.pulse_pin, HIGH);
  delayMicroseconds(min_delay);
  digitalWrite(m.pulse_pin, LOW);
  delayMicroseconds(pulse_delay);
}

void FilmScanner::moveOneStep(StepperMotor &m1, StepperMotor &m2)
{
  digitalWrite(m1.pulse_pin, HIGH);
  digitalWrite(m2.pulse_pin, HIGH);
  // TODO ADD RAMP delayMicroseconds(min_delay+abs(custom_delay)+abs(ramp_delay));
  delayMicroseconds(min_delay + abs(custom_delay));
  digitalWrite(m1.pulse_pin, LOW);
  digitalWrite(m2.pulse_pin, LOW);
  delayMicroseconds(pulse_delay);
}

void FilmScanner::moveOneStep(StepperMotor &m1, StepperMotor &m2, StepperMotor &m3)
{
  digitalWrite(m1.pulse_pin, HIGH);
  digitalWrite(m2.pulse_pin, HIGH);
  digitalWrite(m3.pulse_pin, HIGH);
  delayMicroseconds(min_delay);
  digitalWrite(m1.pulse_pin, LOW);
  digitalWrite(m2.pulse_pin, LOW);
  digitalWrite(m3.pulse_pin, LOW);
  delayMicroseconds(pulse_delay);
}

void FilmScanner::rewindOneStep(StepperMotor &m, int d) // TODO: d not used, use as delay ramp
{
  digitalWrite(m.pulse_pin, HIGH);
  delayMicroseconds(min_delay);
  digitalWrite(m.pulse_pin, LOW);
  delayMicroseconds(min_delay + d);
}

void FilmScanner::rewindOneStep(StepperMotor &m1, StepperMotor &m2, int d) // TODO: d not used, use as delay ramp
{
  digitalWrite(m1.pulse_pin, HIGH);
  digitalWrite(m2.pulse_pin, HIGH);
  delayMicroseconds(min_delay);
  digitalWrite(m1.pulse_pin, LOW);
  digitalWrite(m2.pulse_pin, LOW);
  delayMicroseconds(min_delay + d);
}

void FilmScanner::moveOneFrame(StepperMotor &m1, StepperMotor &m2)
{
  gate_sensor.pressed = false; //default
  readytodrop = false; //default

      // MOVE GATE FIRST
    for (int i = 0; i < 400; i++)
    {
      moveOneStep(m2);
    }
   for (int i = 0; i < 400; i++)
    {
      if (digitalRead(lower_swing_arm.pin) == HIGH)
      {
        i = 1000;
      }
      else
      {
      moveOneStep(m1);
      }
    }

/*
  // MAKE SURE THE FILM IS NOT PULLING TOO MUCH
  if (digitalRead(lower_swing_arm.pin) == HIGH)
  {
    //Forward 1000 steps? TODO: CHOOSE ANOTHER HARD CODED VALUE?
    
    // MOVE GATE FIRST
    for (int i = 0; i < 400; i++)
    {
      moveOneStep(m2);
    }
    // MOVE 
    for (int i = 0; i < 100; i++)
    {
      moveOneStep(m1);
    }
  }
  else
  {
    for (int i = 0; i < 400; i++)
    {
      moveOneStep(m2);
    }
    for (int i = 0; i < 400; i++)
    {
      moveOneStep(m1);
    }
  }
  */
}

// USE this only for Calibration Play!!

void FilmScanner::moveOneFrameCalibration(StepperMotor &m1, StepperMotor &m2)
{
  gate_sensor.pressed = false; //default
  readytodrop = false; //default

  //Forward 1000 steps? TODO: CHOOSE ANOTHER HARD CODED VALUE?
  for (int i = 0; i < 1000; i++)
  {

    moveOneStep(m1, m2);

    // Detect dropped frames
    // TODO: Calculate automatically average threshold >> Too expensive?

    if (i > 450)
    {
      dropped_frames++;
        // TODO: Send error message to main program OR (better) go back one frame
      if (debugMode == true)
      {
        //Serial.println("Dropped frame, total number of dropped frames: " + String(dropped_frames));
      }
    }

    // Do not detect switch state until motor has rotated
    if (i > 150)
    {
      // read the state of the frame detection switch value:
      // if switch is pressed break out from for loop
      if (digitalRead(gate_sensor.pin) == HIGH)
      {
        if (debugMode == true)
        {
          Serial.println("Frame detected after " + String(i) + " steps!");
        }
        gate_sensor.pressed = true;
        //readytodrop = true;
        // break out from loop when frame is detected
        steps_for_one_frame = i;
       // Serial.println(steps_for_one_frame);
        i = 1000;
      }


    }

  }
  // here to exit from calibration after the success
  if(mode!=4) // when not playing
  {
   setMode(6);
   stopPlaying();
  }
}

void FilmScanner::rewinding(StepperMotor &m)
{
  for (int i = 0; i < 500; i++) // TODO: Miten looppi muokataan jatkuvaksi?
  {
    rewindOneStep(m, 0); // TODO: USE delay_ramp here instead of 0, currently parameter not used, see func
  }
}

void FilmScanner::rewinding(StepperMotor &m1, StepperMotor &m2)
{
  //Forward 1000 steps? TODO: CHOOSE ANOTHER HARD CODED VALUE?
  for (int i = 0; i < 500; i++)
  {
    rewindOneStep(m1, m2, 0);
  }
}

void FilmScanner::speedUpRewinding()
{
  nopeutuva = true;
  hidastuva = false;
  delay_ramp = max_delay;
  is_rewinding = true;
}
void FilmScanner::slowDownRewinding()
{
  hidastuva = true;
}

void FilmScanner::stopRewinding()
{
  is_rewinding = false;
  nopeutuva = false;
  hidastuva = false;
}


void FilmScanner::readControlPanel()
{
  delay(1);

  if (digitalRead(reel_master_switch.pin) == HIGH)
  {
    delay(1);

    if (is_playing == true)
    {
      stopPlaying();
      stopRecording();
    }

    if (digitalRead(rw_button.pin) == HIGH && auto_ffw == true)
    {
      delay(1);
      // stop rewinding (after another button is pressed again)
      mode = 1;
      auto_ffw = false;
      delay(1000);
    }

    else if (digitalRead(ffw_button.pin) == HIGH && auto_ffw == false)
    {
      delay(1);
      mode = 2; // REEL-TO-REEL: FFW
      // TODO: Count here how long button is pressed
      auto_ffw_counter++;

      // if button is pressed long enough start auto rewinding
      if (auto_ffw_counter > auto_rewind_couter_limit)
      {
        auto_ffw = true;
      }

      if (ffw_button_down == false)
      {
        running_direction_set = false;
        ffw_button_down = true;
      }
      running_direction = true;
      speedUpRewinding();
    }
    else if (digitalRead(ffw_button.pin) == HIGH && auto_ffw == true && auto_ffw_counter > auto_rewind_couter_limit)
    {
      delay(1);
      // continue rewinding while button pressed
      mode = 2;
    }
    else if (digitalRead(ffw_button.pin) == HIGH && auto_ffw == true && auto_ffw_counter < auto_rewind_couter_limit)
    {
      delay(1);
      // stop rewinding (after button is pressed again)
      mode = 1;
      auto_ffw = false;
    }
    else if (digitalRead(ffw_button.pin) == LOW && auto_ffw == true)
    {
      delay(1);
      // continue rewinding automatically, no button pressed
      mode = 2;
      auto_ffw_counter = 0;
    }
    else if (digitalRead(rw_button.pin) == HIGH)
    {
      delay(1);
      mode = 3; // REEL-TO-REEL: RW

      if (rw_button_down == false)
      {
        running_direction_set = false;
        rw_button_down = true;
      }
      running_direction = false;
      speedUpRewinding();
    }
    else
    {
      delay(1);
      mode = 1; // REEL-TO-REEL: STOP
      // TODO: reset counter
      auto_ffw = false;
      auto_ffw_counter = 0;

      if (is_rewinding == true)
      {
        slowDownRewinding();
      }
      stopRewinding(); // TODO: CHANGE TO RAMP
      ffw_button_down = false;
      rw_button_down = false;
    }
  }
  else
  {
    // PLAY MODE
    delay(1); // delay in between reads for stability
    auto_ffw_counter = 0;
    auto_ffw = false;


    if (is_playing == false) // better:  mode == 6
    {
      if (digitalRead(rec_button.pin) == HIGH)
      {
        delay(1); // delay in between reads for stability
        mode = 4; // PLAY AND REC
        startPlayingForwards();
        startRecording();
      }
      else if (digitalRead(play_button.pin) == HIGH)
      {
        delay(1); // delay in between reads for stability
        mode = 4; // PLAY
        startPlayingForwards();
      }
      else if (digitalRead(play_backwards_button.pin) == HIGH)
      {
        delay(1); // delay in between reads for stability
        mode = 5; // play forwards calibration
        startPlayingCalibration();
      }
      else
      {
        delay(1); // delay in between reads for stability
        mode = 6; // STOPPED
      }
    }
    else
    {
      // WHILE PLAYING

      if (digitalRead(stop_button.pin) == HIGH)
      {
        delay(1); // delay in between reads for stability
        mode = 6; // STOP (PLAY + REC)
        stopPlaying();
        stopRecording();
      }
    }
    delay(1); // delay in between reads for stability
  }
}

void FilmScanner::debugControlPanel()
{
  delay(1);
  if (digitalRead(reel_master_switch.pin) == HIGH) // TSEKKAA KELAUS
  {
    // REEL-TO-REEL MODE
    if (digitalRead(ffw_button.pin) == HIGH)
    {
      delay(1);
      Serial.println("FilmScanner FFW button connect to pin: " + String(ffw_button.pin) + " is pressed");
    }
    else if (digitalRead(rw_button.pin) == HIGH)
    {
      delay(1);
      Serial.println("FilmScanner RW button connect to pin: " + String(rw_button.pin) + " is pressed");
    }
    else
    {
      delay(1);
      Serial.println("FilmScanner reel-to-reel connect to pin: " + String(reel_master_switch.pin) + " is on");
    }
  }
  else
  {
    // PLAY MODE
    delay(1);

    if (digitalRead(rec_button.pin) == HIGH)
    {
      delay(1);
      // START PLAY AND REC
      Serial.println("FilmScanner REC button connect to pin: " + String(rec_button.pin) + " is pressed");
    }
    else if (digitalRead(play_button.pin) == HIGH)
    {
      delay(1); // delay in between reads for stability
      // START PLAY
      Serial.println("FilmScanner PLAY button connect to pin: " + String(play_button.pin) + " is pressed");
    }
    else if (digitalRead(play_backwards_button.pin) == HIGH)
    {
      delay(1); // delay in between reads for stability
      // START PLAY Calibration play
      Serial.println("FilmScanner PLAY BACK button connect to pin: " + String(play_backwards_button.pin) + " is pressed");
    }
    else if (digitalRead(stop_button.pin) == HIGH)
    {
      delay(1); // delay in between reads for stability
      // STOP PLAY (AND REC)
      Serial.println("FilmScanner STOP button connect to pin: " + String(stop_button.pin) + " is pressed");
    }
    // TODO: Add multi-jog functionality, default to make button click possible
  }
  delay(1); // delay in between reads for stability
}


void FilmScanner::startPlayingForwards()
{
  running_direction_set = false;
  running_direction = true;
  is_playing = true;
}

void FilmScanner::startPlayingCalibration()
{
  running_direction_set = false;
  running_direction = true;
  is_playing = true;
}

void FilmScanner::startPlayingBackwards()
{
  running_direction_set = false;
  running_direction = false;
  is_playing = true;
}

void FilmScanner::stopPlaying()
{
  is_playing = false;
}

void FilmScanner::startRecording()
{
  is_recording = true;
}

void FilmScanner::stopRecording()
{
  is_recording = false;
}

bool FilmScanner::isRecording() {
  return is_recording;
}

void FilmScanner::lockMotor(StepperMotor &m)
{
  if (m.enabled == DISABLED)
  {
    digitalWrite(m.enable_pin, ENABLED);                    // täytyy tsekkaa onko logiikka unlock-lock oikein päin ???
    m.enabled = ENABLED;
  }
}

void FilmScanner::unlockMotor(StepperMotor &m)
{
  if (m.enabled == ENABLED)
  {
    digitalWrite(m.enable_pin, DISABLED);
    m.enabled = DISABLED;
  }
}

void FilmScanner::lockMotors(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate)
{
  if (m1.enabled == DISABLED && m2.enabled == DISABLED && m_gate.enabled == DISABLED)
  {
    digitalWrite(m1.enable_pin, ENABLED);
    digitalWrite(m2.enable_pin, ENABLED);
    digitalWrite(m_gate.enable_pin, ENABLED);
    m1.enabled = ENABLED;
    m2.enabled = ENABLED;
    m_gate.enabled = ENABLED;
  }
}
void FilmScanner::unlockMotors(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate)
{
  if (m1.enabled == ENABLED && m2.enabled == ENABLED && m_gate.enabled == DISABLED)
  {
    digitalWrite(m1.enable_pin, DISABLED);
    digitalWrite(m2.enable_pin, DISABLED);
    digitalWrite(m_gate.enable_pin, DISABLED);
    m1.enabled = DISABLED;
    m2.enabled = DISABLED;
    m_gate.enabled = DISABLED;
  }
}

void FilmScanner::setMotorDirectionForward(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate)
{
  if (running_direction_set == false)
  {
    digitalWrite(m1.direction_pin, FORWARDS);
    digitalWrite(m2.direction_pin, FORWARDS);
    digitalWrite(m_gate.direction_pin, FORWARDS);
    delay(1);
    running_direction_set = true;
    running_direction = true; // FORWARDS
    if (debugMode == true)
    {
      Serial.println("Running direction set to " + String(running_direction));
    }
  }
  else
  {
    // already set correctly, do nothing
    if (debugMode == true)
    {
      Serial.println("Do nothing, running direction already set to " + String(running_direction));
    }
  }
}

void FilmScanner::setMotorDirectionBackward(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate) {
  if (running_direction_set == false)
  {
    digitalWrite(m1.direction_pin, BACKWARDS);
    digitalWrite(m2.direction_pin, BACKWARDS);
    digitalWrite(m_gate.direction_pin, BACKWARDS);
    delay(1);
    running_direction_set = true;
    running_direction = false; //BACKWARDS
    if (debugMode == true)
    {
      Serial.println("Running direction set to " + String(running_direction));
    }
  }
  else
  {
    // already set correctly, do nothing
    if (debugMode == true)
    {
      Serial.println("Do nothing, running direction already set to: " + String(running_direction));
    }
  }
}

bool FilmScanner::isRunningForwards() {
  return running_direction;
}

void FilmScanner::captureFrame()
{
  delay(10);
  // Focus and Shutter pins shorts to GND, use transistor
  digitalWrite(capture_output.pin, HIGH);
  // Wait for camera to respond
  delay(100);
  digitalWrite(capture_output.pin, LOW);
}
