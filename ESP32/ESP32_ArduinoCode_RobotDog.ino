//Initialize sender

// Using hardware serial -- 
//we can try out of box serial 2 also on default RX2 and TX2 pins on other side
HardwareSerial receiver(2); 
int count = 0;
unsigned int receivedValue = 0;

int LED = 13;
int myTX = 32;  // not used -- will send later 
int myRX = 33; // connect TX of VC 02 here -- this is input pin

// Inititalize Motor Code
#include "BluetoothSerial.h"
// Initialize Bluetooth
BluetoothSerial SerialBT;

//Initialize Servo
#include <MobaTools.h>  // Include the Servo library
// Initialize servo objects
MoToServo panServo;
MoToServo tiltServo;

// Define servo pins
#define PSrv 4  // Pan servo connected to GPIO 4
#define TSrv 2  // Tilt servo connected to GPIO 2
int step = 2;

// Define servo positions
const int scenter = 90;    // Center position for pan
const int sleft = 45;      // Left position for pan
const int sright = 135;    // Right position for pan

//motor driver pins
#define IN1 25  // Left motor forward
#define IN2 26  // Left motor backward
#define IN3 27  // Right motor forward
#define IN4 14  // Right motor backward

void setup() {
  //Serial.begin(Baud Rate, Data Protocol, TX pin, RX pin);
receiver.begin(9600, SERIAL_8N1, myRX, myTX);

// Start serial communication for Bluetooth
  Serial.begin(115200);
  SerialBT.begin("Hacker1"); // Bluetooth name

  // Set all motor control pins to output mode
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

// Attach the both servo
  panServo.attach(PSrv);
  panServo.write(scenter);  // Attach & Set pan servo to center position

  tiltServo.attach(TSrv);
  tiltServo.write(scenter);  // Attach & Set tilt servo to center position

  // Run servo demo for pan and tilt servos
  servoDemo();  // Test pan servo
  servoDemo(); // 2 times

  panServo.write(scenter);
  tiltServo.write(scenter);

  // Start with motors off
  stopMotors();
  motorDemo();    // Run Demo once
  Serial.println("Bluetooth is ready to pair. Waiting for commands...");
  pinMode(LED,OUTPUT);

}

void loop() {
  panServo.write(90);
  tiltServo.write(90);

 if (SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n');
    command.trim(); // Remove any extra whitespace or newline characters
    Serial.println("Command received: " + command);
    
    if (command == "100") {
      forward();
    } else if (command == "200") {
      backward();
    } else if (command == "400") {
      turnLeft();
    } else if (command == "300") {
      turnRight();
    } else {
      Serial.println("Unknown command.");
    }
  }

	if(receiver.available()){
		Serial.println("inside while");
    Serial.print("Recvr : "); // debug only
    Serial.println(count); // debug only

    //read incoming bytes
    byte receivedValue = receiver.read();
    // Print the received value in Hex Format
    Serial.print("rcvd Hex is : 0x");
    Serial.println(receivedValue,HEX);

    if(receivedValue == 0x1C){
      Serial.println("Got Light ---- ON ");
      digitalWrite(LED,HIGH);
    } else if (receivedValue == 0x2C){
      Serial.println("Got Light ==== OFF ");
      digitalWrite(LED,LOW);

    //Voice Forward  
    } else if (receivedValue == 0x3C){
      Serial.println("Got Go Forward ");
      forward();
    //Voice Backward
    } else if (receivedValue == 0x4C){
      Serial.println("Got Go back ");
      backward();
    //Voice Turn Right
      } else if (receivedValue == 0x5C){
      Serial.println("Got Turn Right ");
      turnRight();
    //Voice Turn Left
      } else if (receivedValue == 0x6C){
      Serial.println("Got Turn Left ");
      turnLeft();
    //Voice Stop
      } else if (receivedValue == 0x7C){
      Serial.println("Got a Stop ");
      stopMotors();
      
    } else {
      Serial.println("recevd val not known ");
    }
  }
}

// Function to move forward
void forward() {
  Serial.println("Moving forward...");
  backingTilt();
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(2000);
  stopMotors();
}

// Function to move backward
void backward() {
  Serial.println("Moving backward...");
  backingTilt();
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(2000);
  stopMotors();
}

// Function to turn left
void turnLeft() {
  Serial.println("Turning left...");
  panServo.write(135);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(600);
  stopMotors();
  delay(600);
  panServo.write(90);
}

// Function to turn right
void turnRight() {
  Serial.println("Turning right...");
  panServo.write(45);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(600);
  stopMotors();
  delay(600);
  panServo.write(90);
}

// Function to stop all motors
void stopMotors() {
  Serial.println("Stopping motors...");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Function to run MotorDemo
void motorDemo() {
  Serial.println("Running Motor Demo...");
  forward();
  forward();
  backward();
  backward();
  turnRight();
  turnRight();
  turnRight();
  turnLeft();
  turnLeft();
  turnLeft();
}

// Servo Demo -- Moves both the servos from 0 to 1 -- one by one
void servoDemo() {
  int pos=0;
  for (pos = 30; pos <= 150; pos += step) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		panServo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
  delay(1000);
  pos=0;
	for (pos = 150; pos >= 30; pos -= step) { // goes from 180 degrees to 0 degrees
		panServo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
  
  pos=0;
  for (pos = 50; pos <= 120; pos += step) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		tiltServo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
	for (pos = 120; pos >= 50; pos -= step) { // goes from 180 degrees to 0 degrees
		tiltServo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
}

void backingTilt(){
  int pos=0;
  for (pos = 50; pos <= 120; pos += step) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		tiltServo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
	for (pos = 120; pos >= 50; pos -= step) { // goes from 180 degrees to 0 degrees
		tiltServo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
}
