#include "Beacon.h"



Beacon::Beacon(): model(Model("Models/Monkey.obj"))
{
	position = glm::vec3();
	rotation = glm::vec2();
	model.translate(position);
	model.rotate(rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
}

Beacon::Beacon(glm::vec3 positionIn, glm::vec2 rotationIn, Model modelIn): model(modelIn)
{
	position = positionIn;
	rotation = rotationIn;
	model.translate(position);
	model.rotate(rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
}


Beacon::~Beacon()
{
}

glm::vec3 Beacon::getPosition()
{
	return position;
}

Model Beacon::getModel()
{
	return model;
}
