#pragma once
#include <vector>
#include "DreamRenderable.h"

class DreamComponent
{
private:
	std::vector<DreamRenderable*> renderables;

public:	
	DreamComponent(void);
	DreamComponent(float[2], float[2], float[4]);
	DreamComponent(DreamRenderable*);
	~DreamComponent(void);
	
	void addRenderable(DreamRenderable*);
	std::vector<DreamRenderable*> getRenderables();
};

