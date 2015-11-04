#include "DreamComponent.h"


DreamComponent::DreamComponent(void){
	renderables.push_back(new DreamRenderable());
}

DreamComponent::DreamComponent(float offset[2], float size[2], float color[4]){
	renderables.push_back(new DreamRenderable(offset, size, color));
}

DreamComponent::DreamComponent(DreamRenderable* renderable){
	renderables.push_back(renderable);
}

DreamComponent::~DreamComponent(void){}

void DreamComponent::addRenderable(DreamRenderable* renderable) {
	renderables.push_back(renderable);
}

std::vector<DreamRenderable*> DreamComponent::getRenderables() {
	return renderables;
}