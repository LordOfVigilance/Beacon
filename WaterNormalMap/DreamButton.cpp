#include "DreamButton.h"

DreamButton::DreamButton(method buttonExecution) : DreamClickable(new DreamRenderable()) {
	this->buttonExecution = buttonExecution;
}

DreamButton::DreamButton(float offset[2], float size[2], float color[4], method buttonExecution)
	: DreamClickable(new DreamRenderable(offset, size, color)) {
	this->buttonExecution = buttonExecution;
}

DreamButton::DreamButton(DreamRenderable renderable, method buttonExecution)
	: DreamClickable(&renderable){
	this->buttonExecution = buttonExecution;
}

DreamButton::~DreamButton(void){}

void DreamButton::mousePress(double, double) {
	buttonExecution();
}