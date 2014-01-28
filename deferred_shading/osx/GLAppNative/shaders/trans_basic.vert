#version 150

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

in vec3 position;

void main() {
	vec4 pos = modelview_matrix * vec4(position, 1.0);
    gl_Position = projection_matrix * pos;
}