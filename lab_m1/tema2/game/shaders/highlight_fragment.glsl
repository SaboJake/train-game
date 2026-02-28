#version 330

in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture_coord;
in vec3 frag_color;

layout(location = 0) out vec4 out_color;

void main()
{
    vec3 highlight_color = vec3(0.0, 1.0, 0.0);
    vec3 final_color = mix(frag_color, highlight_color, 0.5);
    
    out_color = vec4(final_color, 1.0);
}
