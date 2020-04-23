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
I have only compiled this under Windows 10 using mingw-w64, but it should be possible to build it for Linux and MacOS without much modification. The compiler command can found in .vscode/tasks.json, if you wanna give it a go.
