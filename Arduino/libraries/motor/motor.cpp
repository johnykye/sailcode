#include "motor.h"

Motor::Motor(Stream* serialLine, int pin, char controllerID) {
	this->serialCom = serialLine;
	this->anglePin = pin;
	this->controllerID = controllerID;

	//Exit safe-start mode.
	this->serialCom->write(0xAA);
	this->serialCom->write(this->controllerID);
	this->serialCom->write(0x03);
	this->currentSpeed = 0;
}

void Motor::setMotorParams(int min, int max) { 
	this->zeroAngle = min;
	this->maxAngle = max;
}

double Motor::getAngle() {
	int val = analogRead(this->anglePin);
	//Serial.print("Val from angle sensor: ");
	//Serial.println(val);
	//1023=5V, 0=0V. Need to map 0.5V to -180, and 4.5V to 180
	//204.6 points per Volt
	//90 degrees per Volt.
	//So we have 204.6/90 points per degree.
	//or 90/204.6 degrees per point.
	//This means that at 2.5 volts, we are getting 225 degrees. 
	//We want that to be 0
	val = int((double(val)*0.4399) - 225);
	return val;
}

void Motor::reset(){
	this->serialCom->write(0xAA);
	this->serialCom->write(this->controllerID);
	this->serialCom->write(0x03);
	this->currentSpeed = 0;
}

int Motor::setLength(int position) {
	/* Adjust the motor to have the specified rope length out.
	For example, setLength(0) would bring the rope in all the way,
	setLength(100) would take it all the way out.
	Need to determine max and min angles, but we should be able to make this work.
	There is a linear relationship between the angle and the position of the knot on the line.
	Also need to include a timeout in case the motor isn't responsive.
	*/

	//Cap out possible values.
	if(position>100) {
		position=100;
	} else if(position<0){
		position=0;
	}

	int speed, sign;
	//First calculate desired angle from position.
	double dif = this->maxAngle - this->zeroAngle;
	double endAngle = this->zeroAngle + (position/100.0 * dif);

	double currAngle = this->getAngle();
	dif=abs(endAngle - currAngle);

	//Need to include a timeout.
	unsigned long st = millis();
	while(dif > this->ANGLE_ERROR) {

		//Need to translate the difference into an appropriate motor speed.
		sign = (endAngle > currAngle) ? 1:-1;
		if(dif > 90) {
			speed = sign*3600; //Full speed
		} else if(dif > 40) {
			speed = sign*1800;//Half speed
		} else if(dif >15) {
			speed = sign*900;//Quarter speed
		} else {
			speed = sign*500;
		}
		this->setMotorSpeed(speed);
		currAngle = this->getAngle();
		dif = abs(endAngle - currAngle); 

		if(currAngle > this->maxAngle || currAngle < this->zeroAngle) {
			break;
		}

		if(abs(millis()-st) > 2000) {
			break;
		}
	}

	this->setMotorSpeed(0);

	//Returns the actual position of the motor
	currAngle = this->getAngle();
	position = int(100*(currAngle-this->zeroAngle)/(this->maxAngle - this->zeroAngle));
	return position;
}

int Motor::setMotorSpeed(int speed) {

	int currAngle = this->getAngle();
	if (currAngle >= maxAngle || currAngle <= zeroAngle)
		return 0;
	
	if(speed!=this->currentSpeed) {
		this->serialCom->write(0xAA);
		this->serialCom->write(this->controllerID);

		if (speed < 0)
	  	{
	    	this->serialCom->write(0x86);  // motor reverse command
	    	speed = -speed;  // make speed positive
	  	}
	  	else
	  	{
	    	this->serialCom->write(0x85);  // motor forward command
	  	}
		this->serialCom->write(speed & 0x1F);
		this->serialCom->write(speed >> 5);
		this->currentSpeed=speed;
	}
	return 0;
}

