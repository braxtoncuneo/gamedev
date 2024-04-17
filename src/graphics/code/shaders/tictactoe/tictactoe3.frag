#version 130

in  vec2 vuv;

out vec4 pColor;

uniform sampler2D the_texture;

void main() {
	pColor =  vec4(texture(the_texture, vuv).rgb,1);
}
