/*   .___________________________________________________
    |   _     _     _     _     _     _     _     _     _|
    |  |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_ 
    | .                                                  |
    |          This is KuvA-Scanner-Code v.1.0 .         |
    |                                                    |
    |      description: code for 16mm telecine machine . | 
    |                                                    |
    |                  date: 11.12.2020                   |
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

int my_delay= 100;
bool drawLCD = true;
bool drawLCD_stopped = true;

bool printToLcd = true;

FilmScanner FilmScanner(0); // modes 0=stopped, 1=rewind, 2=play
StepperMotor m1, m2, m_gate;

void setup() {
  
  Serial.begin(9600);

 // FilmScanner.enableDebugMode(); // comment out if not needed

  FilmScanner.setControlPanelButtonPins(36,38,40,42,44,46,48,50); // Input pins in following order: multi_jog,stop,playback,play,rec,rw,ffw,reel-to-reel
  FilmScanner.setupEncoder(18,19,20);
//  attachInterrupt(digitalPinToInterrupt(18), updateEncoder, CHANGE);
//  attachInterrupt(digitalPinToInterrupt(19), updateEncoder, CHANGE);
  FilmScanner.setGateSensorToPin(52);

  FilmScanner.setupMotor(m1,2,3,4);
  FilmScanner.setupMotor(m2,5,6,7);
  FilmScanner.setupMotor(m_gate,8,9,10);

  // Initialize the LCD
  lcd.init();
  printLCD(0);

}


void loop() 
{  
  switch (FilmScanner.getMode()) // tarkistetaan miten filmiä siirretään
  {
    case 1: // WINDING
      FilmScanner.unlockMotor(m_gate);

      // FFW
      if(FilmScanner.isRewindingForwards() == true){
        FilmScanner.setMotorDirectionForward(m1,m2,m_gate);
        FilmScanner.unlockMotor(m1);
        FilmScanner.lockMotor(m2);
       // no need to run 2 motors: FilmScanner.rewinding(m1,m2);   
        FilmScanner.rewinding(m2);   
      }
      // RW
      else if (FilmScanner.isRewindingBackwards() == true)
      {
        FilmScanner.setMotorDirectionBackward(m1,m2,m_gate);
        FilmScanner.unlockMotor(m2);
        FilmScanner.lockMotor(m1);
       // no need to run 2 motors: FilmScanner.rewinding(m1,m2); 
        FilmScanner.rewinding(m1);      
      }
      // STOP
      else
      {
        FilmScanner.unlockMotor(m1);
        FilmScanner.unlockMotor(m2);
        // scanner is stopped, waiting user to push ffw / rw button
      }
      printLCD(1);

      break;
    case 2: // PLAY AND RECORD

      // PLAY FORWARDS
      if(FilmScanner.isPlayingForwards() == true)
      {
        FilmScanner.setMotorDirectionForward(m1,m2,m_gate);
        FilmScanner.unlockMotor(m1);
        FilmScanner.lockMotor(m2);
        FilmScanner.lockMotor(m_gate);
        FilmScanner.moveOneFrame(m2,m_gate);

        // RECORDING
        if(FilmScanner.isRecording() == true)
        {
          // Take photo!
          // TODO: send signal via lanc: SEND HIGH TO TWO PINS: RELEASE + FOCUS
          // TODO: wait until frame is saved. 
          saved_frames_count=saved_frames_count + 1;
          printLCD(2);
        }
        else
        {
         printLCD(3);
          
        }
        // pause always after single frame is moved
        delay(my_delay);
      }
      // PLAY BACKWARDS
      else if(FilmScanner.isPlayingBackwards() == true)
      {
        FilmScanner.setMotorDirectionBackward(m1,m2,m_gate);
        FilmScanner.unlockMotor(m2);
        FilmScanner.lockMotor(m1);
        FilmScanner.lockMotor(m_gate);
        FilmScanner.moveOneFrame(m1,m_gate);
        printLCD(4);
        // pause always after single frame is moved
        delay(1000);
      }
      // STOPPED
      else
      {
        // wait user to push: play / play_b / rec button
        FilmScanner.unlockMotor(m1);
        FilmScanner.lockMotor(m2);
        FilmScanner.unlockMotor(m_gate);

        printLCD(5);
      }
      break;
    case 3:
        // ERROR IN SCANNING:
        // TODO: solve error with multi jog?
        printLCD(6);

      break;
    default: // STOP
      // stopped, no need to move anywhere
      // Print a message to the LCD ?
      break;
  }

  // read inputs from control panel
  FilmScanner.readControlPanel();
  //FilmScanner.debugControlPanel(); // print inputs from control panel (only for debugging)

  FilmScanner.readEncoder(); // >> TODO: move to interrupt function
  my_delay = constrain((100+(FilmScanner.getEncoderCounter()*100.0)), 100,5000);
  
}


//void updateEncoder()
//{
//    FilmScanner.readEncoder();
//    my_delay = constrain((100+(FilmScanner.getEncoderCounter()*10.0)), 100,5000);
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
        lcd.print("Reel-To-Reel");
        drawLCD=false;
      }   
      drawLCD_stopped=true;
      break;
    case 2:
      if(drawLCD == true)
      {
        // Print a message to the LCD (only once)
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Recording");
        lcd.setCursor(0,1);
        lcd.print("FC:");
        drawLCD=false;
        drawLCD_stopped=true;
      } 
      // Print a message to the LCD (continuously)
      lcd.setCursor(4,1);
      lcd.print(String(saved_frames_count));
    break;
    case 3:
     if(drawLCD == true)
      {
        // Print a message to the LCD (only once)
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Playing");
        drawLCD=false;
        drawLCD_stopped=true;
      }
    break;
    case 4:
      if(drawLCD == true)
      {
        // Print a message to the LCD (only once)
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Play Backwards");
        drawLCD = false;
      }
    break;
    case 5:
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
      //lcd.print("    ");
      lcd.print(String(my_delay));
    break;
    case 6:
      if(drawLCD == true)
      {
        // Print a message to the LCD.
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ERROR");
        drawLCD = false;
      }
    break;
  }
}
