# CronosEngine
Cronos Engine is a 3D Game Engine based on OpenGL and made by 2 students (Lucho Suaya and Roger Leon) of the Degree in Videogames Design and Development of Universitat Politecnica de Catalunya for the Videogames Engines subject of 3rd course.

We used SDL, SDL_Mixer, STL, OpenGL4 with GLAD, ImGui, Premake, GLM, GPUDetect, MMRG, ParShapes, Assimp, Devil, a couple of headers of COCO for AABBs and JSON for Modern C++.

## Innovating Features
* Improved Illumination System: Blinn-Phong is used, an improvement of the Phong algorithm. The difference can be tested with spotlights, by activating it in the Rendering Mode tab (in the Scene window)
* Performance Measurements on different ways of rendering objects with materials
* Support for Geometry Shader - Test it by yourself by adding a Geometry Shader in a new shader.
* Filesystem improved - New features such as creating folders, shaders, copying paths...
* Quicksort algorithm to order assets to load them in the order we want

* Shaders System (as assets!) - Create new shaders and modify/compile them from the Engine! 
* Engine's camera moves similar to Unity! (Check controls)
* Guizmo implementation (accessed from the Transform Component Pannel or with keys)
* Water simulation calculated from shaders - Modifiable from the Water Simulation pannel
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

## Installation
Just download a release and run the executable! Inside 'res' folder you'll find all the resources used by the engine (textures, models loaded, configuration,...).

**Note:** Be careful when modifying things on config file, it will probably modify engine's configuration. And please, don't delete the dll files, they are needed for the engine to work. Also, be careful when scaling objects, the Bounding Boxes transformations are not very well performed and they can be badly deformed (but kinda work).

**Note 2:** Warning! If you wanna work with bullet, first, integrate it, then, go to MathBuildConfig.h, look for the definition of MATH_BULLET_INTEROP, and uncomment it. Should work :)

## Usage
**Camera Controls**
* Right Click:		        	Camera Look Around
* Right Click + WASD:	    	Move camera and look around (using camera's forward vector)
* LAlt + Left Click:		    Orbit object (or center if none selected)
* LAlt + Left Click + WASD:	Orbit object and move around it (or center if not selected)
* Shift while Moving:	    	Duplicate camera moving speed.
* Right Click + T, G:	    	Move camera up and down.
* Mouse Wheel:			        Zoom in/out.
* F:					              Focus camera at object (or center if not selected)

* W, E, R while selecting an object (and not moving the camera) to change gizmo mode
* CTRL+C/CTRL+V to Copy/Paste selected objects

For timing: Play/Pause/Stop button work as spected. Faster/Slower/Next-Frame won't be seen very well on the time shown but will on delta time of the scene. For the Faster/Slower be careful, because because they keep active, I mean, if you play and press one of those buttons, the time will go faster/slower, and you have to press again to "unactive" this. Of course, with the stop button they are put to unactive too. 

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
* Frustum Culling & Octrees Rendering Optimization
* Mouse Picking Implementation
* Window/Scene resizing fix
* UI Exhaustively updated

* Resource Manager & Scene serialization diverse fixes
* Filesystem improved - New features such as creating folders, shaders, copying paths...

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
Cronos Engine is developed by LuchoSuaya (@lucho1) and Roger Leon (@rleonborras) for Videogames Engines subject of UPC's Degree in Videogames Design and Development, taught by Marc Garrigo Garcia (@markitus18).

Take a look to our web: https://lucho1.github.io/CronosEngine/
Take a look to our Github Repository: https://www.github.com/lucho1/CronosEngine

## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-newfeature`
3. Commit your changes: `git commit -am 'Add some
feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request and we might look into it. :D

Warning! If you wanna work with bullet, first, integrate it, then, go to MathBuildConfig.h, look for the definition of MATH_BULLET_INTEROP, and uncomment it. Should work :)

## License
**Copyright (c) 2019 Lucho Suaya Leiro and Roger Leon Borras**
Licensed under the Apache License.

Cronos Engine is Licensed under Apache License (http://www.apache.org/licenses/), you'll find the details in the License file in Github repository or in Engine's folder.