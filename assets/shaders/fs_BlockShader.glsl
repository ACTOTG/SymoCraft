#version 460 core

layout(early_fragment_tests) in; //Built-in Optimization

in vec3 o_tex_coord; // ** z coord is the layer index for the texture array, starting from 0 **

out vec4 frag_color;

uniform sampler2DArray u_texture_array;

void main()
{
	frag_color = texture(u_texture_array, o_tex_coord);
}
