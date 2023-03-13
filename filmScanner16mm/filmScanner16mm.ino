/*   .___________________________________________________
    |   _     _     _     _     _     _     _     _     _|
    |  |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_
    | .                                                  |
    |          This is KuvA-Scanner-Code v.1.3          |
    |                                                    |
    |      description: code for 16mm telecine machine . |
    |                                                    |
    |                  date: 14.12.2022                   |
    |                                                    |
    ^----------------------------------------------------^


                       NOTES
Joakim's attempt to make the frame stop more accurate by
driving the film srocket "over the threshold"
and take the feedback when falling. smart?

14.12.2022
Fixing the lower swing arm with a switch.





*/

#include "FilmScanner.h"
//#include <Encoder.h>
#include <LiquidCrystal_I2C.h> // external library


// FILM SCANNER
FilmScanner FilmScanner(0); // set default mode: 0=stop
StepperMotor UpperReelMotor, LowerReelMotor, GateMotor;
unsigned long saved_frames_count = 0;
const int pause_between_frames = 1000;
int prev_mode = 0;
bool mode_changed = false;

// ENCODER BREAKS THE FUNCTIONALITY
// TODO: REPLACE WITH POTENTIOMETER
//Encoder rotary_encoder(18, 19); // set pins to interrupts
//float rotary_encoder_speed = 10.0; // Multiplier for frame delay
//long encoder_ref  = -999; // encoder reference value
//long encoder_value  = 0; // encoder reference value

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 21); // NOTE! Before use, run ic2_scanner sketch and get the IC2 address, 0x27 for example
bool drawLCD = true;
bool drawLCD_stopped = true;

void setup() {
  
  // Uncomment for debugging
  FilmScanner.enableDebugMode();

  // Motors
  FilmScanner.setupMotor(UpperReelMotor, 2, 3, 4);
  FilmScanner.setupMotor(LowerReelMotor, 5, 6, 7);
  FilmScanner.setupMotor(GateMotor, 8, 9, 10);

  // Control panel
  FilmScanner.setControlPanelButtonPins(38, 40, 42, 44, 46, 48, 50);
  // Camera control
  FilmScanner.setCameraRemoteControlPin(11);

  // Switches and sensors
  FilmScanner.setGateSensorToPin(30);
  FilmScanner.setSwingArmSensorsToPin(32,34); // pins (upper, lower)

  // Initialize the LCD
  lcd.init();
  // Print start messsage (mode=0)
  //printLCD(0);



}





void loop()
{
  switch (FilmScanner.getMode()) // Valitaan toiminto
  {
    case 1: // REEL-TO-REEL: STOP
      if (mode_changed)
      {
        FilmScanner.unlockMotor(UpperReelMotor);
        FilmScanner.unlockMotor(LowerReelMotor);
        FilmScanner.unlockMotor(GateMotor);
      }
      // scanner is stopped, waiting user to push ffw / rw button
      break;

    case 2: // REEL-TO-REEL: FFW
      if (mode_changed)
      {
        FilmScanner.setMotorDirectionForward(UpperReelMotor, LowerReelMotor, GateMotor);
        FilmScanner.unlockMotor(UpperReelMotor);
        FilmScanner.unlockMotor(GateMotor);
        FilmScanner.lockMotor(LowerReelMotor);
      }
      FilmScanner.rewinding(LowerReelMotor);

      //      // READ ENCODER VALUE > TESTING
      // ENCODER BREAKS THE FUNCTINALITY
      //    long encoder_current = rotary_encoder.read();

      
      break;

    case 3: // REEL-TO-REEL: RW
      if (mode_changed)
      {
        FilmScanner.setMotorDirectionBackward(UpperReelMotor, LowerReelMotor, GateMotor);
        FilmScanner.unlockMotor(LowerReelMotor);
        FilmScanner.unlockMotor(GateMotor);
        FilmScanner.lockMotor(UpperReelMotor);
      }
      FilmScanner.rewinding(UpperReelMotor);
      break;

    case 4: // PLAY AND RECORD
      if (mode_changed)
      {
        FilmScanner.setMotorDirectionForward(UpperReelMotor, LowerReelMotor, GateMotor);
        FilmScanner.unlockMotor(UpperReelMotor);
        FilmScanner.lockMotor(LowerReelMotor);
        FilmScanner.lockMotor(GateMotor);
        FilmScanner.moveOneFrameCalibration(LowerReelMotor, GateMotor); //Calibrate for first frame
      }
      else
      {
        FilmScanner.moveOneFrame(LowerReelMotor, GateMotor);
      }
      // RECORDING
      if (FilmScanner.isRecording() == true)
      {
        FilmScanner.captureFrame();
        saved_frames_count = saved_frames_count + 1;
      }
      delay(pause_between_frames);


      break;

    case 5: // CALIBRATION PLAY
      if (mode_changed)
      {
        FilmScanner.setMotorDirectionForward(UpperReelMotor, LowerReelMotor, GateMotor);
        FilmScanner.unlockMotor(UpperReelMotor);
        FilmScanner.lockMotor(LowerReelMotor);
        FilmScanner.lockMotor(GateMotor);
      }
      FilmScanner.moveOneFrameCalibration(LowerReelMotor, GateMotor); //Calibrate
      FilmScanner.setMode(byte(6));
      delay(pause_between_frames);

      break;

    case 6:// STOPPED
      if (mode_changed)
      {
        FilmScanner.unlockMotor(UpperReelMotor);
        FilmScanner.unlockMotor(LowerReelMotor);
        FilmScanner.unlockMotor(GateMotor);
      }
      // do nothing, wait user to push: play / play_b / rec button
      break;

    case 7: // ERROR:
      break;

    default:
      break;
  }

  // Print information about the selected mode
  printLCD(FilmScanner.getMode());

  // Read control panel buttons
  FilmScanner.readControlPanel();

  if (FilmScanner.getMode() != prev_mode)
  {
    mode_changed = true;
    prev_mode = FilmScanner.getMode();
    lcd.clear();
  }
  else
  {
    mode_changed = false;
  }
}


