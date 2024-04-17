#version 130


in  vec3 point;

out vec3 model_coord;
out vec3 world_coord;
out vec3 view_coord;

uniform float time;

uniform mat4  modl_transform;
uniform mat4  view_transform;
uniform mat4  proj_transform;



void main() {
	
	vec4 model = vec4(point,1);
	vec4 world = modl_transform * model;
	vec4 cam   = view_transform * world;

	vec4 proj  = proj_transform * cam;

	model_coord = model.xyz;
	world_coord = world.xyz;
	view_coord  = cam.xyz;
	gl_Position = proj;
}

