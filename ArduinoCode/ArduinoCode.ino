#include <SoftwareSerial.h>
#include <Servo.h>
//SoftwareSerial btm(2,3); // rx tx

// Initialize all the servos
Servo baseServo;
Servo lowServo1;
Servo lowServo2;
Servo midServo;
//Servo upperServo // Uncomment this once the grasper at the end of the arm is added.

// Set starting positions for the servos
int basePos = 90;
int low1Pos = 90;
int low2Pos = 180 - low1Pos;
int midPos = 90;
// int upperPos = 90;
int x_pos = 9;            // X position of arm tip in centimeters (used in moveTo()) (approximate)
int y_pos = 9;            // Y position of arm tip in centimeters (used in moveTo()) (approximate)
int base_mov = 90;        // Position of base in degrees (used in moveTo()) (approximate)

char inputAsChars[10];    // A buffer to hold the data from the bluetooth module
int input;                // Holds the received data after it's been converted to an int
boolean command = false;  // Senses if a command has been sent
int armLength = 9;        // arms = ~9cm? (should re-measure for more accurate movements)
int baseAngle;            // The angle of the base servo
int midAngle;             // The angle of the middle servo
int theta;                // Used in the trig calculations for the servo angles
double r;

void setup() {
  // Connects to serial (blueooth) and attaches servos
   Serial.begin(9600);
   pinMode(LED_BUILTIN, OUTPUT);
   baseServo.attach(2);
   lowServo1.attach(3);
   lowServo2.attach(4);
   midServo.attach(5);
   //upperServo.attach(6); // Add once grasping arm is added
   delay(1000);
}


void loop() {
   // Waits to recieve input in the serial port
   if(Serial.available()){
     int i = 0;
     // Reads each thing that is sent.
     while(Serial.available() > 0){
          inputAsChars[i] = Serial.read();
          i++;
          delay(3);
     }
     input = atoi(inputAsChars);
     Serial.println(input);
     // The serial has sent something. So do stuff other than just wait for more commands
     command = true;
   }

   // If a command has been recieved go do it
   if(command){
     processCommand(input);
     command = false;
   }
}

void moveTo(int x, int y, int base) {
  // Find polar coordinate definition for the position
  if (x == 0) {
    theta = 90;
  }
  else {
    theta = atan2(y,x)*(180/PI);
  }
  r = sqrt(x*x+y*y);

  // Convert polar coordinates to servo angles
  midAngle = acos((-r*r + 2*armLength*armLength)/(2*armLength*armLength))*(180/PI);
  baseAngle = theta + ((180-midAngle)/2);
  delay(100);

  // Set limits to angles to prevent going past the limits on the arm's joints.
  if (midAngle < 25) {
    midAngle = 25;
  }
  if (baseAngle < 10) {
    baseAngle = 10;
  }
  if (baseAngle > 170) {
    baseAngle = 170;
  }
  
  // Move servos accordingly
  lowServo1.write(180-baseAngle);
  lowServo2.write(baseAngle);
  midServo.write(midAngle);
  baseServo.write(base);
}

void processCommand(int input) {
  switch (input) {
    case 1:
    if (y_pos < 17) {
      y_pos++;
    }
    break;
    case 2:
    if (x_pos > 0) {
      x_pos--;
    }
    break;
    case 3:
    // zap
    break;
    case 4:
    if (x_pos < 17) {
      x_pos++;
    }
    break;
    case 5:
    if (base_mov > 10) {
      base_mov = base_mov - 10;
    }
    break;
    case 6:
    if (y_pos > 0) {
      y_pos--;
    }
    break;
    case 7:
    if (base_mov < 170) {
      base_mov = base_mov + 10;
    }
    break;
    default:
    break;
  }
  moveTo(x_pos, y_pos, base_mov);
}
