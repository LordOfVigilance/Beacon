#pragma once

#include "glm/glm.hpp"
#include "Model.h"
#include "Sound.h"


class Player
{
public:
	Player();
	Player(glm::vec3 positionIn, glm::vec2 rotationIn, Model modelIn, Sound* soundsIn);
	~Player();

	glm::vec3 getPosition();
	glm::vec3 getDirection();
	glm::vec2 getRotation();

	void timeUpdate(float timeIn);
	void rotatePlayer(float radians);
	void scalePlayerUp();
	void scalePlayerDown();
	void translate(glm::vec3 translation, bool forward);
	Model getModel();
	float getSpeed();
	float getScale();
	

	void setSpeed(float speedIn);
	void resetSpeed(float speedIn);

private:
	const float PI = 3.14159265f;
	glm::vec3 position;
	glm::vec2 rotation;
	float scale;
	float speed;
	float speedMultiplier;
	float time;
	Model playerModel;
	glm::vec3 direction;

	const float minSize = 2.0f;
	const float maxSize = 3.0f;

	Sound* sounds;


	float radToDeg(float radian);
};

