#version 130

in  vec3  pos;
in  vec2  uv;

out vec2  vuv;

uniform vec2 offset;
uniform vec2 scale;

void main() {
	vec3 position = pos;
	position.xy *= scale;
	position.xy += offset;
	vuv = uv;
	gl_Position = vec4(position,1);
}
