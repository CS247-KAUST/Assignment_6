# Assignment 6 Description (9% of total grade) #

**Due: May 10th**

The task for this assignment is the implementation of Line Integral Convolution (LIC) plus color coded overlays for scalar fields.

## Reading assignments ##

* Brian Cabral, Leith Casey Leedom: Imaging vector fields using line integral convolution SIGGRAPH '93 https://dl.acm.org/citation.cfm?id=166151

## Basic Tasks ##

+ Implement Line Integral Convolution on the GPU (using fragment shaders). 
  * Perform LIC in the fragment shader using a noisefield texture and a vector-field texture as input.
  * Render the result on a screen filling quad.

+ Overlay a scalar field.
  * Render the provided scalar fields at a time and blend it over the LIC field. 
  * Either use a separate render pass or combine the scalar field and the LIC in the same fragment shader.
  * Use color mapping for the scalar field.

## Minimum Requirements ##

+ GPU‐based LIC using GLSL (40 points)
  * Generate binary noise image (0,1) to be used for LIC 

+ Extension of GPU‐based LIC using GLSL
  * User adjustable kernel size for LIC (use a uniform variable) (10 points)
  * Do backward Integration along with the forward integration (25 points)

+ Scalar field overlay plus color mapping.
  * Use either a color map texture, like the transfer function for volume rendering, OR interpolate a few constant
    colors manually (e.g. green -> yellow -> red) in the shader. (20 points)
    * User adjustable blend factor (use a uniform variable) (5 points)

## Notes ##

**Note that when we load the data to the GPU (shaders) the vectors will be between 0-1, you will need to adjust it to include negative values as well.**

* The description and the download location of the data can be found in the file README_data.txt found in the source code.
* The vector data is in the vector_array, where every 3 elements in the array will give you the xyz component of a vector. After one 2D slice is finished, and if there is more than one time step, the vector data continues for the next time step.
  So the size of the array = 3 * width * height * number_of_timesteps. More information can be found in the README_data.txt file found in the source code.
* There aren't prototypes for every function you might need. Create functions as you need them.


## Screenshots for Minimum Requirements Solution ##

![LIC.png](https://bitbucket.org/repo/Mq6ygx/images/3352886959-LIC.png)
![LIC2.png](https://bitbucket.org/repo/Mq6ygx/images/3222200156-LIC2.png)
