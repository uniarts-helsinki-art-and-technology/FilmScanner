#include "Arduino.h"
#include "FilmScanner.h"

FilmScanner::FilmScanner(byte _mode){
     mode = _mode;
}

void FilmScanner::enableDebugMode(){
  Serial.begin(9600);
  Serial.println("Debug mode enabled. Serial communication started");
  //debug.printf("Debug mode enabled. Serial communication started!\n");
  debugMode = true;
}

void FilmScanner::disableDebugMode(){
  debugMode = false;
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
  digitalWrite(m.enable_pin,ENABLED);
  delay(1);
}

void FilmScanner::setEncoderPins(byte _a,byte _b,byte _sw)
{
  enc.pin_A = _a;
  enc.pin_B = _b;
  enc.pin_Switch = _sw;   
  pinMode(enc.pin_A,INPUT_PULLUP);
  pinMode(enc.pin_B,INPUT_PULLUP);
  pinMode(enc.pin_Switch, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(enc.pin_A), encoderStateChange, CHANGE); // funktion kutsuminen täältä ei toimi, miksi?
  //attachInterrupt(digitalPinToInterrupt(enc.pin_B), encoderStateChange, CHANGE); // funktion kutsuminen täältä ei toimi, miksi?
  // Read the initial state of CLK
  enc.lastStateA = digitalRead(enc.pin_A);
  // Put in a slight delay to help debounce the reading
  delay(1);
}

void FilmScanner::setCameraRemoteControlPin(byte capture)
{
  capture_output.pin = capture;
  pinMode(capture_output.pin,OUTPUT);
}

void FilmScanner::setSwingArmSensorsToPin(byte upper_arm_pin, byte lower_arm_pin)
{
  upper_swing_arm.pin = upper_arm_pin;
  lower_swing_arm.pin = lower_arm_pin;
  pinMode(capture_output.pin,OUTPUT);
}

void FilmScanner::readEncoder()
{
  // Read the current state of pin A (CLK)
  enc.currentStateA = digitalRead(enc.pin_A);

  // If last and current state of pin A (CLK) are different, then pulse occurred
  // React to only 1 state change to avoid double count
  if (enc.currentStateA != enc.lastStateA  && enc.currentStateA == 1)
  {
    // If the pin B (DT) state is different than the CLK state then
    // the encoder is rotating CCW
    if (digitalRead(enc.pin_B) != enc.currentStateA)
    {
      enc.counter --;
      enc.currentDir ="CCW";
    }
    else
    {
      // Encoder is rotating CW
      enc.counter ++;
      enc.currentDir ="CW";
    }
    
    if(debugMode==true)
    {
      Serial.print("Direction: ");
      Serial.print(enc.currentDir);
      Serial.print(" | Counter: ");
      Serial.println(enc.counter);
    }
  }

  // Remember last state of pin A (CLK)
  enc.lastStateA = enc.currentStateA;

  // Read the button state
  int btnState = digitalRead(enc.pin_Switch);

  // If we detect LOW signal, button is pressed
  if (btnState == LOW) {
    // If 50ms have passed since last LOW pulse, it means that the
    // button has been pressed, released and pressed again
    if (debugMode==true && millis() - enc.lastButtonPress > 50)
    {
      Serial.println("Button pressed!");
    }
    
    // Remember last button press event
    enc.lastButtonPress = millis();
  }

  // Put in a slight delay to help debounce the reading
  delay(1);

  // Reset encoder state (for interrupt function)
  enc.state  = false;
}

int FilmScanner::getEncoderCounter()
{
  return enc.counter;
}

void FilmScanner::setControlPanelButtonPins(byte stop_b,byte playb_b,byte play_b,byte rec_b,byte rw_b,byte ffw_b, byte reel_b)
{
  // remove this: setupButton(multi_jog, multi_jog_b);
  setupButton(stop_button, stop_b);
  setupButton(play_backwards_button, playb_b);
  setupButton(play_button, play_b);
  setupButton(rec_button, rec_b);
  setupButton(rw_button, rw_b);
  //TODO: setupInterruptButton(rw_button, rw_b);
  setupButton(ffw_button, ffw_b);
  //TODO: setupInterruptButton(ffw_button, ffw_b);
  setupButton(reel_master_switch, reel_b);
}

void FilmScanner::setupButton(button &b, byte pin)
{
  b.pin = pin;
  pinMode (b.pin, INPUT);
  delay(1);
        
  if(debugMode==true)
  {
    Serial.println("Button set to pin "+String(pin));
  }
}

