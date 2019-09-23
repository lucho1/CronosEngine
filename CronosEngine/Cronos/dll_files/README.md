# Crazy Taxi by Doc Donkeys
This is a university project made by CITM (https://www.citm.upc.edu/ing/) Centre de la Imatge i la Tecnologia Multimedia students of the TTC (Talent Tech Center) in Barcelona, in which we make a physics-based Crazy Taxi tribute in C & C++. We primarly used the Bullet (https://github.com/bulletphysics/bullet3), OPEN GL and SDL (https://www.libsdl.org/) libraries.

Crazy Taxi (our version) is a physics-based racing game where you have to transport passangers to randomly located points in a randomly generated city full of obstacles.
To win, reach all the flags marked by the cylinder above you, which will guide you to them. Each time you drop a passenger in a flag, he/she will thank you, and 30 extra seconds
will be rewarded to you.

When you reach the last flag destination to leave the last passenger, the flag will become blue and a disctinctive sound will sound.
If you don't reach all destinations in time you will lose.

The cylinder above your car is your best ally, it will show you the way to the next flag. It also changes color depending on how close you are to the destination.
If you are far from the flag, the cylinder will turn green, as you get closer to the flag it will turn red, this will help you know which end
of the cylinder you must follow.

When you reach a flag you must STOP completely right next to the flag, you need top be very close to drop the passenger and get directions for the next flag!

Feel free to visit our Webpage: https://docdonkeys.github.io/Crazy-Taxi/

## Installation
No previous installations is required to play the game, just download and execute the .exe file *Crazy Taxi* from releases.

## Usage
### General Controls
* ESC: Exit Game
* R: Restart Game

### Player Controls
* Move: Up/Down/Left/Right
* Handbrake: Spacebar
* E: Flip vehicle

### Debug Controls
* F1 3D debug (rigidbodies, sensors, elements axis etc.)
* F2 Toggle In-Game camera with Free View Camera (to look freely around the map)
* F3 Add 30 seconds to the time left.

## Free View Camera controls
* W A S D To move around in x-z plane
* F To move upwards in the Y axis
* C To move downwards in the Y axis
* SHIFT to move faster

* Right click to look around

## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-newfeature`
3. Commit your changes: `git commit -am 'Add some
feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request and we might look into it. :D

## The Team: Doc Donkeys (https://github.com/DocDonkeys)
* Carles Homs 
  * Role: Code
  * Github: ch0m5 (https://github.com/ch0m5)

* Dídac Romero
  * Role: Code
  * Github: DídacRomero (https://github.com/DidacRomero)

* Repository: https://github.com/DocDonkeys/Crazy-Taxi

## Disclosure
We do not own any of the audio used for the game. Authors credited below.

* Music:
	* Yellow Line - James Shimoji: https://www.youtube.com/watch?v=Ok2369YY2P8

* Sfx:
	* Royalty Free Sfx

## License & Copyright 

MIT License

Copyright (c) 2018 DocDonkeys

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.