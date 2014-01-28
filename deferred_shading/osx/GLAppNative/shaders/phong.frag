#version 150

uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;
uniform vec3 light_pos;

uniform mat4 view_matrix;

uniform sampler2D color;
uniform sampler2D normal;
uniform sampler2D view;

out vec4 out_color;

void main() {
    ivec2 window = textureSize(color, 0);
    vec2 uv = vec2(gl_FragCoord.x / (window.x*2),
                    gl_FragCoord.y / (window.y*2));
    
    vec4 c = texture(color, uv);
    vec3 n = texture(normal, uv).xyz;
    vec3 v = texture(view, uv).xyz;
    
    vec3 light_pos = vec3(view_matrix*vec4(light_pos,1.0));
    vec3 l = normalize(light_pos - v);
    
    vec3 h = normalize(-v+l);
    n = normalize(n);
    
	vec4 diff = max(0.0f, dot(n, l)) * (vec4(diffuse, 1.0)*c);
    vec4 spec = pow(max(0.0f, dot(n, h)), shininess) * vec4(specular, 1.0);

    float dist_2d = distance (light_pos, v);
    float atten_factor = -log (dist_2d /0.8);
    
    out_color = ((diff + spec) * atten_factor);
}