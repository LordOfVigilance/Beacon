#pragma once

#include "DreamRenderable.h"
#include "DreamClickable.h"

#include <stdio.h>
#include <vector>

class DreamSlider : public DreamClickable {
private:
	float* sliderValue;
	float minValue;
	float maxValue;

public:
	DreamSlider(float*, float, float);
	DreamSlider(float*, float[2], float[2], float[4], float, float);
	DreamSlider(float*, DreamRenderable, float, float);
	~DreamSlider(void);
	
	void constructorSliderMinMax(float*, float, float);

	bool pointInAndExecute();
	void mousePress(double, double);
	void mouseDown(double, double);
	void adjustSlider(double, double);


	DreamRenderable* createSliderRenderableFromContainer();
};
