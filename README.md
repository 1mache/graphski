# graphski
Graph visualizer in C++ using SFML

## Dependencies
[SFML](https://github.com/SFML/SFML)\
[nlohmann's json](https://github.com/nlohmann/json)

## How to draw
- Create a node - Left mouse button
- Move nodes around - hold Left mouse button on them
- Create an edge - select two nodes by clicking on them with the right mouse button
- Write to json file (not sure why you would do that) - press W when you're done drawing

## Things to not do (at this point)
- There can be multiple edges from some node v to some node u. I did not bother to fix it yet :P
- DO NOT RESIZE THE WINDOW. Even if it lets you (I'm not sure I disabled that). The code is not prepared for such wizardry yet.

## Building
Build the project by running:
```bash
  premake/premake5.exe vs2022
```
(if you're using *VisualStudio2022*)\
Or by executing `build.bat` (on Windows).\
This will create VisualStudio2022 project files inside the repo folder.\
(If you're not on Windows or not using Visual Studio 2022 Im sorry... you will have to link SFML on your own)
