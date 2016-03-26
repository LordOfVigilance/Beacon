#include "Player.h"


// Remove this when done. Here:
#include <iostream>
//To Here


Player::Player() : playerModel("Models/Monkey.obj")
{
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	rotation = glm::vec2(0.0f, 3.14f);
	scale = 1.0f;
	direction = glm::vec3(-cos(0.0f) * sin(rotation.x), sin(0.0f), -cos(0.0f) * cos(rotation.x));
	playerModel.scale(glm::vec3(scale, scale, scale));
	playerModel.translate(position);
	playerModel.rotate(rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
}

Player::Player(glm::vec3 positionIn, glm::vec2 rotationIn, Model modelIn) : playerModel(modelIn)
{
	position = positionIn;
	rotation = rotationIn;
	scale = 1.0f;
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

void Player::rotatePlayer(float radians)
{
	rotation.x += radians;
	float degrees = (radians * 180) / PI;
	playerModel.rotate(degrees, glm::vec3(0.0f, 1.0f, 0.0f));
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
	scale += 0.2f;
	if (scale > maxSize)
	{
		scale = maxSize;
	}
	playerModel.scale(glm::vec3(scale, scale, scale));
}

void Player::scalePlayerDown()
{
	scale -= 0.2f;
	if (scale < minSize)
	{
		scale = minSize;
	}
	playerModel.scale(glm::vec3(scale, scale, scale));
}

void Player::translate(glm::vec3 translation)
{
	position += translation;
	playerModel.translate(translation);
}

Model Player::getModel()
{
	return playerModel;
}
