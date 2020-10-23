/*   .___________________________________________________
    |   _     _     _     _     _     _     _     _     _|
    |  |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_ 
    | .                                                  |
    |          This is KuvA-Scanner-Code v.1.0 .         |
    |                                                    |
    |      description: code for 16mm telecine machine . | 
    |                                                    |
    |                  date: 23.10.2020                   |
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

// Kytkimet
const int FRAME_DETECTION = 52;


bool backwards = false;
bool ready_to_move = false;

int frame_detected = false;


int kierrokset=0;



void setup() {
  
  Serial.begin(9600);
  
  pinMode (PULSE_1, OUTPUT);
  pinMode (DIRECTION_1, OUTPUT);
  pinMode (ENABLE_1, OUTPUT);
  
  pinMode (PULSE_2, OUTPUT);
  pinMode (DIRECTION_2, OUTPUT);
  pinMode (ENABLE_2, OUTPUT);
  
  pinMode (PULSE_3, OUTPUT);
  pinMode (DIRECTION_3, OUTPUT);
  pinMode (ENABLE_3, OUTPUT);

  pinMode (FRAME_DETECTION, INPUT_PULLUP);
  

}


void loop() {

if(backwards){
    
    digitalWrite(DIRECTION_1,LOW);
    digitalWrite(ENABLE_1,LOW);
    digitalWrite(DIRECTION_2,LOW);
    digitalWrite(ENABLE_2,LOW);
    digitalWrite(DIRECTION_3,LOW);
    digitalWrite(ENABLE_3,LOW);
} 
else
{
    digitalWrite(DIRECTION_1,HIGH);
    digitalWrite(ENABLE_1,LOW);
    digitalWrite(DIRECTION_2,HIGH);
    digitalWrite(ENABLE_2,LOW);
    digitalWrite(DIRECTION_3,HIGH);
    digitalWrite(ENABLE_3,LOW);
}



    
  for (int i=0; i<5000; i++)    //Forward 5000 steps
  {
    
    digitalWrite(PULSE_1,HIGH);
    digitalWrite(PULSE_2,HIGH);
    digitalWrite(PULSE_3,HIGH);

    delayMicroseconds(50);

    digitalWrite(PULSE_1,LOW);
    digitalWrite(PULSE_2,LOW);
    digitalWrite(PULSE_3,LOW);
    
    delayMicroseconds(50);

      // Do not detect switch state until motor has rotated
      if(i>200)
      {
      // read the state of the frame detection switch value:
      frame_detected = digitalRead(FRAME_DETECTION);
  
          // if switch is pressed break out from for loop
          if(frame_detected == LOW)
          {
              Serial.println("frame detected!");
               i= 5000;
          }
      }
      
    }
    
  // laske kierrokset    
  kierrokset++;
  
  Serial.println("ota raw kuva!");
  Serial.print("Frameja on :"+kierrokset);

  // wait between frames
  delay(2000);
  
  }
