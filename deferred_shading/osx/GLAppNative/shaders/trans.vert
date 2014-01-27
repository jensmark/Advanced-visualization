#version 150

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;

uniform vec4 color;

in vec3 position;
in vec3 normal;

smooth out vec3 v;
smooth out vec3 n;
smooth out vec4 c;

void main() {
	vec4 pos = modelview_matrix * vec4(position, 1.0);
    
	v = normalize(-pos.xyz);
	n = normal_matrix*normal;
    c = color;
    
    gl_Position = projection_matrix * pos;
}