void FilmScanner::setupInterruptButton(button &b, byte pin){
  b.pin = pin;
  pinMode (b.pin, INPUT);
  // TODO: attachInterrupt(digitalPinToInterrupt(18), interruptRewinding, LOW);// pins on MEGA 2, 3, 18, 19, 20, 21
  delay(1);
        
  if(debugMode==true)
  {
    Serial.println("Interrupt button set to pin "+String(pin));
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
        
  if(debugMode==true)
  {
    Serial.println("Sensor set to pin "+String(pin));
  }
}

void FilmScanner::setGateSensorToPin(byte pin){
  gate_sensor.pin = pin;
  gate_sensor.pressed = false;
  pinMode (gate_sensor.pin, INPUT);
  delay(1);

  if(debugMode==true)
  {
    Serial.println("Gate sensor set to pin "+String(pin));
  }
}

void FilmScanner::setMode(byte m)
{
  mode = m;
  if(debugMode==true)
  {
    Serial.println("Mode set to "+String(m));
  }
}

byte FilmScanner::getMode()
{      
  return mode;
}

// POISTA
//void FilmScanner::setDelay(int d){
//        pulse_delay = constrain(d, min_delay,max_delay);
//        if(debugMode==true){
//          Serial.println("Pulse delay set to "+String(d));
//          //debug.printf("Pulse delay set to %d\n",d);
//        } 
//}

// POISTA
//int FilmScanner::getPulseDelay(){
//       return pulse_delay;
//}

void FilmScanner::setPulseDelay(int _p)
{
  custom_delay = _p;
}

int FilmScanner::getPulseDelay()
{
  return custom_delay;
}

void FilmScanner::moveOneStep(StepperMotor &m)
{
  digitalWrite(m.pulse_pin,HIGH);
  delayMicroseconds(min_delay);
  digitalWrite(m.pulse_pin,LOW);
  delayMicroseconds(pulse_delay);
}

void FilmScanner::moveOneStep(StepperMotor &m1, StepperMotor &m2)
{
  digitalWrite(m1.pulse_pin,HIGH);
  digitalWrite(m2.pulse_pin,HIGH);
  delayMicroseconds(min_delay+abs(custom_delay)+abs(ramp_delay));
  digitalWrite(m1.pulse_pin,LOW);
  digitalWrite(m2.pulse_pin,LOW);
  delayMicroseconds(pulse_delay);
}

void FilmScanner::moveOneStep(StepperMotor &m1, StepperMotor &m2, StepperMotor &m3)
{
  digitalWrite(m1.pulse_pin,HIGH);
  digitalWrite(m2.pulse_pin,HIGH);
  digitalWrite(m3.pulse_pin,HIGH);
  delayMicroseconds(min_delay);
  digitalWrite(m1.pulse_pin,LOW);
  digitalWrite(m2.pulse_pin,LOW);
  digitalWrite(m3.pulse_pin,LOW);
  delayMicroseconds(pulse_delay);
}

void FilmScanner::rewindOneStep(StepperMotor &m, int d)
{
  digitalWrite(m.pulse_pin,HIGH);
  delayMicroseconds(min_delay);
  digitalWrite(m.pulse_pin,LOW);
  delayMicroseconds(min_delay+d);
}

void FilmScanner::rewindOneStep(StepperMotor &m1, StepperMotor &m2, int d)
{
  digitalWrite(m1.pulse_pin,HIGH);
  digitalWrite(m2.pulse_pin,HIGH);
  delayMicroseconds(min_delay);
  digitalWrite(m1.pulse_pin,LOW);
  digitalWrite(m2.pulse_pin,LOW);
  delayMicroseconds(min_delay+d);
}

void FilmScanner::moveOneFrame(StepperMotor &m1, StepperMotor &m2)
{
  gate_sensor.pressed = false; //default

  int ramp = 0;
  ramp_delay = 1000;
  //Forward 1000 steps? TODO: CHOOSE ANOTHER HARD CODED VALUE?
  for (int i=0; i<1000; i++)
  {
    if(i%50==0)
    {
    ramp++;
    ramp_delay = ramp_delay - (ramp*50);
    
    }
    moveOneStep(m1,m2);

    // Detect dropped frames
    // TODO: Calculate automatically average threshold >> Too expensive?
     // if(i>getArrayAverage(stepsBetweenFrames)+50)
     
      if(i>450)
      {
          dropped_frames++;
          // TODO: Send error message to main program OR (better) go back one frame
          if(debugMode==true)
          {
            Serial.println("Dropped frame, total number of dropped frames: "+String(dropped_frames));
          }
      }
     
    // Do not detect switch state until motor has rotated
    if(i>150)
    {
      // read the state of the frame detection switch value:
      // if switch is pressed break out from for loop
      if(digitalRead(gate_sensor.pin) == HIGH)
      {
          if(debugMode==true)
          {
            Serial.println("Frame detected after "+String(i)+" steps!");
          }
          gate_sensor.pressed = true;
          // save the number of steps between each frame
          //stepsBetweenFrames[stepCount++] = i;
          //if(stepCount>=5){
          //  stepCount=0;
          //}
          // break out from loop when frame is detected
          i=1000;
      }

    }

  }
}

void FilmScanner::moveOneFrame(StepperMotor &m1, StepperMotor &m2, StepperMotor &m3)
{
  gate_sensor.pressed = false; //default

  //Forward 1000 steps? TODO: CHOOSE ANOTHER HARD CODED VALUE?
  for (int i=0; i<1000; i++)
  {
    moveOneStep(m1,m2,m3);

    // Detect dropped frames
    // TODO: Calculate automatically average threshold >> Too expensive?
     // if(i>getArrayAverage(stepsBetweenFrames)+50)
     
      if(i>450)
      {
          dropped_frames++;
          // TODO: Send error message to main program OR (better) go back one frame
          if(debugMode==true)
          {
            Serial.println("Dropped frame, total number of dropped frames: "+String(dropped_frames));
          }
      }
     
    // Do not detect switch state until motor has rotated
    if(i>150)
    {
      // read the state of the frame detection switch value:
      // if switch is pressed break out from for loop
      if(digitalRead(gate_sensor.pin) == HIGH)
      {
          if(debugMode==true){
            Serial.println("Frame detected after "+String(i)+" steps!");
          }
          gate_sensor.pressed = true;
          // save the number of steps between each frame
          //stepsBetweenFrames[stepCount++] = i;
          //if(stepCount>=5){
          //  stepCount=0;
          //}
          // break out from loop when frame is detected
          i=1000;
      }
    }
  }
}


void FilmScanner::rewinding(StepperMotor &m)
{
    for (int i=0; i<500; i++) // TODO: Miten looppi muokataan jatkuvaksi?
    {
      rewindOneStep(m,0); // TODO: USE delay_ramp here instead of 0
    }
}

void FilmScanner::rewinding(StepperMotor &m1, StepperMotor &m2)
{
  //Forward 1000 steps? TODO: CHOOSE ANOTHER HARD CODED VALUE?
    for (int i=0; i<500; i++)
    {
    rewindOneStep(m1,m2,delay_ramp);
    }
/*    if(hidastuva == true && delay_ramp<max_delay)
    {
      delay_ramp++;
      if(debugMode==true)
      {
        Serial.println("Slowing down, delay ramp: "+String(delay_ramp));
      }
    }
    else if (nopeutuva == true && delay_ramp>0)
    {
      delay_ramp--;
      if(debugMode==true)
      {
        Serial.println("Speeding up, delay ramp: "+String(delay_ramp));
      }              
    }
    
    if (nopeutuva == true && delay_ramp<=0)
    {
      nopeutuva = false; // rewinding is at top speed
    }
    if(hidastuva == true && delay_ramp>=max_delay)
    {
      //  hidastuva = false;
      stopRewinding(); // stop rewinding when slowest speed is achieved
      if(debugMode==true)
      {
        Serial.println("Stop rewinding,, delay ramp: "+String(delay_ramp));
      }   
    }*/

  

 
// for (int i=0; i<500; i++)
// {
//      rewindOneStep(m1,m2,delay_ramp);

/*
        // Do not detect switch state until StepperMotor has rotated
        if(i>150 && i % 10 == 0)
        {
          // read the state of the frame detection switch value:
230          int frame_detected = digitalRead(gate_sensor.pin);

          // if gate sensor value is changed break out from for loop
          // TODO: check if this works as expected... the gate sensor should not change value
          if(frame_detected == gate_sensor.pressed)
          {
            gate_sensor.pressed = frame_detected;
            // break out from loop when frame is detectedmoveOneFrame
            if(debugMode==true){
              Serial.println("ALERT! Remove film from gate!");
            }
            i=500;
            //mode=3;
            setMode(3);
            stopRewinding();
          }
          
        }


     //   } 
        */
/*
        if(hidastuva == true && delay_ramp<max_delay)
        {
                delay_ramp++;
                if(debugMode==true){
                  Serial.println("Slowing down, delay ramp: "+String(delay_ramp));
                }
        }
        else if (nopeutuva == true && delay_ramp>0)
        {
                delay_ramp--;
                if(debugMode==true){
                  Serial.println("Speeding up, delay ramp: "+String(delay_ramp));
                }              
        }
       
        if (nopeutuva == true && delay_ramp<=0)
        {
                nopeutuva = false; // rewinding is at top speed
        }
        if(hidastuva == true && delay_ramp>max_delay)
        {
              //  hidastuva = false;
                stopRewinding(); // stop rewinding when slowest speed is achieved
                if(debugMode==true){
                  Serial.println("Stop rewinding,, delay ramp: "+String(delay_ramp));
                }   
        }
        */

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

  if(digitalRead(reel_master_switch.pin) == HIGH)
  {
    delay(1);
     
    if(is_playing == true)
    {
      stopPlaying();
      stopRecording();
    }
      
    if(digitalRead(ffw_button.pin) == HIGH)
    {
      delay(1);
      mode = 2; // REEL-TO-REEL: FFW
      
      if(ffw_button_down == false)
      {
        running_direction_set=false;
        ffw_button_down=true;
      }
        running_direction = true;
        speedUpRewinding();       
    }
    else if(digitalRead(rw_button.pin) == HIGH)
    {
      delay(1);
      mode = 3; // REEL-TO-REEL: RW
        
      if(rw_button_down == false)
      {
        running_direction_set=false;
        rw_button_down=true;
      }
        running_direction = false;
        speedUpRewinding(); 
      }
      else
      {
        delay(1);
        mode = 1; // REEL-TO-REEL: STOP

        if(is_rewinding == true)
        {
          slowDownRewinding();
        }
        stopRewinding(); // TODO: CHANGE TO RAMP
        ffw_button_down=false;
        rw_button_down=false;
      }
  }
  else
  {
    // PLAY MODE

      delay(1); // delay in between reads for stability
        
     // TODO: WAIT UNTIL RWND IS STOPPED
      //if(is_rewinding == true)
    //  {
        // REEL TO REEL SWITCH IS OFF, GENTLY STOP RWND
      //  slowDownRewinding();
    //  }
    
      if (is_playing == false) // better:  mode == 6
      {
                
     //   remove this: multi_jog.pressed = false; // default to make button click possible
        
        if(digitalRead(rec_button.pin) == HIGH)
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
        else if(digitalRead(play_backwards_button.pin) == HIGH)
        {
          delay(1); // delay in between reads for stability
          mode = 5; // PLAY BACKWARDS
          startPlayingBackwards();
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
        
        if(digitalRead(stop_button.pin) == HIGH)
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
  if(digitalRead(reel_master_switch.pin) == HIGH) // TSEKKAA KELAUS
  {
    // REEL-TO-REEL MODE
    if(digitalRead(ffw_button.pin) == HIGH)
    {
      delay(1);
      Serial.println("FilmScanner FFW button connect to pin: "+String(ffw_button.pin)+" is pressed");
    }
    else if(digitalRead(rw_button.pin) == HIGH)
    {
      delay(1);
      Serial.println("FilmScanner RW button connect to pin: "+String(rw_button.pin)+" is pressed");
    }
    else
    {
      delay(1);
      Serial.println("FilmScanner reel-to-reel connect to pin: "+String(reel_master_switch.pin)+" is on");
    }
  }
  else
  {
    // PLAY MODE
    delay(1);
                
    if(digitalRead(rec_button.pin) == HIGH)
    {
      delay(1);
      // START PLAY AND REC
      Serial.println("FilmScanner REC button connect to pin: "+String(rec_button.pin)+" is pressed");
    }
    else if (digitalRead(play_button.pin) == HIGH)
    {
      delay(1); // delay in between reads for stability
      // START PLAY
      Serial.println("FilmScanner PLAY button connect to pin: "+String(play_button.pin)+" is pressed");
    }
    else if(digitalRead(play_backwards_button.pin) == HIGH)
    {
      delay(1); // delay in between reads for stability
      // START PLAY BACKWARDS
      Serial.println("FilmScanner PLAY BACK button connect to pin: "+String(play_backwards_button.pin)+" is pressed");
    }
    else if(digitalRead(stop_button.pin) == HIGH)
    {
      delay(1); // delay in between reads for stability
      // STOP PLAY (AND REC)
      Serial.println("FilmScanner STOP button connect to pin: "+String(stop_button.pin)+" is pressed");
    }
      // remove this: multi_jog.pressed = false; // TODO: Add multi-jog functionality, default to make button click possible
  }
  delay(1); // delay in between reads for stability
}


void FilmScanner::startPlayingForwards()
{
  running_direction_set=false;
  running_direction = true;
  is_playing = true;
}

void FilmScanner::startPlayingBackwards()
{
  running_direction_set=false;
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

bool FilmScanner::isRecording(){
  return is_recording;
}

void FilmScanner::lockMotor(StepperMotor &m)
{
  if(m.enabled == DISABLED)
  {
    digitalWrite(m.enable_pin,ENABLED);
    m.enabled = ENABLED;
  }
}

void FilmScanner::unlockMotor(StepperMotor &m)
{
  if(m.enabled == ENABLED)
  {
    digitalWrite(m.enable_pin,DISABLED);
    m.enabled = DISABLED;
  }
}

void FilmScanner::lockMotors(StepperMotor &m1,StepperMotor &m2, StepperMotor &m_gate)
{
  if(m1.enabled == DISABLED && m2.enabled == DISABLED && m_gate.enabled == DISABLED)
  {
 //   noInterrupts();
    digitalWrite(m1.enable_pin,ENABLED);
    digitalWrite(m2.enable_pin,ENABLED);
    digitalWrite(m_gate.enable_pin,ENABLED);
//    interrupts();
    m1.enabled = ENABLED;
    m2.enabled = ENABLED;
    m_gate.enabled = ENABLED;
  }
}
void FilmScanner::unlockMotors(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate)
{
  if(m1.enabled == ENABLED && m2.enabled == ENABLED && m_gate.enabled == DISABLED)
  {
   // noInterrupts();
    digitalWrite(m1.enable_pin,DISABLED);
    digitalWrite(m2.enable_pin,DISABLED);
    digitalWrite(m_gate.enable_pin,DISABLED);
  //  interrupts();
    m1.enabled = DISABLED;
    m2.enabled = DISABLED;
    m_gate.enabled = DISABLED;
  }
}

void FilmScanner::setMotorDirectionForward(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate)
{
  if(running_direction_set==false)
  {
    digitalWrite(m1.direction_pin,HIGH);
    digitalWrite(m2.direction_pin,HIGH);
    digitalWrite(m_gate.direction_pin,HIGH);
    delay(1);
    running_direction_set=true;
    running_direction = true; // FORWARDS
    if(debugMode==true)
    {
      Serial.println("Running direction set to "+String(running_direction)); 
    }   
  }
  else
  {
    // already set correctly, do nothing
    if(debugMode==true)
    {
      Serial.println("Running direction already set to "+String(running_direction)); 
    } 
  }  
}

void FilmScanner::setMotorDirectionBackward(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate){
  if(running_direction_set==false)
  {
    digitalWrite(m1.direction_pin,LOW);
    digitalWrite(m2.direction_pin,LOW);
    digitalWrite(m_gate.direction_pin,LOW);
    delay(1);
    running_direction_set=true;
    running_direction = false; //BACKWARDS
    if(debugMode==true)
    {
      Serial.println("Running direction set to "+String(running_direction)); 
    } 
  }
  else
  {
    // already set correctly, do nothing
    if(debugMode==true)
    {
     // Serial.println("Running direction already set to "+String(running_direction)); 
    } 
  }
}

bool FilmScanner::isRunningForwards(){
  return running_direction;
}

float FilmScanner::getArrayAverage (int a[])
{
  int result = 0;
  for (byte i = 0; i < (sizeof(a) / sizeof(a[0])); i++) {
  result += a[i];
  }
  float average = (float)result/(sizeof(a));
  return average;
}

void FilmScanner::captureFrame()
{
  delay(10);
   // Focus and Shutter pins shorts to GND, use transistor
  digitalWrite(capture_output.pin,HIGH);
  // Wait for camera to respond
  delay(100);
  digitalWrite(capture_output.pin, LOW); 
}
