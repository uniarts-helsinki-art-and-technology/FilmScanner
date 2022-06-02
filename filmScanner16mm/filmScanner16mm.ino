/*   .___________________________________________________
    |   _     _     _     _     _     _     _     _     _|
    |  |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_
    | .                                                  |
    |          This is KuvA-Scanner-Code v.1.2b          |
    |                                                    |
    |      description: code for 16mm telecine machine . |
    |                                                    |
    |                  date: 11.02.2022                   |
    |                                                    |
    ^----------------------------------------------------^


                       NOTES
Joakim's attempt to make the frame stop more accurate by
driving the film srocket "over the threshold"
and take the feedback when falling. smart?





*/

#include "FilmScanner.h"
#include <Encoder.h>
#include <LiquidCrystal_I2C.h> // external library


// FILM SCANNER
FilmScanner FilmScanner(0); // set default mode: 0=stop
StepperMotor m1, m2, m_gate;
unsigned long saved_frames_count = 0;
const int pause_between_frames = 1000;
int prev_mode = 0;
bool mode_changed = false;

// ENCODER
Encoder rotary_encoder(18, 19); // set pins to interrupts
float rotary_encoder_speed = 10.0; // Multiplier for frame delay
long encoder_ref  = -999; // encoder reference value

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 21); // NOTE! Before use, run ic2_scanner sketch and get the IC2 address, 0x27 for example
bool drawLCD = true;
bool drawLCD_stopped = true;

void setup() {

  // Motors
  FilmScanner.setupMotor(m1, 2, 3, 4);
  FilmScanner.setupMotor(m2, 5, 6, 7);
  FilmScanner.setupMotor(m_gate, 8, 9, 10);

  // Control panel
  FilmScanner.setControlPanelButtonPins(38, 40, 42, 44, 46, 48, 50);
  // Camera control
  FilmScanner.setCameraRemoteControlPin(11);

  // Switches and sensors
  FilmScanner.setGateSensorToPin(32);
  // TODO: FilmScanner.setSwingArmSensorsToPin(33?, 34?); // Input pins in following order: upper, lower

  // Initialize the LCD
  lcd.init();
  // Print start messsage (mode=0)
  printLCD(0);
  //FilmScanner.enableDebugMode();

}


void loop()
{
  switch (FilmScanner.getMode()) // Valitaan toiminto
  {
    case 1: // REEL-TO-REEL: STOP
      if (mode_changed)
      {
        FilmScanner.unlockMotor(m1);
        FilmScanner.unlockMotor(m2);
        FilmScanner.unlockMotor(m_gate);
      }
      // scanner is stopped, waiting user to push ffw / rw button
      break;

    case 2: // REEL-TO-REEL: FFW
      if (mode_changed)
      {
        FilmScanner.setMotorDirectionForward(m1, m2, m_gate);
        FilmScanner.unlockMotor(m1);
        FilmScanner.unlockMotor(m_gate);
        FilmScanner.lockMotor(m2);
      }
      FilmScanner.rewinding(m2);
      break;

    case 3: // REEL-TO-REEL: RW
      if (mode_changed)
      {
        FilmScanner.setMotorDirectionBackward(m1, m2, m_gate);
        FilmScanner.unlockMotor(m2);
        FilmScanner.unlockMotor(m_gate);
        FilmScanner.lockMotor(m1);
      }
      FilmScanner.rewinding(m1);
      break;

    case 4: // PLAY AND RECORD
      if (mode_changed)
      {
        FilmScanner.setMotorDirectionForward(m1, m2, m_gate);
        FilmScanner.unlockMotor(m1);
        FilmScanner.lockMotor(m2);
        FilmScanner.lockMotor(m_gate);
        FilmScanner.moveOneFrameCalibration(m1, m_gate); //For Calibration!!
      }
      else
      {
        FilmScanner.moveOneFrame(m2, m_gate);
      }
      // RECORDING
      if (FilmScanner.isRecording() == true)
      {
        FilmScanner.captureFrame();
        saved_frames_count = saved_frames_count + 1;
      }
      // joakimin lisäys ---------------------------------------------------------- poista

      //FilmScanner.unlockMotor(m1);
      //FilmScanner.unlockMotor(m2);
      //FilmScanner.unlockMotor(m_gate);
      // joakimin lisäys ---------------------------------------------------------- poist


      delay(pause_between_frames);

//      // READ ENCODER VALUE I took these away J
//      long encoder_current = rotary_encoder.read();
//      if (encoder_current != encoder_ref) {
//        encoder_ref = encoder_current;
//        FilmScanner.setPulseDelay(int(encoder_current));
//      }
      break;

    case 5: // CALIBRATION PLAY
      if (mode_changed)
      {
        FilmScanner.setMotorDirectionForward(m1, m2, m_gate);
        FilmScanner.unlockMotor(m1);
        FilmScanner.lockMotor(m2);
        FilmScanner.lockMotor(m_gate);
      }
       FilmScanner.moveOneFrameCalibration(m1, m_gate); //For Calibration!!
       delay(pause_between_frames);
      //FilmScanner.setMode(byte(6)); //Change mode after one calibration move to stop!!!! J
      //mode_changed=true;

      break;

    case 6:// STOPPED
      if (mode_changed)
      {
        FilmScanner.unlockMotor(m1);
        FilmScanner.lockMotor(m2);
        FilmScanner.unlockMotor(m_gate);
      }
      // do nothing, wait user to push: play / play_b / rec button
      break;

    case 7: // ERROR:
      // TODO: solve errors with multi jog?
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
      lcd.print("KuvaScan 1.2b");
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
          lcd.print("Blind-Rec");
          lcd.setCursor(0, 1);
          lcd.print("FC:");
        }
        else
        {
          lcd.print("Blind-Play");
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

    case 5:       //calibration play !! new thing
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
