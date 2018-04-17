//
// Created by Anadi Jaggia on 17/04/2018.
//


#include "mandel_julia.cpp"
//#include "sfml_sound.cpp"
#include <SFML/Audio.hpp>
#include <thread>
#include <chrono>
#include <iostream>

int main(int argc, char *argv[]) {
    //Load in wave file to buffer
    sf::SoundBuffer buffer; // 16-bit signed ints
    if (!buffer.loadFromFile("../flume_insane.wav"))
        return -1;

    sf::Sound sound;
    sound.setBuffer(buffer); // Specify the buffer to load samples to play
    sound.play();

    const short* samples = buffer.getSamples();

    // start playback
    sound.play();

    // advance to 2 seconds
    //sound.setPlayingOffset(sf::seconds(10));
    unsigned sampleCount = buffer.getSampleCount();
    unsigned sampleRate  = buffer.getSampleRate();

    std::cout << "sampleCount = " << sampleCount << std::endl;
    std::cout << "sampleRate  = " << sampleRate << " samples/second" << std::endl;

    render();
}