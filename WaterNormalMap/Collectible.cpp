#include "Collectible.h"



Collectible::Collectible() : model("Models/Monkey.obj"), visible(true), radius(1.0f), position(glm::vec3())
{
}

Collectible::Collectible(glm::vec3 positionIn, float radiusIn, bool visibleIn, Model modelIn) : model(modelIn), visible(visibleIn), radius(radiusIn), position(positionIn)
{
}


Collectible::~Collectible()
{
}

glm::vec3 Collectible::getPosition()
{
	return position;
}

float Collectible::getRadius()
{
	return radius;
}

bool Collectible::getVisible()
{
	return visible;
}

void Collectible::setPostion(glm::vec3 positionIn)
{
	model.translate(positionIn - position);
	position = positionIn;
}

void Collectible::collide()
{
	if (visible)
	{
		visible = false;
		//Call Music Player Here

	}
}
