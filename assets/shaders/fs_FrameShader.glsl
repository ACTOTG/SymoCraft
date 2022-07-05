#version 460 core

layout(early_fragment_tests) in; //Built-in Optimization

out vec4 o_frag_color;

void main()
{
    o_frag_color = vec4(1.0f, 0.98f, 0.98f, 1.0f);
}
