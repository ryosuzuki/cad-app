# CSCI-5229 Project: CAD App
Simple 3D CAD Design Tool

Ryo Suzuki
ryo.suzuki@colorado.edu

# Compiling

For Linux (Ubuntu), you should first install dependencies. (Tested with Ubuntu 16.04 LTS)

```
$ apt-get install cmake xorg-dev libglu1-mesa-dev
```

Then, build with `CMake`

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

Run the application

```
$ ./cad-app
```

Stop the application with `ESC` key or `Ctrl-C` in terminal.


# How to play

## Step.1: Load the OBJ file from your computer.
Clicking the `Load` button, the file manager shows up. You can choose your own OBJ file from your local computer.
As default, I show `bunny.obj`.

## Step.2: Control the camer
By dragging the mouse, you can control the camera. By scrolling, you can zoom in and out.

## Step.3: Toggle wireframe
By clicking the `wireframe` button, you can change the wireframe or solid body.


# Remaining tasks
For some reasons, the lighting does not work for now. I need to fix as soon as possible.
Also, I will allow the user to interactively change the color with color picker.
After that, I'd like to add deforming feature.





