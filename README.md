# CSCE 438 Final Project: Single-View Modeling

## Usage

To start the application, a user must provide a path to an image file as the first argument on the
command line. For example:

    ./build/single_view_modeling ~/Downloads/reveille.jpg

Next, the application starts on the mesh screen where the user selects four corners of the "rear
wall" and the vanishing point. When you are setting the corner points, instead of dragging the
corner points themselves, drag the edges of the box. However, you can drag the vanishing point like 
normal. Use the mesh lines eminating from the selected vanishing point to help guide the points so
that the mesh aligns with lines in the image parallel to the camera.

When you are done setting the points, hit the enter key on your keyboard. This brings you into 
theater mode. Use your mouse to look around, and WASD to move the camera. Look at all the new
perspectives that can be made from a flat 2D image! When you are done, press the escape key on the
kayboard.

## Dependencies

The only dependencies needed to compile and run this project is OpenGL 3.3, which should come
supported by default with the drivers of any modern GPU, and CMake 3.1.

## Ubuntu

    apt install cmake

## macOS

    brew install cmake

## How to Build

### Linux and macOS

```bash
    git clone --recursive URL_TO_THIS_REPO svm # note the recursive flag
    cd svm
    ./build.sh
```

## How to Run

### Linux and macOS

    ./build/single_view_modeling
