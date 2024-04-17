#version 130

in  vec3  pos;
in  vec2  uv;

out vec2  vuv;

void main() {
	vec3 position = pos;
	vuv = uv;
	gl_Position = vec4(position,1);
}

