#include "DreamClickable.h"


DreamClickable::DreamClickable(void) : DreamComponent(new DreamRenderable()) {}

DreamClickable::DreamClickable(float offset[2], float size[2], float color[4]) : DreamComponent(new DreamRenderable(offset, size, color)){}

DreamClickable::DreamClickable(DreamRenderable* renderable) : DreamComponent(renderable){}

DreamClickable::~DreamClickable(void){}

bool DreamClickable::pointInAndExecute(double pointX, double pointY) {
	for(DreamRenderable* renderable : getRenderables()) {
		if(renderable->pointIn(pointX, pointY)) {
			mousePress(pointX, pointY);
			return true;
		}
	}

	return false;
}

bool DreamClickable::checkAndSetHovering(double pointX, double pointY) {
	for(DreamRenderable* renderable : getRenderables()) {
		if(renderable->pointIn(pointX, pointY)) {
			renderable->setHover(true);

			return true;
		} else {
			renderable->setHover(false);
		}
	}

	return false;
}

void DreamClickable::mouseDown(double, double) {}

void DreamClickable::print() {}