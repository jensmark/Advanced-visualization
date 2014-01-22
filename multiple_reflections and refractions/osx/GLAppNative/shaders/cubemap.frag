#version 150

uniform samplerCube tex;

smooth in vec4 uv;

out vec4 out_color;

void main() {
    out_color = texture(tex, uv.xyz);
}