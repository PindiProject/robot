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

// Control for cleaning system
#define control 2
#define PWRLED 3

#define SPEED 20

unsigned char *outBuffer = (unsigned char *) malloc(9*sizeof(outBuffer));
unsigned char *inBuffer = (unsigned char *) malloc(6*sizeof(inBuffer));
int Step_size = 200;
long Distance;
int Obstacle = 0;
unsigned char currentDirection = CMD_MOVE_FORWARD;
unsigned char inByte;
unsigned char rpiDirection;
int NSteps;
// Variaveis para monitorar bateria
long previousMillis = 0;
long interval = 1000*60;

void setup()
{
    unsigned char Wakeup = 0;

    Serial.begin(57600);
    
    // Configure motor speed 
    stepper_RIGHT.setSpeed(SPEED);
    stepper_LEFT.setSpeed(SPEED);

    // Configura control pin
    pinMode(control, OUTPUT);
    

    // Configure ultrasound pins
    pinMode(initPin, OUTPUT);
    pinMode(echoPin, INPUT);

    while(Wakeup != CMD_WAKEUP){
        Wakeup = Serial.read();
        delay(50);
    }

    Serial.write(0xFF);
    // Ativa succao
    digitalWrite(control, HIGH);
}

void loop()
{
    int voltage;

    // Parte da locomoção
    locomotion ();

    // Monitoramento da bateria
    unsigned long currentMillis = millis();
 
    if(currentMillis - previousMillis > interval) {
	// save the last time it was initiated 
	previousMillis = currentMillis;  

	//voltage read 
	voltage = analogRead(A2);
	if (voltage < 3.5){
	    digitalWrite(control, LOW);
	    digitalWrite(PWRLED, HIGH);
	    // Volta pra base
	    while (1);
	}
    }
}

void locomotion ()
{
    int len = 0;
    // Check whether there's something in front of us
    readUltrasonic(); // read and store the measured distances
	    
    // Move forward checking for obstacles
    for (NSteps=1; NSteps<=Step_size; NSteps++){
	    // Moves one step forward
	    moveForward();
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
        inByte = Serial.read();
        *(inBuffer + len) = inByte;
        len++;
        delay(200); // Arduino was looping to fast, available needs more time
    }
    
    rpiDirection = *(inBuffer+2);// TODO just a test
    Step_size = *(inBuffer+5) & 0xFF;

    // Turn to desired direction
    if(rpiDirection != currentDirection){//TODO isso tá zuado porra
        directionDecision();    
    }

    Obstacle = 0;
}

void readUltrasonic()
{
    long PulseTime;
    long aux[10];
    long temp = 0;
    int i;
    long rpiDistance;

    for (i=0;i<10;i++){
        digitalWrite(initPin, LOW);
	delayMicroseconds (2);
	digitalWrite(initPin, HIGH);
        delayMicroseconds(10); // must keep the trig pin high for at least 10us
        digitalWrite(initPin, LOW);

        PulseTime = pulseIn(echoPin, HIGH);
        aux[i] = PulseTime/58.2; // PaceOfSound = 1/SpeedOfSound = 29.1 us/cm
    }

    for (i=0;i<10;i++){
	temp = temp + aux[i];
    }
    Distance = temp/10;

    rpiDistance = 28.27*Step_size/200; // 2piR = 28.27cm - 200passos

    // Caso haja algum obstaculo antes do que se pretende percorrer
    if (Distance < rpiDistance){
	Step_size = (int) Distance*200/28.27;
	Step_size = Step_size - 100;
	Obstacle = 1;
    }
    // Caso nada seja encontrado, Step_size permanece o mesmo
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
