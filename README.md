# MR3
MR3 is a renderer that I am currently working on. The purpose of this project is to implement fun and interesting rendering methodologies (PBR, Ray Tracing, etc.) and potentially expand it to a game engine.

__DISCLAIMER:__ This project is written in C++17 and utilizes OpenGL 4.5. If you have a relatively new PC (or, at least, a non-ancient PC), you should be able to run this, however...
* If you're running this on macOS (which only supports up to OpenGL 4.1), enjoy the demo. 
* If you're running this on Windows and it does not launch properly, make sure to check if your GPU supports at least OpenGL 4.5. If it does, update your drivers. If it doesn't, enjoy the demo.

---

### Newest Features
1. Physically Based Rendering (PBR) of Shapes (Models to be added later)
2. Dear ImGUI support for real-time variable updates

---

### How to View this Project
If you want to view the code, you can either:
* View it directly from within the repository under the MR3 folder for .cpp files, and MR3\shaders for GLSL files.
* Clone this repository and launch the solution file to view in Visual Studio (assuming you have it installed).

If you want to just view the application, you can either:
* Watch the demo video "demo.mov" that I've made, which displays the current features.
* Download just the Release executable and the shaders, textures, and vendor folders from the MR3 folder into one directory, then launch the .exe.

#### Controls
W = Forward        S = Backward        A = Left        D = Right       

Space = Move Up        Left Ctrl = Move Down        Left Shift = Increase Speed

Left Alt = Locks the camera (for interacting with UI)

F11 = Fullscreen/Windowed

Escape = Close App

Backslash = Enable/Disable Wireframe