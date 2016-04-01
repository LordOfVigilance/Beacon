#include "Sound.h"

#include <iostream>
#include <string>

#include <chrono>
#include <thread>

#include "include/irrKlang.h"

using namespace std;
using namespace irrklang;

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll


Sound::Sound()
{
	mEngine = createIrrKlangDevice();
	sEngine = createIrrKlangDevice();

	sync = false;

	current = 0;

	if (!mEngine) {
		cout << "Error: Music Engine could not be created" << endl; // could not start engine
	}

	if (!sEngine) {
		cout << "Error: Sound Engine could not be created" << endl; // could not start engine
	}

	drop = sEngine->addSoundSourceFromFile("media/sounds/Drop.wav");
	bgWater = sEngine->addSoundSourceFromFile("media/sounds/bgWater.wav");
	grow = sEngine->addSoundSourceFromFile("media/sounds/grow.wav");

	beaconSources.push_back(sEngine->addSoundSourceFromFile("media/sounds/beacon1.mp3"));

	sEngine->play2D(bgWater, true);
	

	loadSounds(0);
}


Sound::~Sound()
{
	mEngine->drop(); // delete engine
	sEngine->drop(); // delete engine
}


int Sound::loadSounds(int x)
{

	// Stop and clear all current sounds
	mEngine->stopAllSounds();
	mEngine->removeAllSoundSources();
	sources.erase(sources.begin(), sources.end());
	music.erase(music.begin(), music.end());

	current = 0;

	string type = "";
	int min = 0;
	int max = 0;

	// Choose which set of sounds to load
	if (x == 0) {
		type = ".mp3";
		min = 0;
		max = 19;
		sync = false;
	}
	else if (x == 1) {
		type = ".wav";
		min = 20;
		max = 31;
		sync = true;
	}
	else if (x == 2) {
		type = ".wav";
		min = 32;
		max = 47;
		sync = true;
	}

	cout << "Loading sound set " << x << endl;

	// Load the sound set
	for (int i = min; i < max+1; i++) {
		string str = "media/music/" + to_string(i) + type;

		char * cstr = new char[str.length() + 1];
		std::strcpy(cstr, str.c_str());
		const ik_c8 * filename = cstr;

		if (sync) {
			ISound* snd = mEngine->play2D(filename, true, true, true); // play sound, start paused
			if (snd) snd->setVolume(0.0f); // set volume to 0
			music.push_back(snd);
		}
		else {
			sources.push_back(mEngine->addSoundSourceFromFile(filename));
		}

	}

	return max - min + 1;
}


void Sound::play()
{

	if (sync && current < music.size()) {
		cout << "Playing sound " << current << endl;
		sEngine->play2D(grow);
		music[current]->setVolume(1.0f);
		mEngine->setAllSoundsPaused(false);
		current++;
	}
	else if (!sync && current < sources.size()) {
		cout << "Playing sound " << current << endl;
		sEngine->play2D(grow);
		music.push_back(mEngine->play2D(sources[current], true, false, true)); // play sound
		current++;
	}
	else cout << "No sound to play" << endl;

}


void Sound::addBeacon(int num, float posx, float posy, float posz, float dist) // Set beacon positions.  dist is the distance from the beacon where the sound does not get louder
{
	beacons.push_back(sEngine->play3D(beaconSources[num], vec3df(posx, posy, posz), false, true, true));
	beacons[num]->setMinDistance(dist);
}


void Sound::playBeacon(int num) // Play beacon
{
	beacons[num]->setIsPaused(false);
}


void Sound::stopBeacon(int num) // Stop beacon
{
	beacons[num]->setIsPaused(true);
	beacons[num]->setPlayPosition(0.0f);
}


void Sound::updatePosition(float posx, float posy, float posz, float lookx, float looky, float lookz) {
	sEngine->setListenerPosition(vec3df(posx, posy, posz), vec3df(lookx, looky, lookz));
}


void Sound::stop()
{

	if (sync && current > 0) {
		current--;
		cout << "Stopping sound " << current << endl;
		sEngine->play2D(grow);
		sEngine->play2D(drop);
		this_thread::sleep_for(std::chrono::milliseconds(100));
		music[current]->setVolume(0.0f);
	}
	else if (!sync && current > 0) {
		current--;
		cout << "Stopping sound " << current << endl;
		sEngine->play2D(grow);
		sEngine->play2D(drop);
		this_thread::sleep_for(std::chrono::milliseconds(100));
		music[current]->stop();
		music.pop_back();
	}
	else cout << "No sound to stop" << endl;

}


void Sound::stopAll()
{
	current = 0;
	cout << "Stopping all sounds" << endl;
	sEngine->play2D(grow);
	sEngine->play2D(drop);
	this_thread::sleep_for(std::chrono::milliseconds(100));

	if (sync) {
		mEngine->setAllSoundsPaused(true);

		for (int i = 0; i < music.size(); i++) {
			music[i]->setVolume(0.0f);
			music[i]->setPlayPosition(0);
		}
	}
	else {
		mEngine->stopAllSounds();
		music.erase(music.begin(), music.end());
	}

}


void Sound::setMaster(ik_f32 x) // 0.0f is silent, 1.0f is full volume
{
	mEngine->setSoundVolume(x);
	sEngine->setSoundVolume(x);
}


void Sound::pause(bool x) // x==true will pause, x==false will unpause
{
	mEngine->setAllSoundsPaused(x);
}


void Sound::mute(bool x, float volume) // x==true will mute, x==false will unmute
{
	if (x) {
		mEngine->setSoundVolume(0.0f);
		sEngine->setSoundVolume(0.0f);
	}
	else {
		mEngine->setSoundVolume(volume);
		sEngine->setSoundVolume(volume);
	}
}