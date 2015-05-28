#include "packet.h"
#include "def.h"
#include <Stepper.h>
#define STEPS 200   //number of steps the stepper motor needs to complete a 360 turn. Used by the Stepper.h header
#define r 5 // Tire radius [cm]

//Definir motores de passo
Stepper stepper_RIGHT(STEPS, 11, 9, 8, 10);
Stepper stepper_LEFT(STEPS, 7, 5, 4, 6);
//#define speedMotorB 11

// Pin definitions for HC-SR04
#define echoPin 8              // the SRF05's echo pin
#define initPin 9              // the SRF05's init pin

// Define states
#define FORWARD STATE_MOVING_FORWARD
#define CHECK STATE_WAITING_CMD

int PulseTime;
int Step_size = 50;
int Distance;
unsigned char Step_Distance;
long Cmd;
int Big_step=0;
int Degree=0;
int state = FORWARD;
unsigned char Wakeup = 0;
unsigned char Obstacle = 0;
unsigned char *outBuffer = (unsigned char *) malloc(8*sizeof(outBuffer));
unsigned char *inBuffer = (unsigned char *) malloc(6*sizeof(inBuffer));
unsigned char rpiDirection;

void setup()
{
    Serial.begin(57600); 
    pinMode(13, OUTPUT);
    
    // Configure motor pins
    stepper_RIGHT.setSpeed(10);
    stepper_LEFT.setSpeed(10);
 
    // Configure ultrasound pins
    pinMode(initPin, OUTPUT);
    pinMode(echoPin, INPUT);

/*    while(Serial.available() == 0) {
        digitalWrite(13, HIGH);
        delay(50);
        digitalWrite(13, LOW);
        delay(50);
    }*/

    while(Wakeup != CMD_WAKEUP){
        digitalWrite(13, HIGH);
        Wakeup = Serial.read();
        delay(50);
    }

    Serial.write(0xFF);
    digitalWrite(13, LOW);
}
  
void loop()
{
        
    //Serial.write(0x99);
    readUltrasonic(); // read and store the measured distances
    stateMachine();

}

void stateMachine()
{
    int len = 0;
    unsigned char inByte;

    if(state == FORWARD){ // no obstacles detected
        // if there's something in front of us
        for (Big_step=0; Big_step<Step_size; Big_step++){
            if(Distance > 15 || Distance < 0){
                moveForward();
            }
            // there's an object in front of it
            else{
                // Make the motor stop by leaving the loop.
                Step_Distance = 2*3.1415*r*Big_step;
                Obstacle = 1;
                break;
            }
        }
        //After completing the desired motor rotation, go to the next state
        state=CHECK;
    }
    
    else if(state == CHECK){ // obstacle detected, send data and wait for commands
        // Sending data
        // Distance info
        *(outBuffer) = TAG_SENSOR_DATA;
        *(outBuffer+1) = 2;
        *(outBuffer+2) = SENSOR_0;
        *(outBuffer+3) = 1; //Step_Distance;

        // Obstacle info
        *(outBuffer+4) = TAG_SENSOR_DATA;
        *(outBuffer+5) = 2;
        *(outBuffer+6) = SENSOR_1;
        *(outBuffer+7) = 1; //Obstacle;

        Serial.write(outBuffer,8);
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

        rpiDirection = *(inBuffer+2);
        Step_size = *(inBuffer+5);

        if (rpiDirection == CMD_TURN_RIGHT){ // Considering 200 makes 90 degrees
            for(Degree=200; Degree>0; Degree--)
                turnRight();
        }
        else if (rpiDirection == CMD_TURN_LEFT){
            for(Degree=200; Degree>0; Degree--)
                turnLeft();
        }
        else if (rpiDirection == CMD_MOVE_BACKWARD){
            for(Degree=2*200; Degree>0; Degree--)
                turnLeft();
        }
    
        state = FORWARD;
        Obstacle = 0;
    }
}

/*packet *rx ()
{
    int len = 0;
    unsigned char inByte;
    unsigned char *buffer = (unsigned char *) malloc (6*sizeof(buffer));
    packet *pkg = NULL;

    while (!Serial.available()){
        // Do nothing if no data is sent
    }
    while(Serial.available() > 0){
        inByte = Serial.read();
        *(buffer + len) = inByte;
        len++;
        delay(200); // Arduino was looping to fast, available needs more time
    }

    if (len != 3){
        //
    }
    else{
        // receive 6, 
        pkg =  packet_create(*(buffer), *(buffer+1), buffer+2);
        free(buffer);

        if(pkg->tag == TAG_CMD)
            Serial.println ("Processing...");
        else
            Serial.println ("Processing error");

        return pkg;
    }
}*/

void readUltrasonic()
{
    digitalWrite(initPin, HIGH);
    delayMicroseconds(10); // must keep the trig pin high for at least 10us
    digitalWrite(initPin, LOW);
    
    PulseTime = pulseIn(echoPin, HIGH);
    Distance = PulseTime/58.2; // PaceOfSound = 1/SpeedOfSound = 29.1 us/cm
}

void turnRight()
{
    stepper_RIGHT.step(-1);
    stepper_LEFT.step(1);
}

void turnLeft()
{
    stepper_RIGHT.step(1);
    stepper_LEFT.step(-1);
}

void moveForward()
{
    stepper_RIGHT.step(1);
    stepper_LEFT.step(1);
}

void moveBackward()
{
    stepper_RIGHT.step(-1);
    stepper_LEFT.step(-1);
}
