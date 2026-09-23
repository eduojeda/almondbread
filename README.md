# Almondbread

Almondbread is a GPU-accelerated Mandelbrot set renderer written in C++. It allows you to look around and zoom into the fractal in realtime:

![](res/mandelzoom_demo.webp)

It also supports rendering the corresponding Julia set for the point under the cursor, which makes for some nice trippy user-controlled visuals:

![](res/julia_demo.webp)

It was written as a way for me to better understand fractals, and also to learn some shader programming basics.

This program is quite GPU intensive and so requires a relatively poweful and modern videocard.

## Controls
[Left Mouse Button] Zoom into point  
[Right Mouse Button] Zoom out, keeping the point under the cursor in place  
[Middle Mouse Button] Drag to move around  
[Arrow Keys] Move around  
[Spacebar] Show Julia set  
[W] Zoom in  
[S] Zoom out  
[A] Lower maximum iterations  
[D] Increase maximum iterations  
[Tab] Print the zoom level and the exact view center  

The top-left corner shows the frame rate, the zoom level and the coordinates under the cursor. The frame rate is measured while frames render back to back, such as while zooming; otherwise it shows how long the last frame took. Coordinates carry enough decimals to tell neighbouring pixels apart, and past about 40 decimals the middle digits are elided, keeping the leading ones that say where you are and the trailing ones that change as the cursor moves. Tab prints the full values.

## Deep zoom
There is no zoom limit other than how long you are willing to wait for a frame. The view center is kept as an arbitrary-precision fixed-point number, and the CPU computes one reference orbit through it at whatever precision the current zoom needs. The GPU then iterates only each pixel's small offset from that orbit (perturbation theory), in floats that carry a separate integer exponent so offsets far below 10^-38 do not underflow. Rebasing, which restarts the reference whenever a pixel's own orbit passes closer to 0 than its offset, keeps that single reference valid for every pixel. Bilinear approximation skips most of the iterations: while a pixel's offset is small enough, a run of 2^k iterations collapses into one multiply-add, and the CPU precomputes a binary tree of these runs along the reference orbit.

Checked against direct arbitrary-precision computation on a grid of pixels, down to 10^-998: every sampled pixel matches on views around c = i, and 98.2-99.6% match exactly on deep minibrots. The rest are chaotic boundary pixels whose true value changes within a thousandth of a pixel.

Frame times on an Apple M4 Max at 1800x1800, viewing minibrots:

| Zoom | Maximum iterations | Seconds per frame |
| --- | --- | --- |
| 10^100 | 9,841 | 0.06 |
| 10^203 | 23,317 | 0.11 |
| 10^480 | 67,802 | 0.26 |
| 10^998 | 168,712 | 0.46 |

Holding the mouse to zoom at 10^1000 around c = i runs at about 11 ms per frame. Frame time grows with the iteration limit, which rises with zoom depth; A and D adjust it. Once the iteration limit passes 256 the palette repeats every 256 iterations, so deep views keep their contrast.

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

### Precision modes
The default build renders by perturbation, as described above. The older modes compute each pixel from scratch in the shader and can be built for comparison:

```
make clean && make PRECISION=FLOAT    # or DOUBLE_FLOAT, DOUBLE, ARBITRARY (the default)
```

Plain float stops resolving detail at about 10^4x zoom, double-float (a pair of floats per value) at about 10^11x, and double at about 10^12x. Apple GPUs have no hardware doubles, so on macOS the double mode falls back to software rendering at about 30 seconds per frame.

### Linux
Not tested, but the macOS Makefile should need only minor changes (link against your distro's GLFW and OpenGL instead of the Apple frameworks).
