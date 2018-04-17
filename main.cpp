//
// Created by Anadi Jaggia on 17/04/2018.
//

//#include "sfml_sound.cpp"
#include "mandel_julia.cpp"
#include <SFML/Audio.hpp>
#include <thread>
#include <future>
#include <chrono>
#include <iostream>
#include <complex>

double pi = M_PI;

void transform1D(const short * samples, float coeff[], float * coeff_max, int window_size, int sampleNumber) {
    std::complex<float> sum(0,0);
    float theta, real, imag;
    for (int n = 0; n < window_size; ++n) {
        //std::cout << n << std::endl;
        for (int k = sampleNumber - window_size; k < sampleNumber; ++k) {
            theta = 2 * pi * n * k;
            real = std::cos(theta);
            imag = std::sin(theta);
            sum = sum + std::complex<float>(real * samples[k], imag * samples[k]);
        }
        coeff[n] = std::abs(sum);
        if (*coeff_max < coeff[n]) {
            *coeff_max = coeff[n];
        }
        //std::cout << std::abs(sum) << std::endl;
        sum = std::complex<float>(0, 0);
    }
}

void update_dft(sf::Sound * sound, const short * samples, float coeff_arr[],float * coeff_max, int window_size, unsigned sampleCount) {
    int sampleNumber = 0;
    while (sampleNumber < sampleCount - window_size) {
        //coeff_arr = dft(samples);
        sf::Time t = sound->getPlayingOffset();
        sampleNumber = (int) (t.asMilliseconds() * 44.1);
        *coeff_max = -999;
        transform1D(samples, coeff_arr, coeff_max, window_size, sampleNumber);

        //std::cout << coeff_arr[714] << std::endl;
        //std::cout << sample_number << std::endl;
        //get_sample_numberfloat * coeff_max,
        //run DFT from sample_number - window to sample_number
        //feed DFT to render
    }
}

int main(int argc, char *argv[]) {
    //Load in wave file to buffer
    sf::SoundBuffer buffer; // 16-bit signed ints
    if (!buffer.loadFromFile("../flume_insane.wav"))
        return -1;

    sf::Sound sound;
    sound.setBuffer(buffer); // Specify the buffer to load samples to play

    const short* samples = buffer.getSamples();
    // start playback


    // advance to 2 seconds
    //sound.setPlayingOffset(sf::seconds(10));
    unsigned sampleCount = buffer.getSampleCount();
    unsigned sampleRate  = buffer.getSampleRate();

    sound.play();
    std::cout << "sampleCount = " << sampleCount << std::endl;
    std::cout << "sampleRate  = " << sampleRate << " samples/second" << std::endl;

    float coeff_arr[1000];
    /*for(int i = 0; i < window_size; i++) {
        coeff_arr[i] = std::complex<float>((float)i/window_size, 0);
    }*/

    int x;
    float coeff_max;
    //std::future<int> val = std::async(&render, &coeff_arr, window_size);
    std::thread t(update_dft, &sound, samples, coeff_arr, &coeff_max, 1000, sampleCount);
    //int x = val.get();
    //t.join();

    render(coeff_arr, &coeff_max, 1000);
    t.join();
}