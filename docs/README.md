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

Cronos Engine is a 3D Game Engine based on OpenGL and made by 2 students ([Lucho Suaya](@lucho1) and [Roger Leon](@rlborras)) of the Degree in Videogames Design and Development of Universitat Politecnica de Catalunya for the Videogames Engines subject of 3rd course.

![](https://raw.githubusercontent.com/lucho1/CronosEngine/Cronos_development/CronosEngine/Cronos/res/Icons/Cronos_Engine_Logo.png)

We used [SDL](https://www.libsdl.org/), [SDL_Mixer](https://www.libsdl.org/projects/SDL_mixer/), STL, [OpenGL4](https://www.opengl.org/) with [GLAD](https://glad.dav1d.de/), [ImGui](https://github.com/ocornut/imgui), [Premake](https://premake.github.io/), [GLM](https://glm.g-truc.net/0.9.9/index.html) and [MathGeoLib](https://github.com/juj/MathGeoLib) (for some purposes), [GPUDetect](https://github.com/GameTechDev/gpudetect), [MMGR](https://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml), [ParShapes](https://prideout.net/shapes), [Assimp](http://www.assimp.org/), [Devil](http://openil.sourceforge.net/) and [JSON for Modern C++](https://nlohmann.github.io/json/).

## Screenshots

## Innovating Features for v0.3
* Lighting algorithm improved, better than Phong (Blinn-Phong) - Test the difference with Spotlights!
* Engine's camera speed control with mouse middle button

### Innovating Features for v0.2
* ZBuffer Real-Time Renderer (Check the Shaded dropdown in the editor, above scene window)
* GameObjects Copy/Paste with CTRL+C/CTRL+V
* Modifiable ambient color of the materials.
* Resource Manager: All data (not scene) saved/loaded in binary own format and loaded into a Resource Manager to optimize resources loading and avoiding files duplication.
* All assets in the filesystem are loaded and saved to own file format from the beginning, and, if loaded, loaded from the own file format.
* Timed-Game Mode

### Innovating Features for v0.1
* Extensive Filesystem with good UX Functionalities
* Asset Panel
  * Asset Browser (Searcher)
  * Workflow in and out
  * Renaming of Assets
  * Creation/Deletion of Assets and Folders
  * Names of Assets are 10 chars max but the full name appears when the mouse is on top
  * Icons with textures
  * Drag & Drop
  * Textures Changing
  * (Internal) File copy into project when dragging it and dropping it
  * Game Object Hierarchy (with no huge parenting for now)
  * Game Object Selection
  * Game Object debug draw (wireframe, vertices/polygons normals, positional axis...)

* Started the implementation of node System
* Many different types of primitives creation (check it out!)
* Shaders usage with Vertex Arrays and a good abstraction for buffers and textures classes
* Engine Icon

* Namespaced Engine code
* OpenGL Automatic Error Handler and logger (check out Errors.h!) and with possibility of calling GLCall() for every OGL Function to get possible errors with OGL functions/methods (through glGetError()) with extensive information
* Extensive System Info Read (on GPU, CPU, Memory and Software!)

* Cronos Engine will detect if you have a dedicated NVIDIA or AMD Graphics Card and will use it automatically!
* Usage of Precompiled Header to imporve build performance
* Engine's Project built with a Premake file and Project.bat generator to automatize build process

## Installation
Just download a release and run the executable! Inside 'res' folder you'll find all the resources used by the engine (textures, models loaded, configuration,...).
**Note:** Be careful when modifying things on config file, it will probably modify engine's configuration. And please, don't delete the dll files, they are needed for the engine to work.

## Usage
**Camera Controls**
* Right Click:		        	  Camera Look Around
* Right Click + WASD:	    	  Move camera and look around (using camera's forward vector)
* Right Click + TG:	       	  Move camera up and down
* Right Click + Mouse Wheel:  Modify camera speed
* Mouse Wheel:			          Zoom in/out.
* Shift while Moving:	    	  Duplicate camera moving speed.
* F:					                Focus camera at object (or center if not selected)
* LAlt + Left Click:		      Orbit object (or center if none selected)
* LAlt + Left Click + WASD:	  Orbit object and move around it (or center if not selected)

* CTRL+C/CTRL+V to Copy/Paste selected objects

For timing: Play/Pause/Stop button work as spected. Faster/Slower/Next-Frame won't be seen very well on the time shown but will on delta time of the scene. For the Faster/Slower be careful, because because they keep active, I mean, if you play and press one of those buttons, the time will go faster/slower, and you have to press again to "unactive" this. Of course, with the stop button they are put to unactive too. 

Also, ImGui handles its own controls to navigate through UI. You can also Save/Load scene in menu bar and do plenty of stuff through menus (check changelog).

## Changelog
v0.3
* Implemented camera components and several camera features (having many ones, chosing the rendering camera...)
* Wireframe draw fix
* Shader assets as external GLSL files and GLSL file reader
* AABBs/OOBBs fix
* Camera Frustums & Octrees Optimization for rendering
* Mouse picking
* Illumination system with Phong and three different light types (spot, directional and point lights)
* Illumination system improvement with Blinn-Phong
* Shaders System
* Material system rework to make them assets
* Water simulation working with a water shader and fully modifiable

v0.2
* Rework on Materials: Textures are now inside materials and many can be handled (in code level, not yet on a user level) as they are stored individually in an unordered map storing the texture and its type with a pointer to a main shader.
* Rework on Camera: Completely changed. Now is based on glm library and its movement has changed.
* Object Transformations: Now they can be translated, scaled and rotated from the editor.
* Models Fix: Now any model can be loaded.
* Renderer Little Rework: Now all rendering object is rendered from the render in the PostUpdate. In the mesh update, we call Render->Submit() to send the rendering data. 
* Octrees implemented to improve performance at renderer.
* ZBuffer Real-Time rendering.
* AABBs for Game Objects.
* GameObjects Copy/Paste .
* Resource Manager: All data (not scene) saved/loaded in binary own format and loaded into a Resource Manager to optimize resources loading and avoiding files duplication.
* Scene Save & Load
* TGA Textures Load/Set
* Modifiable ambient color of the materials
* Timing implementation: Ability to play pause and stop the game being done in the editor (even with a little "darkness" included at run-time). Also, possibility of move time faster/slower (at double and half), pass to the next frame and save the changes in run-time. Cute Icons included.
* All assets in the filesystem are loaded and saved to own file format from the beginning, and, if loaded, loaded from the own file format.

v0.1
* Many different types of primitives creation
* Models Load, including textures.
* 3D Renderer
* Game Objects and Components Hierarchy
* Shaders usage with Vertex Arrays and a good abstraction for buffers and textures classes
* Camera Movement
* Engine's basic GUI
  * Implemented Asset Panel
    * Asset Browser (Searcher)
    * Workflow in and out
    * Renaming of Assets
    * Creation/Deletion of Assets and Folders
    * Names of Assets are 10 chars max but the full name appears when the mouse is on top
    * File Copy when adding them at project (internally)
    * Drag & Drop
    * Textures Changing
  * Full Configuration menus
  * UI Panels
    * Assetbrowser Panel
    * Inspector Panel (Under construction)
    * Hierarchy (Under Construction)
    * Configuration Panel
* Filesystem Implemented
* Game Objects, Hierarchy and Debug Drawing
* Hardware's Log Window - Extensive System Info Read (on GPU, CPU, Memory and Software, using GPUDetect library to get CPU/GPU information)
* Performance Window to track useful info (Memory, FPS, ...)
* Detection of a dedicated NVIDIA or AMD Graphics Card - automatic usage
* Implementation of MMRG to track memory leaks and usage (for internal purposes by now)
* Usage of Precompiled Header to imporve build performance
* Engine's Project built with a Premake file and a Generate.bat file.
* OpenGL Automatic Error Handler and logger (check out Errors.h!) and with possibility of calling GLCall() for every OGL Function to get possible errors with OGL functions/methods (through glGetError()) with extensive information
* Engine Icon
* Other external libraries implementation

## Credits
Cronos Engine is developed by **LuchoSuaya** (@lucho1) and **Roger Leon** (@rleonborras) for Videogames Engines subject of UPC's Degree in Videogames Design and Development, taught by **Marc Garrigo Garcia** [@markitus18].

**[TODO]** Put devs fotos. And what each did.

Take a look to our web:
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
