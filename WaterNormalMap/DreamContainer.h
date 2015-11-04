#pragma once
#include <vector>
#include "DreamComponent.h"

#include "DreamButton.h"
#include "DreamSlider.h"

class DreamContainer : public DreamComponent
{
typedef void (*Method)();

private:
	std::vector<DreamComponent*> components;

public:
	DreamContainer::DreamContainer(void);
	DreamContainer::DreamContainer(float offset[2], float size[2], float color[4]);
	DreamContainer::DreamContainer(DreamRenderable* renderable);
	DreamContainer::~DreamContainer(void);

	void addComponent(DreamComponent*);
	void addComponent(Method);
	void addComponent(float*, float, float);
	void addComponent(float*, int, float, float);

	std::vector<DreamComponent*> getComponents();
};

