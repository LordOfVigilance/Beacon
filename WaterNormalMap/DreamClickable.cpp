#include "DreamClickable.h"


DreamClickable::DreamClickable(void){
	renderables.push_back(new DreamRenderable());
}

DreamClickable::DreamClickable(float offset[2], float size[2], float color[4]){
	renderables.push_back(new DreamRenderable(offset, size, color));
}

DreamClickable::DreamClickable(DreamRenderable* renderable){
	renderables.push_back(renderable);
}

DreamClickable::~DreamClickable(void){}

void DreamClickable::addRenderable(DreamRenderable* renderable) {
	renderables.push_back(renderable);
}

bool DreamClickable::pointInAndExecute(double pointX, double pointY) {
	for(DreamRenderable* renderable : renderables) {
		if(renderable->pointIn(pointX, pointY)) {
			mousePress(pointX, pointY);
			return true;
		}
	}

	return false;
}

bool DreamClickable::checkAndSetHovering(double pointX, double pointY) {
	for(DreamRenderable* renderable : renderables) {
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

std::vector<DreamRenderable*> DreamClickable::getRenderables() {
	return renderables;
}