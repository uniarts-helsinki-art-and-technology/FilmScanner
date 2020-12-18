#ifndef FilmScanner_h
#define FilmScanner_h

#include "Arduino.h"
//#include <PrintEx.h>

#define FORWARDS true
#define BACKWARDS false
#define DISABLED HIGH
#define ENABLED LOW

typedef struct {
  byte pulse_pin;
  byte direction_pin;
  byte enable_pin;
  boolean enabled = DISABLED;
 // boolean direction_forward = true;
} StepperMotor;

typedef struct {
  byte pin;
  boolean pressed = false;
} button;

typedef struct {
  byte pin;
  boolean pressed = false;
} sensor;

typedef struct {
  byte pin_A;
  byte pin_B;
  byte pin_Switch;
  int counter = 0;
  int currentStateA;
  int lastStateA;
  boolean pressed = false;
  String currentDir ="";
  unsigned long lastButtonPress = 0;
} encoder;


class FilmScanner
{
  public:
        FilmScanner(byte _mode);

        void enableDebugMode();
        void disableDebugMode();
        
        void setupMotor(StepperMotor &m, byte pulse_pin, byte direction_pin, byte enable_pin);

        void setControlPanelButtonPins(byte multi_jog_b,byte stop_b,byte playb_b,byte play_b,byte rec_b,byte rw_b,byte ffw_b, byte reel_b);

        void setupEncoder(byte _a,byte _b,byte _sw);
        void readEncoder();
        int getEncoderCounter();
        
        void setMode(byte m);
        byte getMode();
        
        void setGateSensorToPin(byte pin);
        
        void moveOneFrame(StepperMotor &m1, StepperMotor &m2);
        void moveOneFrame(StepperMotor &m1, StepperMotor &m2, StepperMotor &m3);
        void rewinding(StepperMotor &m);
        void rewinding(StepperMotor &m1, StepperMotor &m2);
         //void rewinding(boolean dir, StepperMotor &m1, StepperMotor &m2);

        // set dir pin to set direction
        void setMotorDirectionForward(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate);
        void setMotorDirectionBackward(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate);

        // set enable pins to lock/unlock motors
        void lockMotors(StepperMotor &m1,StepperMotor &m2, StepperMotor &m_gate);
        void unlockMotors(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate);

        // read buttons in control panel and set functionality
        void readControlPanel(); // MAIN FUNCTIONALITY DEFINE HERE
        void debugControlPanel();

        // Do we need this? int getPulseDelay();
        // Do we need this? void setDelay(int d);

        bool isPlayingForwards();
        bool isPlayingBackwards();
        // Do we need this? bool isRewinding();
        bool isRewindingForwards();
        bool isRewindingBackwards();
        bool isRecording();
        
        void lockMotor(StepperMotor &m);
        void unlockMotor(StepperMotor &m);
        
  private:
          void setupButton(button &b, byte pin);
          void setupSensor(sensor &s, byte pin);
          void setupInterruptButton(button &b, byte pin);
          void interruptRewinding();
  
          void moveOneStep(StepperMotor &m);
          void moveOneStep(StepperMotor &m1, StepperMotor &m2);
          void moveOneStep(StepperMotor &m1, StepperMotor &m2, StepperMotor &m3);
          void rewindOneStep(StepperMotor &m, int d);
          void rewindOneStep(StepperMotor &m1, StepperMotor &m2, int d);
  
          void startPlayingForwards();
          void startPlayingBackwards();
  
          void stopPlaying();
  
          void speedUpRewinding();
          void slowDownRewinding();
          void stopRewinding();
          
          void startRecording();
          void stopRecording();

          //bool getRunningDirection();
          bool isRunningForwards();
  
          button play_button, play_backwards_button, stop_button, rec_button, ffw_button, rw_button, reel_master_switch, multi_jog;
          //motor m1,m2,m_gate; >> currently outside of class
          sensor gate_sensor;
          encoder enc;

          void move_one_step(StepperMotor m1, StepperMotor m2, StepperMotor m3, int d);

          float getArrayAverage(int a[]);

          byte mode = 0;
          boolean running_direction =  FORWARDS;
          boolean running_direction_set =  false;
          boolean debugMode = false;
          boolean is_recording = false;
          boolean is_playing = false;
         // volatile boolean is_rewinding = false;
          boolean is_rewinding = false;
          boolean running_direction_changed = false;
          boolean ffw_button_down=false;
          boolean rw_button_down=false;
          
         // byte PIN_FRAME_DETECTION_SWITCH = 0;
          int pulse_delay = 100;
          const int min_delay = 50; // AccelStepper kirjastossa 1 Microsec
          const int max_delay = 1000;
          int delay_ramp = min_delay;
          boolean hidastuva =false;
          boolean nopeutuva =false;

          long dropped_frames = 0;
          int stepsBetweenFrames[5] = {400,400,410,400,413};
          byte stepCount = 0;

          //Using formatted stirngs without a buffer.
          //PrintEx debug = Serial; //Wrap the Serial object in a PrintEx interface.

};


#endif
