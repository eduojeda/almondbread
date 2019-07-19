## Almondbread

Almondbread is a GPU-accelerated Mandelbrot set renderer. It allows you to zoom into the fractal in realtime (limited by the GPU's maximum floating point precision). It also supports rendering the corresponding Julia set for the point under the cursor, which makes for trippy user-controlled visuals.

It was written as a way for me to better understand fractals, and also to learn some shader programming basics.

This program is quite GPU intensive and so requires a relatively poweful and modern videocard.

# Controls
[Left Mouse Button] Zoom into point
[Arrow Keys] Move around
[Spacebar] Show Julia set
[W] Zoom in
[S] Zoom out
[Q] Lower maximum iterations
[E] Increase maximum iterations

# Compiling
I have only compiled this under Windows 10 using mingw-w64, but it should be possible to run it in Linux and Mac without much pain. Instructions coming soon...