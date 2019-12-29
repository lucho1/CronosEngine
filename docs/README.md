# CronosEngine
<p align="center">
  <b>Cronos Engine</b> is a 3D Game Engine based on OpenGL and made by 2 students <a href="https://github.com/lucho1">Lucho Suaya</a> and <a href="https://github.com/rleonborras">Roger Leon</a> of the Degree in Videogames Design and Development of Universitat Politecnica de Catalunya for the Videogames Engines subject of 3rd course.
</p>

![](res/Cronos_Engine_Logo.png?style=centerme)

## Screenshots & Video
**[TODO]** Add 1min video
**[TODO]** Add screenshots

Download it now [here](https://github.com/lucho1/CronosEngine/releases/download/CronosEngine_v0.3/CronosEngine_v0_3.zip)! You can also check the [latest release](https://github.com/lucho1/CronosEngine/releases/tag/CronosEngine_v0.3) or [other releases](https://github.com/lucho1/CronosEngine/releases)

## Shader System
**TODO: ADD GIFS**
For this version, you can modify and use shaders. There are 4 shaders at the beginning (a water one, a default one, a basic one which Cronos Engine uses to render all and a modifiable one). All of them but the modifiable one, are not modifiable, you can test modifications in the modifiable shader and compile it, but the others won't allow any modification.
However, you can create new shaders, they will be a copy of the default one, which provides the minimal implementation for the shader to compile. From there, is all yours! Enjoy it! You'll see a "Modify" button, which will allow you to modify/stop modifying the shader file. To modify the modifiable or a created shader, just pick on it twice in the asset editor and open the inspector, at the right side of the UI (by the way: you can create a new one with right click --> new shader, the same for materials).

You will see the GLSL structure thanks to the TextEditor implemented (which features colored text!), and is very intuitive and straightforward: Each shader in Cronos will be composed on a part which is the Vertex shader part, and on another part which is the Fragment or Pixel. Both have their characteristics and main functions. In addition, you can add a Geometry shader if you want, it will be supported.

Once you finished with the shader, press Compile and check the console for feedback, it will log any error that there could be. If it's all OK, the shader will be successfully compiled and saved. You can use it at any material you want, they all have a dropdown to chose the shader to use. Of course, materials can be dropped in the objects inspector to set them.

And talking about materials, remember that you can modify them! You can change their diffuse and specular textures, as well as its color and shininess (by now, we're looking forward on further expansions on materials).

Note that the basic shader has an assertion system which is very useful!

### Water Simulation
**TODO: ADD GIFS**
We were required, along with the shaders implementation, to test them with a Simple Water simulation. If you open the window tab, you will be able to activate it. At the beginning you will see that the movement seems a bit strange, but is made on purpose, because you will see that a pannel will also be opened with many different values. You can modify them all to make the water as you want. You will also be able to change the color.
Our council first, when you find this water, is to lower a bit the multiplicator value at the bottom part of the pannel and try to lower velocities and amplitudes/lengths. From there, the water is your sandbox!!

We have also prepared something else: If you go to the engine's folder, inside, at res/models/waterPlane folder, you will see a textures folder. As you may the deduce, the water simulation uses "water_tex_used.png" texture, but we let you there 5 more textures so you can change them and see which one fits the best! (You can also change the specular one, there are 2 of it, but it won't be a huge change).

You can read the Water Shader to check how the simulation is done.

### Light System
And talking about shaders and materials, is also important to mention the implemented lighting system, based on Blinn-Phong algorithm. You can create lights from the hierarchy editor (where all objects are created) and use them as you want.

You will see in the light component of the object created a long slider of 3 positions. This is what decides the light type (pos 0 is directional, 1 pointlight and 2 spotlight). You will see there different options for the different lights, they are by now mixed, but the characteristics for one light type won't affect the other if you switch the light type.

Concretely, you will see the direction from which light if you have a directional light, the cutoff angles for spotlight and other values that affect mostly poinlights (but some of them also affect the spotlight and directional, such as light attenuation or intensity).

Of course, you can also change the light color from there.

**Warning:** Be careful. Cronos Engine won't support more than 20 spotlights, more than 20 pointlight or more than 8 directional lights at a time. If you try to create more, the shaders will active their assertion system and lights will blink in different colors on purpose. Delete the excess of lights to fix it. This will improve in the future, when my head learn about SSBOs to pass as many uniforms as I want :D

### Filesystem
We have also implemented a whole Filesystem in Cronos. It features, among others, asset browser, folder and files creation and modification (name, path...), open files in explorer, carefully designed icons, asset viewer, drag & drop, shaders edition (with a text editor) and support for .jpg, .jpeg, .png, .dds & .tga textures.

## Innovating Features
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

## Credits
Cronos Engine is developed by **LuchoSuaya** ([@lucho1](https://github.com/lucho1)) and **Roger Leon** ([@rleonborras](https://github.com/rleonborras)) for Videogames Engines subject of UPC's Degree in Videogames Design and Development, taught by **Marc Garrigo Garcia** ([@markitus18](https://github.com/markitus18)).

![](res/fotiyo.jpg?style=centerme)

Take a look to our [Github Repository](https://www.github.com/lucho1/CronosEngine)

### Individual Work
* **Lucho Suaya**: In charge of Engine's main systems (core functionality, game objects & components, camera, octrees optimization...) & Rendering workflow (lighting and material system, shaders sub-system...).

* **Roger Leon**: In charge of the whole UI, entire engine's filesystem and resource manager & serialization and engine's aspect as well as handeling several game objects' functionality as well as hierarchies (parenting), UX functionality like copy/paste, delete...

Both have been involved in the system to load geometry and textures in the engine and primitives' creation.

## Libraries
We have used [SDL](https://www.libsdl.org/), [SDL_Mixer](https://www.libsdl.org/projects/SDL_mixer/), STL, [OpenGL4](https://www.opengl.org/) with [GLAD](https://glad.dav1d.de/), [ImGui](https://github.com/ocornut/imgui), [Premake](https://premake.github.io/), [GLM](https://glm.g-truc.net/0.9.9/index.html) and [MathGeoLib](https://github.com/juj/MathGeoLib) (for some purposes), [GPUDetect](https://github.com/GameTechDev/gpudetect), [MMGR](https://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml), [ParShapes](https://prideout.net/shapes), [Assimp](http://www.assimp.org/), [Devil](http://openil.sourceforge.net/) and [JSON for Modern C++](https://nlohmann.github.io/json/). Also, for the filesystem we've used a library of [Text Edition](https://github.com/BalazsJako/ImGuiColorTextEdit) and the [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) library for Guizmo implementation.

## License
**Copyright (c) 2019 Lucho Suaya Leiro and Roger Leon Borras**
Licensed under the **[Apache License](https://github.com/lucho1/CronosEngine/blob/master/docs/LICENSE)**.

Cronos Engine is Licensed under Apache License ([http://www.apache.org/licenses/](http://www.apache.org/licenses/)), you'll find the details in the License file in Github repository or in Engine's folder.
