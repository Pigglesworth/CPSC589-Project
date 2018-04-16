# CPSC589-Project

This application allows the user to draw lines defining volumes which are used to create trees.
These trees can be exported as .obj files.

Usage
==============
Each two lines drawn will define its own volume. Multiples volumes can be drawn.
When you are done with this process, you place the starting point of the tree.
When the tree has grown to an acceptable state, you can then generate a mesh.
Once the mesh is generated, you can once again begin drawing a new line to make a new tree.

Controls
--------------

 - W: zoom camera in
 - S: zoom camera out
 - A: rotate camara clockwise
 - D: rotate camera counter-clockwise
 - Q: pan camera up
 - E: pan camera down
 - Enter: Finish drawing lines, allow placement of starting point
 - Escape: Generate tree mesh
 - Control+S: Save tree to .obj file
 
Compilation
==============
If you have visual studios 2017, this repository should work right out of the box.
If you have a different visual studios version, you may have to retarget the solution.

If you attempt to compile it with a different compiler, you will have to include the following libraries:
 - GLEW
 - GLFW3
 - GLM