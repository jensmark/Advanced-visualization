#version 150

uniform samplerCube env_map;

smooth in vec3 n;
smooth in vec3 v;

out vec4 out_color;

void main() {
    vec3 n = normalize(n);
    
    out_color = texture(env_map, n);
}