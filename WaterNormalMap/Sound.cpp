#include "Sound.h"

#include <iostream>
#include <string>
#include "include/irrKlang.h"

using namespace std;
using namespace irrklang;

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

//int sound()
//{
//	ISoundEngine* engine = createIrrKlangDevice();
//	if (!engine) return 1; // could not start engine
//
//	engine->play2D("media/ophelia.mp3", true); // play some mp3 file, looped
//
//	cin.get(); // wait until user presses a key
//
//	engine->drop(); // delete engine
//	return 0;
//}


Sound::Sound()
{
	mEngine = createIrrKlangDevice();
	sEngine = createIrrKlangDevice();

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

	sEngine->play2D(bgWater, true);
}


Sound::~Sound()
{
	mEngine->drop(); // delete engine
	sEngine->drop(); // delete engine
}


void Sound::loadSounds(int x)
{
	string group = "";
	int min = 0;
	int max = 0;

	// Choose which set of sounds to load
	if (x == 0) {
		group = "media/music/";
		min = 0;
		max = 20;
	}

	// Load the sound set
	for (int i = min; i < max; i++) {
		string str = group + to_string(i) + ".mp3";

		char * cstr = new char[str.length() + 1];
		std::strcpy(cstr, str.c_str());
		const ik_c8 * filename = cstr;

		sources.push_back(mEngine->addSoundSourceFromFile(filename));
	}

}


void Sound::play()
{
	if (current < sources.size()) {
		cout << "Playing sound " << current << endl;
		sEngine->play2D(grow);
		music.push_back(mEngine->play2D(sources[current], true, false, true)); // play sound
		current++;
	}
	else cout << "No sound to play" << endl;
}


void Sound::stop()
{
	if (current > 0) {
		current--;
		cout << "Stopping sound " << current << endl;
		sEngine->play2D(grow);
		sEngine->play2D(drop);
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
	mEngine->stopAllSounds();
	music.erase(music.begin(), music.end());
}