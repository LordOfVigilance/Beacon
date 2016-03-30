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
	vector<ISoundSource*> beaconSources;
	vector<ISound*> music;
	vector<ISound*> beacons;
	ISoundSource* drop;
	ISoundSource* bgWater;
	ISoundSource* grow;
	int current;
	bool sync;

public:
	Sound();
	~Sound();

	int loadSounds(int x);

	void play();

	void addBeacon(int num, float posx, float posy, float posz, float dist);

	void playBeacon(int num);

	void stopBeacon(int num);

	void updatePosition(float posx, float posy, float posz, float lookx, float looky, float lookz);

	void stop();

	void stopAll();

	void setMaster(ik_f32 x);

	void pause(bool x);

	void mute(bool x);
};

