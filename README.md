# Virtual Drone

A virtual environment for testing drone control functions.

## Dependencies

* [SFML library](https://www.sfml-dev.org/) - `sudo apt-get install libsfml-dev`

## Compiling on Linux

Run `./compile.sh`.

## Command line arguments

When running the compiled binary, you can use a few command line arguments:
* `-record` - record screen frame by frame into `/frames/` folder
* `-noscreen` - only render offscreen (useful when recording)
* `-fps` - show FPS at the top right of the screen (wrong values when recording)

## Drone class

The main feature of this project is the Drone class. It handles both drone physics and rendering to the screen.
It can be found in `src/`.

### Physics part

Variables:

* `x,y` - drone position in physics world
* `speedx, speedy` - drone speed in physics world
* `angle` - drone angle in physics world
* `angular_velocity` - drone's angular velocity in physics world
* `inertia, mass, engine_dist, engine_pow` - drone properties

Functions:

* `Drone()` - constructor
* `physics()` - do a physics calculation
* `setPower()` - set engine powers for physics calculations

### Rendering part

Rendering is done using SFML library.

Functions:

* `setColor()` - set drone color
* `draw()` - inherited from Drawable class, can be called for drawing on screen/textures
