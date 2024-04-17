#version 130

in  vec3  pos;

void main() {
	vec3 position = pos;
	gl_Position = vec4(position,1);
}

