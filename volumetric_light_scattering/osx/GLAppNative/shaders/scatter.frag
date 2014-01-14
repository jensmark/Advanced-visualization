#version 150

uniform vec3 screen_light_pos;
uniform int n_samples;
uniform float density;
uniform float weight;
uniform float decay;
uniform float exposure;

uniform sampler2D prepass;
uniform sampler2D frame;

smooth in vec2 uv;

out vec4 out_color;

void main() {
    vec2 duv = (uv-screen_light_pos.xy);
    duv *= 1.0f/float(n_samples)*density;
    
    vec3 c = texture(prepass, uv).xyz;
    float illumDec = 1.0f;
    
    vec2 suv = uv;
    for (int i = 0; i < n_samples; i++) {
        suv -= duv;
        vec3 samp = texture(prepass, suv).xyz;
        
        samp *= illumDec * weight;
        c += samp;
        illumDec *= decay;
    }
    
    out_color = vec4(c * exposure, 1.0f) + texture(frame, uv);
}