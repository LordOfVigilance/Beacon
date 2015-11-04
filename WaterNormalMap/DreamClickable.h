#pragma once

#include <stdio.h>
#include <vector>
#include "DreamComponent.h"

class DreamClickable : public DreamComponent
{
public:
	DreamClickable(void);
	DreamClickable(float[2], float[2], float[4]);
	DreamClickable(DreamRenderable*);
	~DreamClickable(void);

	bool pointInAndExecute(double, double);
	bool checkAndSetHovering(double, double);
	virtual void mousePress(double, double) = 0;
	virtual void mouseDown(double, double);
	virtual void print();
};