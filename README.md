# Fractal Music Vizualizer
An OpenGL music visualizer using the FFT and the Mandelbrot set fractal. Thank you to [Bensound](https://www.bensound.com/) for royalty-free music.

![MusicViz Example](examples/moose.gif)

## Installation Instructions

These installation instructions work for Linux. MacOS works similarly with `brew`, but some of the OpenGL libraries only have 32-bit OSX libraries and hence are not supported by the newest OSX.

First, clone the repository using `git clone https://github.com/alexsaadfalcon/cpp_music_viz`. Then, you simply run `cd cpp_music_viz; ./install.sh`. This will make a build folder, install the necessary packages, and run cmake/make.

## Usage Instructions

The executable will be placed in the `build/` directory. You can run it (after installation) with `cd build; ./musicViz`

You can easily change the song being played by editing `config_file.txt`. You can either switch the song to one of the other two included songs, or add your own `.wav` file and put the file name in `config_file.txt`.

