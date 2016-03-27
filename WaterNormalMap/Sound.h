#pragma once

#include "include/irrKlang.h"
#include <vector>

using namespace irrklang;
using namespace std;

class Sound
{

private:
	ISoundEngine* mEngine;
	ISoundEngine* sEngine;
	vector<ISoundSource*> sources;
	vector<ISound*> music;
	ISoundSource* drop;
	ISoundSource* bgWater;
	ISoundSource* grow;
	int current;

public:
	Sound();
	~Sound();

	void loadSounds(int x);

	void play();

	void stop();

	void stopAll();
};

