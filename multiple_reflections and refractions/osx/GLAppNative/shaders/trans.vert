#version 150

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;
uniform vec3 light_pos;

in vec3 position;
in vec3 normal;

smooth out vec3 v;
smooth out vec3 l;
smooth out vec3 n;
smooth out vec4 uv;

void main() {
	vec4 pos = modelview_matrix * vec4(position, 1.0);
    
	v = normalize(-pos.xyz);
	l = normalize(light_pos - pos.xyz);
	n = normal_matrix*normal;
    uv = vec4(position, 1.0);
    
    gl_Position = projection_matrix * pos;
}