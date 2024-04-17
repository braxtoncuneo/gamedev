#version 130

in  vec2 vuv;

out vec4 pColor;

uniform float     dim;
uniform sampler2D the_texture;

void main() {
	vec3 color = texture(the_texture,vuv).rgb;
	color *= dim;
	pColor =  vec4(color,1);
}

