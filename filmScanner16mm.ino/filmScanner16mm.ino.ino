/*   .___________________________________________________
    |   _     _     _     _     _     _     _     _     _|
    |  |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_ 
    | .                                                  |
    |          This is KuvA-Scanner-Code v.1.0 .         |
    |                                                    |
    |      description: code for 16mm telecine machine . | 
    |                                                    |
    |                  date: 18.9.2020                   |
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

const int PUL1=2; // STEP 1
const int DIR1=3; // DIRECTION 1
const int ENA1=4; // ENABLE 1

// B) Ottava Stepperi 2

const int PUL2=5; // STEP 2
const int DIR2=6; // DIRECTION 2
const int ENA2=7; // ENABLE 2

// C) Gate/spro Stepper 3

const int PUL3=8; // ENABLE 3
const int ENA3=9; // STEP 3
const int DIR3=10; // ENABLE 3

int kierrokset=0;

void setup() {
  
  Serial.begin(9600);
  
  pinMode (PUL1, OUTPUT);
  pinMode (DIR1, OUTPUT);
  pinMode (ENA1, OUTPUT);
  
  pinMode (PUL2, OUTPUT);
  pinMode (DIR2, OUTPUT);
  pinMode (ENA2, OUTPUT);
  
  pinMode (PUL3, OUTPUT);
  pinMode (DIR3, OUTPUT);
  pinMode (ENA3, OUTPUT);
  

}


void loop() {
  
    
    digitalWrite(DIR1,HIGH);
    digitalWrite(ENA1,HIGH);

  for (int i=0; i<300; i++)    //Forward 5000 steps
  {
   
    digitalWrite(PUL1,HIGH);
    delayMicroseconds(50);
    digitalWrite(PUL1,LOW);
    delayMicroseconds(50); 

  }
   digitalWrite(ENA1,LOW);
   kierrokset++;
  Serial.println("ota raw kuva!");
  Serial.print("Frameja on :");
  Serial.println(kierrokset);
   delay(1000);

  
  

}
