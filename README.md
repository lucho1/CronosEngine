# CronosEngine
[![License](https://img.shields.io/github/license/lucho1/CronosEngine?color=red&label=License&style=plastic)](https://github.com/lucho1/CronosEngine/blob/master/LICENSE)
[![Version](https://img.shields.io/github/v/tag/lucho1/CronosEngine?color=blue&label=Version&style=plastic)](https://github.com/lucho1/CronosEngine/releases)
![Last Commit](https://img.shields.io/github/last-commit/lucho1/CronosEngine?color=blue&label=Last%20Commit&style=plastic)

![Code Size](https://img.shields.io/github/languages/code-size/lucho1/CronosEngine?color=green&label=Code%20Size&style=plastic)
![Repo Size](https://img.shields.io/github/repo-size/lucho1/CronosEngine?color=green&label=Repo%20Size&style=plastic)
![GH Top Language](https://img.shields.io/github/languages/top/lucho1/CronosEngine?color=green&style=plastic)
![GH Total Languages](https://img.shields.io/github/languages/count/lucho1/CronosEngine?color=green&style=plastic)

![Total Downloads](https://img.shields.io/github/downloads/lucho1/CronosEngine/total?color=green&label=Total%20Downloads&style=plastic)
![Watchers](https://img.shields.io/github/watchers/lucho1/CronosEngine?style=social)
![Forks](https://img.shields.io/github/forks/lucho1/CronosEngine?style=social)

Cronos Engine is a 3D Game Engine based on OpenGL and made by 2 students (Lucho Suaya and Roger Leon) of the Degree in Videogames Design and Development of Universitat Politècnica de Catalunya for the Videogames Engines subject of 3rd course.

We used SDL, SDL_Mixer, STL, OpenGL4, ImGui, Premake, GLM, GPUDetect, MMRG and GLAD.
**[TODO]** Put the logo image here and links to these libs!

## Innovating Features
* Extensive Filesystem with good UX Functionalities
* Asset Panel
	-Asset Browser (Searcher)
	-Workflow in and out
	-Renaming of Assets
	-Creation/Deletion of Assets and Folders
	-Names of Assets are 10 chars max but the full name appears when the mouse is on top

* Started the implementation of node System
* Extensive System Info Read (on GPU, CPU, Memory and Software!)
* Cronos Engine will detect if you have a dedicated NVIDIA or AMD Graphics Card and will use it automatically!
* Usage of Precompiled Header to imporve build performance
* Engine's Project built with a Premake file

## Usage
**Camera Controls**
* Right Click:			Camera Look Around
* Right Click + WASD:		Move camera and look around (using camera's forward vector)
* Alt + Left Click:		Orbit object (by now, orbit center)
* Alt + Left Click + WASD:	Orbit object (by now, orbit center) and move around it.
* Shift while Moving:		Duplicate camera moving speed.
* T, G:				Move camera up and down.
* Mouse Wheel:			Zoom in/out.
* F:					Focus camera at object (by now, focus center)

## Changelog
v0.1 **[TODO]** Fill this
* Camera Movement
* Engine's basic GUI
   * Implemented Asset Panel
				->Asset Browser (Searcher)
				->Workflow in and out
				->Renaming of Assets
				->Creation/Deletion of Assets and Folders
				->Names of Assets are 10 chars max but the full name appears when the mouse is on top
   * Full Configuration menus
   * UI Panels
				->Assetbrowser Panel
				->Inspector Panel (Under construction)
				->Hierarchy (Under Construction)
				->Configuration Panel
* Filesystem Implemented
* Hardware's Log Window - Extensive System Info Read (on GPU, CPU, Memory and Software)
* Performance Window to track useful info (Memory, FPS, ...)
* Detection of a dedicated NVIDIA or AMD Graphics Card - automatic usage
* Implementation of MMRG to track memory leaks and usage (for internal purposes by now)
* Usage of GPUDetect library to get CPU/GPU information
* Usage of Precompiled Header to imporve build performance
* Engine's Project built with a Premake file

## Credits
Cronos Engine is developed by **LuchoSuaya**(@lucho1) and **Roger Leon** (@rleonborras) for Videogames Engines subject of UPC's Degree in Videogames Design and Development, taught by Marc Garrigó Garcia [@markitus18].

**[TODO]** Put devs fotos. And what each did.

Take a look to our web: **[TODO]** Put the web here
Take a look to our Github Repository: https://www.github.com/lucho1/CronosEngine

## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-newfeature`
3. Commit your changes: `git commit -am 'Add some
feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request and we might look into it. :D

## License
**Copyright (c) 2019 Lucho Suaya Leiro and Roger Leon Borras**
Licensed under the **[Apache License](LICENSE.txt)**

Cronos Engine is Licensed under Apache License (http://www.apache.org/licenses/), you'll find the details in the License file in Github repository or in Engine's folder.
