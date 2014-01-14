#version 150

uniform vec3 diffuse;
uniform vec3 specular;
uniform vec3 ambient;
uniform float shininess;

smooth in vec3 n;
smooth in vec3 v;
smooth in vec3 l;

out vec4 out_color;

void main() {
    vec3 h = normalize(v+l);
    vec3 n = normalize(n);
    
	vec4 diff = max(0.1f, dot(n, l)) * vec4(diffuse, 1.0);
    vec4 spec = pow(max(0.0f, dot(n, h)), shininess) * vec4(specular, 1.0);

    out_color = diff + spec + vec4(ambient, 1.0);
}