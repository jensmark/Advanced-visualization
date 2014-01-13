Volumetric light scattering as a post-processing effect
======================

Volumetric light scattering implemented using a post-processing shader computing the light scattering by the formula; ![equation](http://latex.codecogs.com/png.latex?L(s,\theta,\phi)&space;=&space;exposure&space;\times&space;\sum_{i=0}^ndecay^i&space;\times&space;weight&space;\times&space;\frac{L(s_i,\theta_i)}{n})

======================
TODO:
Implement a screen space occlusion method
	- Occlusion pre-pass
	- Occlusion stencil
	- Occlusion contrast