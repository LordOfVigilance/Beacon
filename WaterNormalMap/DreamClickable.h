#pragma once

#include <stdio.h>
#include <vector>

#include "DreamRenderable.h"

class DreamClickable
{
private:
	std::vector<DreamRenderable*> renderables;

public:
	DreamClickable(void);
	DreamClickable(float[2], float[2], float[4]);
	DreamClickable(DreamRenderable*);
	~DreamClickable(void);

	void addRenderable(DreamRenderable*);

	bool pointInAndExecute(double, double);
	bool checkAndSetHovering(double, double);
	virtual void mousePress(double, double) = 0;
	virtual void mouseDown(double, double);

	std::vector<DreamRenderable*> getRenderables();
};