/*   .___________________________________________________
    |   _     _     _     _     _     _     _     _     _|
    |  |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_ 
    | .                                                  |
    |          This is KuvA-Scanner-Code v.1.0 .         |
    |                                                    |
    |      description: code for 16mm telecine machine . | 
    |                                                    |
    |                  date: 1.04.2021                   |
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
 #include "FilmScanner.h"
 #include <LiquidCrystal_I2C.h> // external library
 
LiquidCrystal_I2C lcd(0x27,20,21);  // NOTE! Before use, run ic2_scanner sketch and get the IC2 address, 0x27 for example

// other global variables
unsigned long saved_frames_count=0;

int pause_between_frames= 100;
int prev_mode = 0;
bool drawLCD = true;
bool drawLCD_stopped = true;
bool mode_changed = false;

FilmScanner FilmScanner(0); // set default mode: 0=stop
StepperMotor m1, m2, m_gate;

void setup() {
  
  Serial.begin(9600);

 // FilmScanner.enableDebugMode(); // comment out if not needed
  
  FilmScanner.setEncoderPins(18,19,52); // a, b, switch
  FilmScanner.setCameraRemoteControlPin(12);
  FilmScanner.setControlPanelButtonPins(38,40,42,44,46,48,50); // Input pins in following order: multi_jog,stop,playback,play,rec,rw,ffw,reel-to-reel
  FilmScanner.setGateSensorToPin(30);
  FilmScanner.setSwingArmSensorsToPin(32,34); // upper, lower
  // LED OUTPUT Pin 36
  
//  attachInterrupt(digitalPinToInterrupt(18), updateEncoder, CHANGE); // TODO: add interrupt
//  attachInterrupt(digitalPinToInterrupt(19), updateEncoder, CHANGE); // TODO: add interrupt


  FilmScanner.setupMotor(m1,2,3,4);
  FilmScanner.setupMotor(m2,5,6,7);
  FilmScanner.setupMotor(m_gate,8,9,10);

  // Initialize the LCD
  lcd.init();
  // Print messsage mode=0
  printLCD(0);

}


void loop() 
{  
  switch (FilmScanner.getMode()) // tarkistetaan miten filmiä siirretään
  {
    case 1: // REEL-TO-REEL: STOP
      if(mode_changed)
      {
      FilmScanner.unlockMotor(m1);
      FilmScanner.unlockMotor(m2);
      FilmScanner.unlockMotor(m_gate);
      }
      // scanner is stopped, waiting user to push ffw / rw button
      break;
      
    case 2: // REEL-TO-REEL: FFW
      if(mode_changed)
      {
        FilmScanner.setMotorDirectionForward(m1,m2,m_gate);
        FilmScanner.unlockMotor(m1);
        FilmScanner.unlockMotor(m_gate);
        FilmScanner.lockMotor(m2);
      }
      FilmScanner.rewinding(m2);  
      break;    

    case 3: // REEL-TO-REEL: RW      
      if(mode_changed)
      {
        FilmScanner.setMotorDirectionBackward(m1,m2,m_gate);
        FilmScanner.unlockMotor(m2);
        FilmScanner.unlockMotor(m_gate);
        FilmScanner.lockMotor(m1);
      }
      FilmScanner.rewinding(m1);      
      break;      

    case 4: // PLAY AND RECORD
      if(mode_changed)
      {
      FilmScanner.setMotorDirectionForward(m1,m2,m_gate);
      FilmScanner.unlockMotor(m1);
      FilmScanner.lockMotor(m2);
      FilmScanner.lockMotor(m_gate);
      }
      FilmScanner.moveOneFrame(m2,m_gate);

      // RECORDING
      if(FilmScanner.isRecording() == true)
      {
        FilmScanner.captureFrame();
        saved_frames_count=saved_frames_count + 1;
      }
      delay(pause_between_frames);
      break;
      
    case 5: // PLAY BACKWARDS
      if(mode_changed)
      {
        FilmScanner.setMotorDirectionBackward(m1,m2,m_gate);
        FilmScanner.unlockMotor(m2);
        FilmScanner.lockMotor(m1);
        FilmScanner.lockMotor(m_gate);
      }
      FilmScanner.moveOneFrame(m1,m_gate);
      delay(pause_between_frames);
      break;
      
    case 6:// STOPPED
      if(mode_changed)
      {
        FilmScanner.unlockMotor(m1);
        FilmScanner.lockMotor(m2);
        FilmScanner.unlockMotor(m_gate);
      }
      // wait user to push: play / play_b / rec button
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
  //FilmScanner.debugControlPanel(); // print inputs from control panel (only for debugging)

  if(FilmScanner.getMode() != prev_mode)
  {
    mode_changed = true;
    prev_mode = FilmScanner.getMode();
  }
  else
  {
    mode_changed = false;
  }

  FilmScanner.readEncoder(); // >> TODO: move to interrupt function
  pause_between_frames = constrain((100+(FilmScanner.getEncoderCounter()*100.0)), 100,5000);
  
}

// TODO: use this function with interrupt
//void updateEncoder()
//{
//    FilmScanner.readEncoder();
//    pause_between_frames = constrain((100+(FilmScanner.getEncoderCounter()*10.0)), 100,5000);
//}


void printLCD (int mode)
{
  switch (mode)
  {
    case 0:
      // Print a message to the LCD.
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print("Film scanner 0.1");
      lcd.setCursor(0,1);
      lcd.print("Starting...");
      delay(1000);
      break;
      
    case 1:
      if(drawLCD == true)
      {
        // Print a message to the LCD.
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Reel-To-Reel:");
        drawLCD=false;
      }   
      drawLCD_stopped=true;
      break;
      
    case 2:
      if(drawLCD == true)
      {
        // Print a message to the LCD.
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Reel-To-Reel:FFW");
        drawLCD=false;
      }   
      drawLCD_stopped=true;
      break;
      
    case 3:
      if(drawLCD == true)
      {
        // Print a message to the LCD.
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Reel-To-Reel:RW");
        drawLCD=false;
      }   
      drawLCD_stopped=true;
      break;
      
    case 4:
      // Print a message to the LCD (only once)
      if(drawLCD == true)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        if(FilmScanner.isRecording())
        {
          lcd.print("Recording");
          lcd.setCursor(0,1);
          lcd.print("FC:");
        }
        else
        {
          lcd.print("Playing");  
        }
        drawLCD=false;
        drawLCD_stopped=true;
      }
      // Print a message to the LCD (continuously)
      if(FilmScanner.isRecording())
      {
        lcd.setCursor(4,1);
        lcd.print(String(saved_frames_count)); 
      }
      break;
    
    case 5:
     if(drawLCD == true)
      {
        // Print a message to the LCD (only once)
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Backwards");
        drawLCD=false;
        drawLCD_stopped=true;
      }
    break;
    
    case 6:
      if(drawLCD_stopped==true)
      {
        // Print a message to the LCD (only once)
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Stopped");
        lcd.setCursor(0,1);
        lcd.print("Speed:");
        drawLCD = true;
        drawLCD_stopped=false;
      }    
      // Print a message to the LCD (continuously)
      lcd.setCursor(6,1);
      lcd.print("....");
      lcd.setCursor(6,1);
      lcd.print(String(pause_between_frames));
    break;
    
    case 7:
      if(drawLCD == true)
      {
        // Print a message to the LCD.
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ERROR");
        drawLCD = false;
        drawLCD_stopped=true;
      }
    break;
  }
}
