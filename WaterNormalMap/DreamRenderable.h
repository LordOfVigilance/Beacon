#pragma once

class DreamRenderable {
private:	
	float* offset;
	float* size;
	float* color;
	bool hover;

public:
	DreamRenderable(void);
	DreamRenderable(float[2], float[2], float[4]);
	~DreamRenderable(void);	

	float* getOffset();
	float* getSize();
	float* getColor();

	bool getHover();
	void setHover(bool);
	bool pointIn(double, double);
	
	float* colorModified();
};

