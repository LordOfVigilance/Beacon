#include "Player.h"


// Remove this when done. Here:
#include <iostream>
//To Here


Player::Player() : playerModel("Models/Monkey.obj"), sounds(NULL)
{
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	rotation = glm::vec2(0.0f, 3.14f);
	scale = 1.0f;
	speed = 5.0f;
	speedMultiplier = 1.0f;
	time = 5.0f;
	direction = glm::vec3(-cos(0.0f) * sin(rotation.x), sin(0.0f), -cos(0.0f) * cos(rotation.x));
	playerModel.scale(glm::vec3(scale, scale, scale));
	playerModel.translate(position);
	playerModel.rotate(rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
}

Player::Player(glm::vec3 positionIn, glm::vec2 rotationIn, Model modelIn, Sound* soundsIn) : playerModel(modelIn), sounds(soundsIn)
{
	position = positionIn;
	rotation = rotationIn;
	scale = 1.0f;
	speed = 7.0f;
	speedMultiplier = 1.0f;
	time = 5.0f;
	direction = glm::vec3(-cos(0.0f) * sin(rotation.x), sin(0.0f), -cos(0.0f) * cos(rotation.x));
	playerModel.scale(glm::vec3(scale, scale, scale));
	playerModel.translate(position);
	playerModel.rotate(rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
}


Player::~Player()
{
}

glm::vec3 Player::getPosition()
{
	return position;
}

glm::vec3 Player::getDirection()
{
	return direction;
}

glm::vec2 Player::getRotation()
{
	return rotation;
}

void Player::timeUpdate(float timeIn)
{
	time -= timeIn;
	if (time <= 0.0f)
	{
		scalePlayerDown();
		time = 5.0f;
	}
}

void Player::rotatePlayer(float radians)
{
	rotation.x += radians;


	playerModel.rotate(radians, glm::vec3(0.0f, 1.0f, 0.0f));
	if (rotation.x >= 2 * PI)
	{
		rotation.x -= 2 * PI;
	}
	else if (rotation.x <= 0)
	{
		rotation.x += 2 * PI;
	}
	direction = glm::vec3(-cos(0.0f) * sin(rotation.x), sin(0.0f), -(cos(0.0f) * cos(rotation.x)));


}

void Player::scalePlayerUp()
{
	float lastScale = scale;
	scale += 0.05f;
	if (scale > maxSize)
	{
		scale = maxSize;
	}
	else
	{
		time = 5.0f;
		sounds->play();
		speed += 0.5f;
	}
	float scaleDown = 1.0f / lastScale;
	playerModel.scale(glm::vec3(scaleDown, scaleDown, scaleDown));
	playerModel.scale(glm::vec3(scale, scale, scale));
	
}

void Player::scalePlayerDown()
{
	float lastScale = scale;
	scale -= 0.05f;
	if (scale < minSize)
	{
		scale = minSize;
	}
	else
	{
		
		sounds->stop();
		speed -= 0.5f;
	}
	float scaleUp = 1.0f / lastScale;
	playerModel.scale(glm::vec3(scaleUp, scaleUp, scaleUp));
	playerModel.scale(glm::vec3(scale, scale, scale));
}

void Player::translate(glm::vec3 translation)
{
	position -= translation;

	float magnitude = sqrt(pow(translation.x,2) + pow(translation.z, 2));
	float inverseScale = 1.0f / scale;
	playerModel.scale(glm::vec3(inverseScale, inverseScale, inverseScale));
	playerModel.translate((glm::vec3(0.0f,0.0f,1.0f) * magnitude));
	playerModel.scale(glm::vec3(scale, scale, scale));
}

Model Player::getModel()
{
	return playerModel;
}

float Player::getSpeed()
{
	return speed * speedMultiplier;
}


void Player::setSpeed(float speedIn)
{
	speedMultiplier = speedIn;
}

void Player::resetSpeed(float speedIn)
{
	if (speedIn == speedMultiplier)
	{
		speedMultiplier = 1.0f;
	}
}


float Player::radToDeg(float radians)
{
	return (radians / PI) * 180;
}