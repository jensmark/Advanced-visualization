#version 150

smooth in vec3 n;
smooth in vec3 v;
smooth in vec4 c;

out vec4 out_color[3];

void main() {
    out_color[0] = c;
    out_color[1] = vec4(n, 1.0);
    out_color[2] = vec4(v, 1.0);
}