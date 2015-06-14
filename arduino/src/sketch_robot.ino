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

unsigned char *outBuffer = (unsigned char *) malloc(9*sizeof(outBuffer));
unsigned char *inBuffer = (unsigned char *) malloc(6*sizeof(inBuffer));
int Step_size = 200;
int Distance;
unsigned char Obstacle = 0;
unsigned char currentDirection = CMD_MOVE_FORWARD;

void setup()
{
    unsigned char Wakeup = 0;

    Serial.begin(57600);

    // Configure motor speed 
    stepper_RIGHT.setSpeed(10);
    stepper_LEFT.setSpeed(10);

    // Configure ultrasound pins
    pinMode(initPin, OUTPUT);
    pinMode(echoPin, INPUT);

    //while(Wakeup != CMD_WAKEUP){
    //    Wakeup = Serial.read();
    //    delay(50);
    //}

    Serial.write(0xFF);
}

void loop()
{
    int len = 0;
    unsigned char inByte;
    unsigned char rpiDirection;
    int NSteps;

    // Move forward checking for obstacles
    for (NSteps=1; NSteps<=Step_size; NSteps++){
	// Check whether there's something in front of us
	readUltrasonic(); // read and store the measured distances
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
    *(outBuffer+3) = 0;//(NSteps & 0xFF00) >> 8; //NSteps;
    *(outBuffer+4) = NSteps; //TODO Previuosly it was unsigned short, now it is int

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
        inByte = Serial.read();
        *(inBuffer + len) = inByte;
        len++;
        delay(200); // Arduino was looping to fast, available needs more time
    }

    rpiDirection = *(inBuffer);//*(inBuffer+2); TODO just a test
    //stepsReceived = *(inBuffer+5);
    Step_size = 200; //TODO this is temporary, it should convert the stepsReceived to int

    // Turn to desired direction
    if(rpiDirection != currentDirection){
        Serial.println(rpiDirection);
        if (rpiDirection == 'a')//CMD_TURN_RIGHT) TODO just a test
            turnRight();
        else if (rpiDirection == 'b')//CMD_TURN_LEFT) TODO just a test
            turnLeft();
        else if (rpiDirection == 'c')//CMD_MOVE_BACKWARD) TODO just a test
            turnBack();

        currentDirection = rpiDirection;
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
