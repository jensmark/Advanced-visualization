#version 150

uniform mat4 view_projection_inverse_matrix;
uniform mat4 view_projection_prev_matrix;

uniform int n_samples;

uniform sampler2D frame;
uniform sampler2D depths;

smooth in vec2 uv;

out vec4 out_color;

void main() {
    float zow   = texture(depths, uv).x;
    vec4 H      = vec4(uv.x*2.0f-1.0f, (1.0f-uv.y)*2.0f-1.0f, zow, 1.0f);
    vec4 D      = view_projection_inverse_matrix * H;
    vec4 pos    = D / D.w;
    vec4 cpos   = H;
    vec4 ppos   = view_projection_prev_matrix * pos;
    
    ppos /= ppos.w;
    vec2 vel    = (cpos.xy-ppos.xy)*0.01f;
    vec4 color  = texture(frame, uv);
    vec2 tuv    = uv+vel;
    
    for (int i = 1; i < n_samples; i++) {
        vec4 ccolor = texture(frame, tuv);
        color += ccolor;
        tuv += vel;
    }
    out_color = color/float(n_samples);
}