void printLCD (int mode)
{

  
  switch (mode)
  {
    case 0:
      // Print a message to the LCD.
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print("KuvaScan 1.3");
      lcd.setCursor(0, 1);
      lcd.print("Starting...");
      delay(1000);
      break;

    case 1:
      if (drawLCD == true)
      {
        // Print a message to the LCD.
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Reel-To-Reel:");
        drawLCD = false;
      }
      drawLCD_stopped = true;
      break;

    case 2:
      if (drawLCD == true)
      {
        // Print a message to the LCD.
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Reel-To-Reel:FFW");
        drawLCD = false;
      }
//      lcd.setCursor(0, 1);
//      lcd.print("    ");
//      lcd.setCursor(0, 1);
//      lcd.print(String(encoder_value));
      
      drawLCD_stopped = true;
      break;

    case 3:
      if (drawLCD == true)
      {
        // Print a message to the LCD.
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Reel-To-Reel:RW");
        drawLCD = false;
      }
      drawLCD_stopped = true;
      break;

    case 4:
      // Print a message to the LCD (only once)
      if (drawLCD == true)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        if (FilmScanner.isRecording())
        {
          lcd.print("Rec");
          lcd.setCursor(0, 1);
          lcd.print("FC:");
        }
        else
        {
          lcd.print("Play");
        }
        drawLCD = false;
        drawLCD_stopped = true;
      }

      // Print a message to the LCD (continuously)
      if (FilmScanner.isRecording())
      {
        lcd.setCursor(4, 1);
        lcd.print(String(saved_frames_count));
      }
      else
      {
        // Print a message to the LCD (continuously)
        lcd.setCursor(0, 1);
        lcd.print("Speed:");
        lcd.setCursor(6, 1);
        lcd.print(String(FilmScanner.getPulseDelay()));
      }
      break;

    case 5:  // calibration
      if (drawLCD == true)
      {
        // Print a message to the LCD (only once)
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Calibr-Play");
        drawLCD = false;
        drawLCD_stopped = true;
      }
      break;

    case 6:
      if (drawLCD_stopped == true)
      {
        // Print a message to the LCD (only once)
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Stopped");
        //lcd.setCursor(0, 1);
        //lcd.print("Speed:");
        drawLCD = true;
        drawLCD_stopped = false;
      }
      // Print a message to the LCD (continuously)
      //lcd.setCursor(6, 1);
      //lcd.print("....");
      //lcd.setCursor(6, 1);
      //lcd.print(String(pause_between_frames));
      break;

    case 7:
      if (drawLCD == true)
      {
        // Print a message to the LCD.
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ERROR");
        drawLCD = false;
        drawLCD_stopped = true;
      }
      break;

    default:
      if (drawLCD == true)
      {
        // Print a message to the LCD.
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("UNKNOWN ERROR");
        drawLCD = false;
        drawLCD_stopped = true;
      }
      break;
  }
}
