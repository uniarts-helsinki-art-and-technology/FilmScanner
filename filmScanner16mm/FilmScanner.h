#ifndef FilmScanner_h
#define FilmScanner_h

#include "Arduino.h"

#define FORWARDS true
#define BACKWARDS false
#define DISABLED HIGH
#define ENABLED LOW

typedef struct {
  byte pulse_pin;
  byte direction_pin;
  byte enable_pin;
  boolean enabled = DISABLED;
} StepperMotor;

typedef struct {
  byte pin;
} button;

typedef struct {
  byte pin;
} output;

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
  volatile bool state = false;
} encoder;


class FilmScanner
{
  public:
        FilmScanner(byte _mode);

        // DEBUG
        void enableDebugMode();
        void disableDebugMode();

        // SETUP
        void setupMotor(StepperMotor &m, byte pulse_pin, byte direction_pin, byte enable_pin);
        void setControlPanelButtonPins(byte stop_b,byte playb_b,byte play_b,byte rec_b,byte rw_b,byte ffw_b, byte reel_b);
        void setEncoderPins(byte _a,byte _b,byte _sw);
        void setCameraRemoteControlPin(byte );
        void setSwingArmSensorsToPin(byte upper_arm_pin, byte lower_arm_pin);

        // ENCODER
        void readEncoder();
        int getEncoderCounter();
        
        // MODES
        void setMode(byte m);
        byte getMode();

        // SENSORS
        void setGateSensorToPin(byte pin);

        // MOTORS
        void moveOneFrame(StepperMotor &m1, StepperMotor &m2);
        void moveOneFrame(StepperMotor &m1, StepperMotor &m2, StepperMotor &m3);
        void rewinding(StepperMotor &m);
        void rewinding(StepperMotor &m1, StepperMotor &m2);
        void setMotorDirectionForward(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate);
        void setMotorDirectionBackward(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate);
        void lockMotor(StepperMotor &m);
        void unlockMotor(StepperMotor &m);        
        void lockMotors(StepperMotor &m1,StepperMotor &m2, StepperMotor &m_gate);
        void unlockMotors(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate);
        
        // CONTROL PANEL
        void readControlPanel(); // MAIN FUNCTIONALITY DEFINE HERE
        void debugControlPanel();

        // SPEED & DELAY
        void setPulseDelay(int _p);
        int getPulseDelay();

        // CAMERA CONTROL
        void captureFrame();

        // Do we need this? int getPulseDelay();
        // Do we need this? void setDelay(int d);
        
        bool isRecording();
        encoder enc; // is public because of interrupt calls
        int custom_delay = 0;
        
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

          bool isRunningForwards();
  
          button play_button, play_backwards_button, stop_button, rec_button, ffw_button, rw_button, reel_master_switch, multi_jog;
          //motor m1,m2,m_gate; >> currently outside of class
          sensor gate_sensor, upper_swing_arm, lower_swing_arm;
          output capture_output;
          
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
          const int min_delay = 500; // AccelStepper kirjastossa 1 Microsec
          const int max_delay = 1000;
          int delay_ramp = min_delay;
          int ramp_delay = 0;
          boolean hidastuva =false;
          boolean nopeutuva =false;

          long dropped_frames = 0;
          int stepsBetweenFrames[5] = {400,400,410,400,413};
          byte stepCount = 0;

};
#endif
