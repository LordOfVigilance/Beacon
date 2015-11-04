#pragma once

#include "DreamClickable.h"

#include <stdio.h>
#include <vector>

class DreamButton : public DreamClickable {
private:
	typedef void (*Method)();
	Method buttonExecution;

public:
	DreamButton(Method);
	DreamButton(float[2], float[2], float[4], Method);
	DreamButton(DreamRenderable, Method);
	~DreamButton(void);

	void mousePress(double, double);
};