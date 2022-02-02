## Drone

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
