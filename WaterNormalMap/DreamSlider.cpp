#include "DreamSlider.h"

DreamSlider::DreamSlider(float* sliderValue, float min=0.0f, float max=1.0f) : DreamClickable(new DreamRenderable()) {
	constructorSliderMinMax(sliderValue, min, max);
}

DreamSlider::DreamSlider(float* sliderValue, float offset[2], float size[2], float color[4], float min=0.0f, float max=1.0f) :	DreamClickable(new DreamRenderable(offset, size, color)){
	constructorSliderMinMax(sliderValue, min, max);
}

DreamSlider::DreamSlider(float* sliderValue, DreamRenderable renderable, float min=0.0f, float max=1.0f) : DreamClickable(&renderable) {
	constructorSliderMinMax(sliderValue, min, max);
}

void DreamSlider::constructorSliderMinMax(float* sliderValue, float min, float max) {
	this->sliderValue = sliderValue;

	if(*(this->sliderValue) < min) *(this->sliderValue) = min;
	if(max < *(this->sliderValue)) *(this->sliderValue) = max;
	minValue = min;
	maxValue = max;

	DreamRenderable* sliderRenderable = createSliderRenderableFromContainer();
	addRenderable(sliderRenderable);
}

DreamRenderable* DreamSlider::createSliderRenderableFromContainer() {
	DreamRenderable* containerRenderable = getRenderables().at(0);

	float containerY = containerRenderable->getOffset()[1];
	float containerSize = containerRenderable->getSize()[1];
	float offsetY = containerSize*(1 - ((*sliderValue-minValue)/(maxValue-minValue))) + containerY;

	float offset[2] = {containerRenderable->getOffset()[0] - containerRenderable->getSize()[0]*0.2f, offsetY};
	float size[2] = {containerRenderable->getSize()[0]*1.4f, containerRenderable->getSize()[1]/20.0f};
	float* colorPtr = containerRenderable->colorModified();
	float color[4] = {colorPtr[0], colorPtr[1], colorPtr[2], colorPtr[3]};

	return new DreamRenderable(offset, size, color);
}

DreamSlider::~DreamSlider(void) {}

void DreamSlider::mousePress(double pointX, double pointY) {
	adjustSlider(pointX, pointY);
}

void DreamSlider::mouseDown(double pointX, double pointY) {
	adjustSlider(pointX, pointY);
}

void DreamSlider::adjustSlider(double pointX, double pointY) {
	DreamRenderable* containerRenderable = getRenderables().at(0);
	DreamRenderable* sliderRenderable = getRenderables().at(1);

	float minY = containerRenderable->getOffset()[1];
	float maxY = minY + containerRenderable->getSize()[1];
	if(pointY < minY) pointY = minY;
	if(maxY < pointY) pointY = maxY;

	sliderRenderable->getOffset()[1] = pointY;
	*sliderValue = maxValue - (pointY - minY)/(maxY-minY) * (maxValue - minValue);
}

void DreamSlider::print() {
	printf("Slider Value: %f\n", *sliderValue);
}