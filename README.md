<img src="/screenshot_mac.png?raw=true" alt="Mac screenshot" align="right">

This is an in-progress port of Glypha III from its Mac OS/QuickDraw version (1995) to cross platform and OpenGL.

> Glypha III is a classic arcade game originally created by John Calhoun in which you are placed inside an Egyptian temple with only your lance and a winged steed to aid you. Here you are forced to do battle with Sphinxes in order to gain the honour of a place on the High Scores list. The game is based on Joust and features sound and the original animated graphics.

### Build

([CMake](https://cmake.org) 2.8+ is required)

macOS:

1. `make`

Windows:

1. `mkdir build`
2. `cd build`
3. `cmake -DCMAKE_BUILD_TYPE=Release ..`
4. `cmake --build . --config Release`

Linux:

1. `make qt`

### TODO

- Implement high score list
- Implement sounds for Qt/Linux version
- About box

### Links

- Originally developed by John Calhoun:  
<http://www.softdorothy.com>  
<https://github.com/softdorothy/glypha_III>

- Mac OS X port by Mark Pazolli:  
<http://sourceforge.net/projects/vitality/files/glypha-iii>

- Glypha - Then and Now  
<http://boredzo.org/glypha>
