#version 130

in  vec3 model_coord;
in  vec3 world_coord;
in  vec3 view_coord;

out vec4 pColor;

uniform float time;
uniform bool show_model;
uniform bool show_world;
uniform bool show_view;


bool on_grid(vec3 point, float freq){
	bool result = false;
	if        (fract(point.x/freq) < 0.1) {
		result = true;
	} else if (fract(point.y/freq) < 0.1) {
		result = true;
	} else if (fract(point.z/freq) < 0.1) {
		result = true;
	}
	return result;
}


void main() {
	vec2 screen_coord = gl_FragCoord.xy / 400.0;
	vec3 color = vec3(1,1,1);
	if(on_grid(model_coord,0.5) && show_model){
		color.r = 0.0;
	}
	if(on_grid(world_coord,0.5) && show_world){
		color.g = 0.0;
	}
	if(on_grid(view_coord,0.5) && show_view){
		color.b = 0.0;
	}
	pColor = vec4(color,1);	
}

