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

Cronos Engine is a 3D Game Engine based on OpenGL and made by 2 students (Lucho Suaya and Roger Leon) of the Degree in Videogames Design and Development of Universitat Politecnica de Catalunya for the Videogames Engines subject of 3rd course.

![](https://raw.githubusercontent.com/lucho1/CronosEngine/Cronos_development/CronosEngine/Cronos/res/Icons/Cronos_Engine_Logo.png)

We used [SDL](https://www.libsdl.org/), [SDL_Mixer](https://www.libsdl.org/projects/SDL_mixer/), STL, [OpenGL4](https://www.opengl.org/) with [GLAD](https://glad.dav1d.de/), [ImGui](https://github.com/ocornut/imgui), [Premake](https://premake.github.io/), [GLM](https://glm.g-truc.net/0.9.9/index.html), [GPUDetect](https://github.com/GameTechDev/gpudetect), [MMRG](https://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml), [ParShapes](https://prideout.net/shapes), [Assimp](http://www.assimp.org/), [Devil](http://openil.sourceforge.net/), [JSON for Modern C++](https://nlohmann.github.io/json/), [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) and a [Text Editor](https://github.com/BalazsJako/ImGuiColorTextEdit) library for ImGui (you'll see links and stuff in webpage and in repository readme).

## Shader System
For this version, you can modify and use shaders. There are 4 shaders at the beginning (a water one, a default one, a basic one which Cronos Engine uses to render all and a modifiable one). All of them but the modifiable one, are not modifiable, you can test modifications in the modifiable shader and compile it, but the others won't allow any modification.
However, you can create new shaders, they will be a copy of the default one, which provides the minimal implementation for the shader to compile. From there, is all yours! Enjoy it! You'll see a "Modify" button, which will allow you to modify/stop modifying the shader file. To modify the modifiable or a created shader, just pick on it twice in the asset editor and open the inspector, at the right side of the UI (by the way: you can create a new one with right click --> new shader, the same for materials).

You will see the GLSL structure thanks to the TextEditor implemented (which features colored text!), and is very intuitive and straightforward: Each shader in Cronos will be composed on a part which is the Vertex shader part, and on another part which is the Fragment or Pixel. Both have their characteristics and main functions. In addition, you can add a Geometry shader if you want, it will be supported.

Once you finished with the shader, press Compile and check the console for feedback, it will log any error that there could be. If it's all OK, the shader will be successfully compiled and saved. You can use it at any material you want, they all have a dropdown to chose the shader to use. Of course, materials can be dropped in the objects inspector to set them.

And talking about materials, remember that you can modify them! You can change their diffuse and specular textures, as well as its color and shininess (by now, we're looking forward on further expansions on materials).

Note that the basic shader has an assertion system which is very useful!

**Important:** With some objects (especially primitives) there is a strange behaviour in which you  create them but there seems to be nothing. Actually, there is, is some trouble with the default material, so if you create a new material and set it to the object (or set one existing different from the default one), should fix it.

### Water Simulation
We were required, along with the shaders implementation, to test them with a Simple Water simulation. If you open the window tab, you will be able to activate it. At the beginning you will see that the movement seems a bit strange, but is made on purpose, because you will see that a pannel will also be opened with many different values. You can modify them all to make the water as you want. You will also be able to change the color.
Our council first, when you find this water, is to lower a bit the multiplicator value at the bottom part of the pannel and try to lower velocities and amplitudes/lengths. From there, the water is your sandbox!!

We have also prepared something else: If you go to the engine's folder, inside, at res/models/waterPlane folder, you will see a textures folder. As you may the deduce, the water simulation uses "water_tex_used.png" texture, but we let you there 5 more textures so you can change them and see which one fits the best! (You can also change the specular one, there are 2 of it, but it won't be a huge change).

### Light System
And talking about shaders and materials, is also important to mention the implemented lighting system, based on Blinn-Phong algorithm. You can create lights from the hierarchy editor (where all objects are created) and use them as you want.

You will see in the light component of the object created a long slider of 3 positions. This is what decides the light type (pos 0 is directional, 1 pointlight and 2 spotlight). You will see there different options for the different lights, they are by now mixed, but the characteristics for one light type won't affect the other if you switch the light type.

Concretely, you will see the direction from which light if you have a directional light, the cutoff angles for spotlight and other values that affect mostly poinlights (but some of them also affect the spotlight and directional, such as light attenuation or intensity).

Of course, you can also change the light color from there.

**Warning:** Be careful. Cronos Engine won't support more than 20 spotlights, more than 20 pointlight or more than 8 directional lights at a time. If you try to create more, the shaders will active their assertion system and lights will blink in different colors on purpose. Delete the excess of lights to fix it. This will improve in the future, when my head learn about SSBOs to pass as many uniforms as I want :D

### Rendering Optimization Measure
One of the things we were proposed to do, as a little innovation, is to measure different ways to render in Cronos Engine. This, originally, came from the doubt of which of some determined manners were faster. We won't bother you much with this, but, summarizing, we made a little investigation on it and we have measured the mentioned rendering ways, and the results can be seen in the pdf attached with this file or in our repository.

## Innovating Features for v0.3
* Improved Illumination System: Blinn-Phong is used, an improvement of the requested Phong algorithm. The difference can be tested with spotlights, by activating it in the Rendering Mode tab (in the Scene window, where the Wireframe mode is)
* Performance Measurements on different ways of rendering objects with materials
* Support for Geometry Shader - Test it by yourself by adding a Geometry Shader in a new shader.
* Filesystem improved - New features such as creating folders, shaders, copying paths...
* Quicksort algorithm to order assets to load them in the order we want

* Shaders System (as assets! Include an assertion system) - Create new shaders and modify/compile them from the Engine! 
* Engine's camera moves similar to Unity! (Check controls)
* Guizmo implementation (accessed from the Transform Component Pannel or with keys)
* Water simulation calculated from shaders - Modifiable from the Water Simulation pannel in Window Tab
* Materials System - Modify them from the Engine in Inspector pannel or in the Object's components pannel
* Choose from which camera to render from the Rendering pannel

## Installation
Just download a release and run the executable! Inside 'res' folder you'll find all the resources used by the engine (textures, models loaded, configuration,...).

**Note:** Be careful when modifying things on config file, it will probably modify engine's configuration. And please, don't delete the dll files, they are needed for the engine to work. Also, be careful when scaling objects, the Bounding Boxes transformations are not very well performed and they can be badly deformed (but kinda work).

**Note 2:** Warning! If you wanna work with bullet, first, integrate it, then, go to MathBuildConfig.h, look for the definition of MATH_BULLET_INTEROP, and uncomment it. Should work :)

**Note 3:** Sorry for the inconveniance, but the mouse picking sometimes enters in conflict with ImGuizmo and it doesn't work as expected.

## Usage
**Camera Controls**
* Right Click:		        	Camera Look Around
* Right Click + WASD:	    	Move camera and look around (using camera's forward vector)
* LAlt + Left Click:		    Orbit object (or center if none selected)
* LAlt + Left Click + WASD:	Orbit object and move around it (or center if not selected)
* Shift while Moving:	    	Duplicate camera moving speed.
* Right Click + T, G:	    	Move camera up and down.
* Mouse Wheel:			        Zoom in/out.
* Right Click + Mouse Wheel: Modify camera's veocity
* F:					              Focus camera at selected object (or center if not selected)

* W, E, R while selecting an object (and not moving the camera) to change gizmo mode

For timing: Play/Pause/Stop button work as spected. Faster/Slower/Next-Frame won't be seen very well on the time shown but will on delta time of the scene. For the Faster/Slower be careful, because because they keep active, if you play and press one of those buttons, the time will go faster/slower, and you have to press again to "unactive" this. Of course, with the stop button they are put to unactive too. 

Also, ImGui handles its own controls to navigate through UI. You can also Save/Load scene in menu bar and do plenty of stuff through menus (check changelog).

## Changelog
v0.3
* Shaders System Improved (modifications from Engine, supporting multiple shaders...)
* Materials System (as assets and references) improved
* Lighting System with Blinn-Phong
* Lighting System with Phong

* AA/OO BBs Fix - They are now well implemented
* Renderer Pannel for multiple purposes (set rendering camera, debug draws...)
* Transformations with a Gizmo

* Engine's camera controls improved
* Cameras Implementation
* Choose from which camera to render from the Rendering pannel
* Frustum Culling & Octrees Rendering Optimization
* Mouse Picking Implementation
* Window/Scene resizing fix
* UI Exhaustively updated

* Resource Manager & Scene serialization diverse fixes
* Filesystem improved - New features such as creating folders, shaders, copying paths...

* Performance Measurements on different ways of rendering objects with materials
* Support for Geometry Shader - Test it by yourself by adding a Geometry Shader in a new shader.
* Quicksort algorithm to order assets to load them in the order we want
* Guizmo implementation (accessed from the Transform Component Pannel or with keys)
* Water simulation calculated from shaders - Modifiable from the Water Simulation pannel in Window Tab


v0.2
* Rework on Materials: Textures are now inside materials and many can be handled (in code level, not yet on a user level) as they are stored individually in an unordered map storing the texture and its type with a pointer to a main shader.
* Rework on Camera: Completely changed. Now is based on glm library and its movement has changed.
* Object Transformations: Now they can be translated, scaled and rotated from the editor.
* Models Fix: Now any model can be loaded
* Renderer Little Rework: Now all rendering object is rendered from the render in the PostUpdate. In the mesh update, we call Render->Submit() to send the rendering data. 
* Octrees implemented to improve performance at renderer.
* ZBuffer Real-Time rendering.
* AABBs for Game Objects. 
* GameObjects Copy/Paste
* Resource Manager: All data (not scene) saved/loaded in binary own format and loaded into a Resource Manager to optimize resources loading and avoiding files duplication.
* All assets in the filesystem are loaded and saved to own file format from the beginning, and, if loaded, loaded from the own file format.
* Scene Save & Load
* TGA Textures Load/Set
* Modifiable ambient color of the materials
* Timing implementation: Ability to play pause and stop the game being done in the editor (even with a little "darkness" included at run-time). Also, possibility of move time faster/slower (at double and half), pass to the next frame and save the changes in run-time. Cute Icons included.

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
Cronos Engine is developed by **[LuchoSuaya](@lucho1)** and **[Roger Leon](@rleonborras)** for Videogames Engines subject of UPC's Degree in Videogames Design and Development, taught by [Marc Garrigo  Garcia](@markitus18).

Take a look to our web: https://lucho1.github.io/CronosEngine/
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

Cronos Engine is Licensed under Apache License ([http://www.apache.org/licenses/](http://www.apache.org/licenses/)), you'll find the details in the License file in Github repository or in Engine's folder.