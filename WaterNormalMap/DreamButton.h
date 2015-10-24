#pragma once

#include "DreamClickable.h"

#include <stdio.h>
#include <vector>

class DreamButton : public DreamClickable {
private:
	typedef void (*method)();
	method buttonExecution;

public:
	DreamButton(method);
	DreamButton(float[2], float[2], float[4], method);
	DreamButton(DreamRenderable, method);
	~DreamButton(void);

	void mousePress(double, double);
};