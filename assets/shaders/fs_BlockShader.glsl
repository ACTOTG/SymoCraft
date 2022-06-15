#version 460 core

in vec3 o_tex_coord; // ** z coord is the layer index for the texture array, starting from 0 **

out vec4 frag_color;

uniform sampler2DArray u_texture_array;

// float ActualLayer(uint capacity, float layer)
// {
// 	return max(0, min(capacity - 1, floor(layer + 0.5)));
// }

void main()
{
    // frag_color = texture(u_texture_array, vec3(o_tex_coord.st, ActualLayer(256, o_tex_coord.p ) ) );
	frag_color = texture(u_texture_array, vec3(o_tex_coord.st, o_tex_coord.p) );
}
