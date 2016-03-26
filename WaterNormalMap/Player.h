#pragma once

#include "glm/glm.hpp"
#include "Model.h"


class Player
{
public:
	Player();
	Player(glm::vec3 positionIn, glm::vec2 rotationIn, Model modelIn);
	~Player();

	glm::vec3 getPosition();
	glm::vec3 getDirection();
	glm::vec2 getRotation();

	void rotatePlayer(float radians);
	void scalePlayerUp();
	void scalePlayerDown();
	void translate(glm::vec3 translation);
	Model getModel();
	float getSpeed();

	void setSpeed(float speedIn);

private:
	const float PI = 3.14159265f;
	glm::vec3 position;
	glm::vec2 rotation;
	float scale;
	float speed;
	Model playerModel;
	glm::vec3 direction;

	const float minSize = 1.0f;
	const float maxSize = 2.0f;


	float radToDeg(float radian);
};

