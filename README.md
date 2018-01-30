Rasterization using Eigen
=============

OpenGL code to draw a triangle, using shaders. Also, the rasterization is using barycentric coordinates. Keys H and J rotate the triangle, K and L translate the triangle, 

### Using Eigen

In all exercises you will need to do operations with vectors and matrices. To simplify the code, you will use [Eigen](http://eigen.tuxfamily.org/).
Have a look at the [Getting Started](http://eigen.tuxfamily.org/dox/GettingStarted.html) page of Eigen as well as the [Quick Reference](http://eigen.tuxfamily.org/dox/group__QuickRefPage.html}) page for a reference of the basic matrix operations supported.

### GLFW

To handle input events (keyboard and mouse), we rely on GLFW to abstract the low-level OS-specific details of the process. Please refer to the [documentation](http://www.glfw.org/docs/latest/) of GLFW for more details.

### Preparing the Environment

Use CMake to setup the code.
