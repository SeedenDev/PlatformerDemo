# PlatformerDemo 
A simple project I made to try making a simple 2D platformer game in 10h using the knowledge I had from my first months learning C++ and OpenGL.

Disclaimer: a lot of the OpenGL abstraction files directly came from my ["HelloOpenGL" project](https://github.com/SeedenDev/HelloOpenGL) with some modifications, so it is indeed a very ahh code overall
but I really wanted to try making a game on top of my abstraction and see how much I could achieve in this little time.
Also, this is the first time for me trying to make a game for real.

Last but not least, this CMake setup is the exact same as my ["HelloOpenGL" project](https://github.com/SeedenDev/HelloOpenGL) too, in order to have a cross-platform development environment for me.
As such, there are a bunch of [TODOs](https://github.com/SeedenDev/HelloOpenGL#todos) to make it even more powerful, the goal is to create the ultimate CMake template for C++ OpenGL development using GLFW/GLAD/etc... And also, there will be commits here following TODOs' completion on the other project, probably.

## Features
- PNG-based map loader. Create your own map using your favorite image editing software (see `src/Scene/Level.cpp` for the color code)
- Instanced rendering of the terrain's tiles (toggleable with Dear ImGui)
- Vision radius (very bad Fog of War, just a light circle around you)
- Basic 2D platformer game (with very poor platformer feature and movement sorry lol)
- Not rly a feature but if you want you can change the zoom level using Home/End.

## TODOs
- Better goal point like.. idk perhaps it is currently not visible in-game AND no visual effect when collided lmao
- Coins
- Checkpoints
- Upgrade enemy movement (atm: "random" direction but meh, following you if near but very bad AI)
- Better movement system
- Remove toggleable instancing, defaulting to using it for everything + maybe some batch rendering too?
- Better collision system (not looping through all of the terrain's tiles + atm can't fall inside a hole of width=1px + can stand on the micro-pixel of a tile's border, basically in the void)
- Sprite animations
- Breakable tiles with two actions: either simply disappear, or spawn a coin or a checkpoint or an enemy (at the current position or y+1)
- Wall ride (bouncing, surface's "slippyness" value ruling how long you can grip before slipping down, or perhaps you wont even be able to take a grip)
- Shift to go down through some special tiles. 2 types actually: one is a one-way, you can't go up through it, and the other is both way.
- SOUNDDDDS!!!!
- Perhaps hook/rope? like, you can hang from a hook or rope, and swing?
- Perhaps "dimension/plane" system (several 2D plane) and/or gravity system (using yaw/pitch/roll)
- Fix gitmodules
- And a bunch of CMake-related stuff ([see the "HelloOpenGL" project's README](https://github.com/SeedenDev/HelloOpenGL#todos))

## Requirements
- CMake 3.21 or higher
- An OpenGL 4.5+ capable GPU driver (Warning: may evolve in the future in order to test newer features)
- Linux x64 or Windows x64.
- Any C++20 compilers should work. Tested using:
  - **Linux**: GCC 16.1 or CLang 22.1
  - **Windows (to-be-tested)**: MSVC (using Visual Studio 2022 or 2026) *or [TODO] GCC/CLang (using Ninja generator)*
  - **Cross-compile Linux x64 -> Windows x64**: x86_64-w64-mingw32-g++ (using gcc 16.1) // BROKEN!!! bc of libcurl
  - **Cross-compile Windows x64 -> Linux x64**: WIP

### About MacOS support
MacOS Intel and Apple Silicon support is not my priority right now. The current codebase uses some 4.3+ OpenGL functions and if I do stuff on
this project it will probably be something else than fixing this because I have a lot to do to improve the game.
Nonetheless, feel free to do your own modifications if you want to try making it run on Mac (and make a PR :D).

## Setup / Build / Run / Cross-compile
Simply put: basic CMake project. Setup and build with the preconfigured presets. You also have cross-compilation toolchain files (WIP).
Or you can use your favorite IDE (tested with Microsoft Visual Studio 2022 on Windows x64 and CLion on Linux x64).

- CLion info: don't forget to set the WORKING_DIR in your run configuration. An example config file can be found in `cmake/ide-templates/clion/runConfigurations`.
You should be able to use the `$ProjectFileDir$` variable to automatically target this root folder.

- Visual Studio 22 info, two ways of doing it: either way after the CMake project has been setup, you can just launch any preset with or without debugging as usual.
	- Simply open this root directory (where the `CMakeLists.txt` file is) with Visual Studio, it should automatically setup the CMake project for you with the right stuff in the Solution Explorer.
	- Or, you can also use `cmake --preset windows-x64-msvc-vs2022` (or vs2026, untested) or run `cmake/Setup.bat` and then open the .sln or .vcxproj file (inside the `build/` folder) with Visual Studio.
However you will need to enter the folder view ("Switch between solution and available views" button in the top bar of the Solution Explorer) in order to see all the right files and folders from the root directory.

Also btw, don't forget to include the `assets` folder alongside the executable if you want to send it to your friends!


## Third-party libraries
- GLAD 2.0.8 (https://github.com/Dav1dde/glad)
- GLFW 3.5.1 (https://github.com/glfw/glfw)
- GLM 1.0.3 (https://github.com/g-truc/glm)
- stb_image 2.30 (https://github.com/nothings/stb)
- Dear ImGui 1.92.2b (https://github.com/ocornut/imgui)

## Licence
WIP but pbby no licence

Every third party licence is available in their corresponding vendor subfolders.