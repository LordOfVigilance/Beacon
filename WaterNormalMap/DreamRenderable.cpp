#include "DreamRenderable.h"


DreamRenderable::DreamRenderable(void) {	
	offset = new float[2];
	size = new float[2];
	color = new float[4];

	offset[0] = 0.0f;
	offset[1] = 0.0f;

	size[0] = 100.0f;
	size[1] = 100.0f;

	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 1.0f;

	hover = false;
}

DreamRenderable::DreamRenderable(float offset[2], float size[2], float color[4]) {
	this->offset = new float[2];
	this->size = new float[2];
	this->color = new float[4];

	this->offset[0] = offset[0];
	this->offset[1] = offset[1];

	this->size[0] = size[0];
	this->size[1] = size[1];

	this->color[0] = color[0];
	this->color[1] = color[1];
	this->color[2] = color[2];
	this->color[3] = color[3];
	this->hover = false;
}

DreamRenderable::~DreamRenderable(void) {
	delete [] offset;
	delete [] size;
	delete [] color;
}

float* DreamRenderable::getOffset() {
	return offset;
}

float* DreamRenderable::getSize() {
	return size;
}

float* DreamRenderable::getColor() {
	if(hover)
		return colorModified();

	return color;
}

float* DreamRenderable::colorModified() {
	float newColor[4] = {color[0], color[1], color[2], color[3]};
	
	float luminance = color[0]+color[1]+color[2];

	if(luminance < 1.5f) {
		newColor[0] += 0.3f;
		newColor[1] += 0.3f;
		newColor[2] += 0.3f;
		return newColor;
	}
	
	newColor[0] -= 0.3f;
	newColor[1] -= 0.3f;
	newColor[2] -= 0.3f;
	return newColor;
}

bool DreamRenderable::getHover() {
	return hover;
}

void DreamRenderable::setHover(bool hover) {
	this->hover = hover;
}

bool DreamRenderable::pointIn(double pointX, double pointY) {
	if(offset[0] < pointX && pointX < offset[0] + size[0])
		if(offset[1] < pointY && pointY < offset[1] + size[1])
			return true;

	return false;
}