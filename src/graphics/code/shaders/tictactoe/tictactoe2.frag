#version 130

in  vec2 vuv;

out vec4 pColor;


void main() {
	vec2 uv = vuv;
	pColor = vec4(uv,0,1);	
}

