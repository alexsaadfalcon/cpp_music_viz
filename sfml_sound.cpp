#ifndef sfml
#define sfml

#include <SFML/Audio.hpp>
#include <thread>
#include <chrono>
#include <iostream>

int play_music()
{

	while(1) {
	}
	

	// pause playback
	sound.pause();

	// resume playback
	sound.play();

	// stop playback and rewind
	sound.stop();          


    return 0;
}

#endif
