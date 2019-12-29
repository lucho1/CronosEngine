# CronosEngine
<p align="center">
  <b>Cronos Engine</b> is a 3D Game Engine based on OpenGL and made by 2 students <a href="https://github.com/lucho1">Lucho Suaya</a> and <a href="https://github.com/rleonborras">Roger Leon</a> of the Degree in Videogames Design and Development of Universitat Politecnica de Catalunya for the Videogames Engines subject of 3rd course.
</p>

![](res/Cronos_Engine_Logo.png?style=centerme)

**[TODO]** Add trailer??

## Screenshots & Video
**[TODO]** Add 1min video
**[TODO]** Add screenshots

## Innovating Features & Functionalities
* Shader System
* Lighting algorithm improved, better than Phong (Blinn-Phong) - Test the difference with Spotlights!
* Material System with shader rendering
* ZBuffer Real-Time Renderer (Check the Shaded dropdown in the editor, above scene window)

* Timing: Play/Pause/Stop & x2/x0.5 speed!
* Engine's camera speed control with mouse middle button (Unity-like!)

* GameObjects:
   * Copy/Paste with CTRL+C/CTRL+V & Deletion
   * Hierarchy system
   * Tranformations & AA/OO BBs
   
* Mesh & Textures importing
* Resource Manager: All data saved/loaded in binary own format and loaded into a Resource Manager to optimize resources loading and avoiding files duplication. All assets in the filesystem are loaded and saved to own file format from the beginning, and, if loaded, loaded from the own file format.
* Extensive Filesystem with good UI/UX Functionalities (asset finder, good navigation system, drag & drop...)
* Scene Serialization

* Many different types of primitives creation (check it out!)
* OpenGL Automatic Error Handler and logger (check out Errors.h!) and with possibility of calling GLCall() for every OGL Function to get possible errors with OGL functions/methods (through glGetError()) with extensive information
* Extensive System Info Read (on GPU, CPU, Memory and Software!)
* Cronos Engine will detect if you have a dedicated NVIDIA or AMD Graphics Card and will use it automatically!
* Usage of Precompiled Header to imporve build performance
* Engine's Project built with a Premake file and Project.bat generator to automatize build process

## Shaders Sub-System
**[TODO]** Add 2 gifs
Cronos Engine has implemented a full Shader System in which you will be able to modify and create shaders from the engine's editor by clicking twice in a shader file in the filesystem. To do so, you'll have to click modify, and it will save the file automatically once you compile successfully (check the console for errors!).

From there, you will be able to assign any shader to any material (which, at its time, materials can be dropped in materials components to be set) and prepare yourself to do lots of magnific things. Cronos Engine uses Shaders to support the Illumination and Material system, for instance! This means that all is rendered passing through the Shader Pipeline of the engine (except debug and UI draw). There is a basic shader used for this purpose, it is unmodifiable but you can check it out to do your own from the filesystem, this will create a new Shader (from a default one) so you can use it at your will. The basic shader gets Diffuse, Specular and Normal textures and uses them to calculate all the Material Rendering.
To create a Shader, as told, you will get a default one which contains a fragment/pixel and a vertex shader ready to use and to modify, but we also support Geometry Shaders compilation, so you can add the specification to the shader you create, just like it's already done with Vertex & Fragment/Pixel ones.

But also, in addition, we have created a Water Shader (unmodifiable, too, but readable!) so you can see an amazing Water Simulation fully calculated in the Shader and with the parameters of the water modifiable from the water material, so you can play with it! We invite you to check it out!

We use GLSL language (so the shader files are in .glsl).

### Filesystem
We have also implemented a whole Filesystem in Cronos. It features, among others, asset browser, folder and files creation and modification (name, path...), open files in explorer, carefully designed icons, asset viewer, drag & drop, shaders edition (with a text editor) and support for .jpg, .jpeg, .png, .dds & .tga textures.

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
