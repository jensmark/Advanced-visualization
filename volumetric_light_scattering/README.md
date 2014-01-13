Volumetric light scattering as a post-processing effect
======================

Volumetric light scattering implemented using a post-processing shader computing the light scattering by the formula; ![equation](http://latex.codecogs.com/png.latex?L%28s%2C%5Ctheta%2C%5Cphi%29%20%3D%20exposure%20%5Ctimes%20%5Csum_%7Bi%3D0%7D%5Endecay%5Ei%20%5Ctimes%20weight%20%5Ctimes%20%5Cfrac%7BL%28s_i%2C%5Ctheta_i%29%7D%7Bn%7D)

======================
TODO:
Implement a screen space occlusion method
	- Occlusion pre-pass
	- Occlusion stencil
	- Occlusion contrast