#ifndef FilmScanner_h
#define FilmScanner_h

#include "Arduino.h"

#define FORWARDS HIGH
#define BACKWARDS LOW
#define DISABLED HIGH
#define ENABLED LOW

typedef struct {
  byte pulse_pin;
  byte direction_pin;
  byte enable_pin;
  boolean enabled = DISABLED;
} StepperMotor;

typedef struct {
  String type;
  byte pin;
} button;

typedef struct {
  byte pin;
} output;

typedef struct {
  byte pin;
  boolean pressed = false;
} sensor;

class FilmScanner
{
  public:
    FilmScanner(byte _mode);

    // DEBUG
    void enableDebugMode();

    // SETUP
    void setupMotor(StepperMotor &m, byte pulse_pin, byte direction_pin, byte enable_pin);
    void setControlPanelButtonPins(byte stop_b, byte playb_b, byte play_b, byte rec_b, byte rw_b, byte ffw_b, byte reel_b);
    void setCameraRemoteControlPin(byte );

    // MODES
    void setMode(byte m);
    byte getMode();

    // SENSORS
    void setGateSensorToPin(byte pin);
    void setSwingArmSensorsToPin(byte upper_arm_pin, byte lower_arm_pin);

    // MOTORS
    void moveOneFrame(StepperMotor &m1, StepperMotor &m2);
    void moveOneFrameCalibration(StepperMotor &m1, StepperMotor &m2);
    void moveOneFrame(StepperMotor &m1, StepperMotor &m2, StepperMotor &m3);
    void rewinding(StepperMotor &m);
    void rewinding(StepperMotor &m1, StepperMotor &m2);
    void setMotorDirectionForward(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate);
    void setMotorDirectionBackward(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate);
    void lockMotor(StepperMotor &m);
    void unlockMotor(StepperMotor &m);
    void lockMotors(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate);
    void unlockMotors(StepperMotor &m1, StepperMotor &m2, StepperMotor &m_gate);

    // CONTROL PANEL
    void readControlPanel(); // MAIN FUNCTIONALITY DEFINE HERE
    void debugControlPanel();

    // SPEED & DELAY
    void setPulseDelay(int _p);
    int getPulseDelay();
    int custom_delay = 0;

    // CAMERA CONTROL
    void captureFrame();
    bool isRecording();

  private:
    void setupButton(String t, button &b, byte pin);
    void setupSensor(sensor &s, byte pin);
    void interruptRewinding();

    void moveOneStep(StepperMotor &m);
    void moveOneStep(StepperMotor &m1, StepperMotor &m2);
    void moveOneStep(StepperMotor &m1, StepperMotor &m2, StepperMotor &m3);
    void rewindOneStep(StepperMotor &m, int d);
    void rewindOneStep(StepperMotor &m1, StepperMotor &m2, int d);

    void startPlayingForwards();
    void startPlayingCalibration();
    void startPlayingBackwards();

    void stopPlaying();

    void speedUpRewinding();
    void slowDownRewinding();
    void stopRewinding();

    void startRecording();
    void stopRecording();

    bool isRunningForwards();

    button play_button, play_backwards_button, stop_button, rec_button, ffw_button, rw_button, reel_master_switch, multi_jog;
    sensor gate_sensor, upper_swing_arm, lower_swing_arm;
    output capture_output;

    void move_one_step(StepperMotor m1, StepperMotor m2, StepperMotor m3, int d);

    byte mode = 0;
    byte prev_mode = 0;
    boolean running_direction = FORWARDS;
    boolean running_direction_set = false;
    boolean debugMode = true;
    boolean is_recording = false;
    boolean is_playing = false;
    boolean is_rewinding = false;
    boolean running_direction_changed = false;
    boolean ffw_button_down = false;
    boolean rw_button_down = false;
    boolean auto_ffw = false;
    boolean auto_rw = false;
    boolean readytodrop = false;

    int auto_ffw_counter = 0;
    int auto_rw_counter = 0;
    int auto_rewind_couter_limit = 3;
    int pulse_delay = 100;
    int steps_for_one_frame = 0;
    const int min_delay = 500; // AccelStepper kirjastossa 1 Microsec, 500 micros sopiva minimiarvo
    const int max_delay = 1000;
    int delay_ramp = min_delay;
    boolean hidastuva = false;
    boolean nopeutuva = false;

    long dropped_frames = 0;


};
#endif
