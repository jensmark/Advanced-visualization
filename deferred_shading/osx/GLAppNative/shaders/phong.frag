#version 150

uniform vec3 diffuse;
uniform vec3 specular;
uniform vec3 ambient;
uniform float shininess;
uniform vec3 light_pos;

uniform sampler2D color;
uniform sampler2D normal;
uniform sampler2D view;

smooth in vec2 uv;

out vec4 out_color;

void main() {
    vec4 c = texture(color, uv);
    vec3 n = texture(normal, uv).xyz;
    vec3 v = texture(view, uv).xyz;
    vec3 l = normalize(light_pos + v);
    
    vec3 h = normalize(v+l);
    n = normalize(n);
    
	vec4 diff = max(0.1f, dot(n, l)) * vec4(diffuse, 1.0) * c;
    vec4 spec = pow(max(0.0f, dot(n, h)), shininess) * vec4(specular, 1.0);

    out_color = (diff + spec + vec4(ambient, 1.0));
}