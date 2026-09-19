# Almondbread

Almondbread is a GPU-accelerated Mandelbrot set renderer written in C++. It allows you to look around and zoom into the fractal in realtime:

![](res/mandelzoom_demo.webp)

It also supports rendering the corresponding Julia set for the point under the cursor, which makes for some nice trippy user-controlled visuals:

![](res/julia_demo.webp)

It was written as a way for me to better understand fractals, and also to learn some shader programming basics.

This program is quite GPU intensive and so requires a relatively poweful and modern videocard.

## Controls
[Left Mouse Button] Zoom into point  
[Arrow Keys] Move around  
[Spacebar] Show Julia set  
[W] Zoom in  
[S] Zoom out  
[A] Lower maximum iterations  
[D] Increase maximum iterations  

## Compiling
### Windows
Built under Windows 10 using mingw-w64. The compiler command can be found in .vscode/tasks.json.

### macOS
Install GLFW with Homebrew, then build with make. Run from the repository root, since the shaders and palette are loaded relative to the working directory:

```
brew install glfw
make
./build/almondbread
```

Apple GPUs have no hardware double precision, so on macOS the fractal is computed with 32-bit floats. Rendering stays on the GPU, but the image breaks up at roughly 10,000x zoom instead of the ~10^12x that doubles allow on Windows.

### Linux
Not tested, but the macOS Makefile should need only minor changes (link against your distro's GLFW and OpenGL instead of the Apple frameworks).
