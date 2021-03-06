#include "packet.h"
#include "def.h"
#include <Stepper.h>
#define STEPS 200   //number of steps the stepper motor needs to complete a 360 turn. Used by the Stepper.h header

//Definir motores de passo
Stepper stepper_RIGHT(STEPS, 11, 9, 8, 10);
Stepper stepper_LEFT(STEPS, 7, 5, 4, 6);

// Pin definitions for HC-SR04
#define echoPin 12              // the SRF05's echo pin
#define initPin 13              // the SRF05's init pin

#define SPEED 20

unsigned char *outBuffer = (unsigned char *) malloc(9*sizeof(outBuffer));
unsigned char *inBuffer = (unsigned char *) malloc(6*sizeof(inBuffer));
int Step_size = 200;
int Distance;
unsigned char Obstacle = 0;
unsigned char currentDirection = CMD_MOVE_FORWARD;
unsigned char inByte;
unsigned char rpiDirection;
int NSteps;

void setup()
{
    unsigned char Wakeup = 0;

    Serial.begin(57600);
    
    // Configure motor speed 
    stepper_RIGHT.setSpeed(SPEED);
    stepper_LEFT.setSpeed(SPEED);

    // Configure ultrasound pins
    pinMode(initPin, OUTPUT);
    pinMode(echoPin, INPUT);

    while(Wakeup != CMD_WAKEUP){
        Wakeup = Serial.read();
        delay(50);
    }

    Serial.write(0xFF);
}

void loop()
{
    int len = 0;
    // Check whether there's something in front of us
    readUltrasonic(); // read and store the measured distances
	    
    // Move forward checking for obstacles
    for (NSteps=1; NSteps<=Step_size; NSteps++){

        if(Distance > 10 || Distance < 0)
	    // Moves one step forward
	    moveForward();

        // there's an object in front of it
        else{
            Obstacle = 1;
            break;
        }
    }

    // Send data and wait for commands
    // Sending data
    // Distance info
    *(outBuffer) = TAG_SENSOR_DATA;
    *(outBuffer+1) = 3;
    *(outBuffer+2) = SENSOR_0;
    *(outBuffer+3) = (NSteps & 0xFF00) >> 8; //NSteps;
    *(outBuffer+4) = NSteps & 0x00FF; //TODO Previuosly it was unsigned short, now it is int

    // Obstacle info
    *(outBuffer+5) = TAG_SENSOR_DATA;
    *(outBuffer+6) = 2;
    *(outBuffer+7) = SENSOR_1;
    *(outBuffer+8) = Obstacle == 1 ? '0' : '1'; //Obstacle;

    Serial.write(outBuffer,9);
    delay(100);

    // Waiting for instructions
    while (!Serial.available()){
        // Do nothing if no data is sent
    }

    // Getting instructions
    while(Serial.available() > 0){
    	//Serial.println("Waiting for instructions");
        inByte = Serial.read();
        *(inBuffer + len) = inByte;
        len++;
        delay(200); // Arduino was looping to fast, available needs more time
    }
    //Serial.println("Instructions received");
    
    rpiDirection = *(inBuffer+2);// TODO just a test
    Step_size = *(inBuffer+5);

    // Turn to desired direction
    if(rpiDirection != currentDirection){//TODO isso tá zuado porra
        directionDecision();    
        Obstacle = 0;
    }
    // Keep going if previous direction is the same as new direction
    else{
        // TODO this conditional seems to be useless 
	Obstacle = 0;
    }
}

void readUltrasonic()
{
    int PulseTime;

    digitalWrite(initPin, HIGH);
    delayMicroseconds(10); // must keep the trig pin high for at least 10us
    digitalWrite(initPin, LOW);

    PulseTime = pulseIn(echoPin, HIGH);
    Distance = PulseTime/58.2; // PaceOfSound = 1/SpeedOfSound = 29.1 us/cm
}

// 210 seems to be the correct value for 90 degrees (TODO keep cheking)
void turnRight()
{
    for(int i=210; i>0; i--){
        stepper_RIGHT.step(-1);
        stepper_LEFT.step(1);
    }
}

void turnLeft()
{
    for(int i=210; i>0; i--){
	stepper_RIGHT.step(1);
        stepper_LEFT.step(-1);
    }
}

void turnBack()
{
    for(int i=2*210; i>0; i--){
        stepper_RIGHT.step(-1);
        stepper_LEFT.step(1);
    }
}

void moveForward()
{
    stepper_RIGHT.step(1);
    stepper_LEFT.step(1);
}
void directionDecision()
{
    switch (currentDirection){
	case CMD_MOVE_FORWARD:
    	switch(rpiDirection){
    	    case CMD_MOVE_BACKWARD: 
		turnBack();
		//Serial.println("Turned back");
	    break;
	    case CMD_TURN_RIGHT: 
		turnRight();
		//Serial.println("Turned Right");
	    break;
	    case CMD_TURN_LEFT: 
		turnLeft();
		//Serial.println("Turned left");
	    break;		
	}
	break;	
	case CMD_MOVE_BACKWARD:
	switch(rpiDirection){
	    case CMD_MOVE_FORWARD: 
		turnBack();
		//Serial.println("Turned back");
	    break;
	    case CMD_TURN_LEFT: 
		turnRight();
		//Serial.println("Turned Right");
	    break;
	    case CMD_TURN_RIGHT: 
		turnLeft();
		//Serial.println("Turned left");
	    break;
	}
	break;
	case CMD_TURN_RIGHT:
	switch(rpiDirection){
	    case CMD_TURN_LEFT: 
		turnBack();
		//Serial.println("Turned back");
	    break;
	    case CMD_MOVE_BACKWARD: 
		turnRight();
		//Serial.println("Turned Right");
	    break;
	    case CMD_MOVE_FORWARD: 
		turnLeft();
		//Serial.println("Turned left");
	    break;
	}
	break;
	case CMD_TURN_LEFT:	
	switch(rpiDirection){
	    case CMD_TURN_RIGHT: 
		turnBack();
		//Serial.println("Turned back");
	    break;
	    case CMD_MOVE_FORWARD: 
		turnRight();
		//Serial.println("Turned Right");
	    break;
	    case CMD_MOVE_BACKWARD: 
		turnLeft();
		//Serial.println("Turned left");
            break;
	}
	break;
    }
	currentDirection=rpiDirection;
}
