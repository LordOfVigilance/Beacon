#include "DreamContainer.h"

DreamContainer::DreamContainer(void) : DreamComponent(new DreamRenderable()) {}

DreamContainer::DreamContainer(float offset[2], float size[2], float color[4]) : DreamComponent(new DreamRenderable(offset, size, color)){}

DreamContainer::DreamContainer(DreamRenderable* renderable) : DreamComponent(renderable){}

DreamContainer::~DreamContainer(void){}

void DreamContainer::addComponent(DreamComponent* component) {
	components.push_back(component);
}

void DreamContainer::addComponent(Method method) {
	DreamRenderable* container = getRenderables()[0];

	float offset[2] = {0.05f*components.size(), container->getOffset()[1]};
	float size[2] = {0.025f, container->getSize()[1]};
	float* colorPtr = container->colorModified();
	float color[4] = {colorPtr[0], colorPtr[1], colorPtr[2], colorPtr[3]};
	DreamButton* button = new DreamButton(offset, size, color, method);
	components.push_back(button);
}

void DreamContainer::addComponent(float* sliderValue, float minValue, float maxValue) {
	DreamRenderable* container = getRenderables()[0];

	float offset[2] = {0.05f*components.size(), container->getOffset()[1]};
	float size[2] = {0.025f, container->getSize()[1]};
	float* colorPtr = container->colorModified();
	float color[4] = {colorPtr[0], colorPtr[1], colorPtr[2], colorPtr[3]};
	DreamSlider* slider = new DreamSlider(sliderValue, offset, size, color, minValue, maxValue);
	components.push_back(slider);
}

void DreamContainer::addComponent(float* sliderArray, int arraySize, float minValue, float maxValue) {
	DreamRenderable* container = getRenderables()[0];

	float offset[2] = {0.05f*components.size(), container->getOffset()[1]};
	float size[2] = {0.025f, container->getSize()[1]};
	float* colorPtr = container->colorModified();
	float color[4] = {colorPtr[0], colorPtr[1], colorPtr[2], colorPtr[3]};

	for(int i=0; i<arraySize; i++) {
		offset[0] = 0.05f*components.size();
		DreamSlider* slider = new DreamSlider(sliderArray+i, offset, size, color, minValue, maxValue);
		components.push_back(slider);
	}
}

void DreamContainer::addComponent(float* sliderArray, int arraySize, float minValue, float maxValue, float color[4]) {
	DreamRenderable* container = getRenderables()[0];

	float offset[2] = {0.05f*components.size(), container->getOffset()[1]};
	float size[2] = {0.025f, container->getSize()[1]};

	for(int i=0; i<arraySize; i++) {
		offset[0] = 0.05f*components.size();
		DreamSlider* slider = new DreamSlider(sliderArray+i, offset, size, color, minValue, maxValue);
		components.push_back(slider);
	}
}

std::vector<DreamComponent*> DreamContainer::getComponents() {
	return components;
}