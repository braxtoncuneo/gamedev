#version 130

in  vec2 vuv;

out vec4 pColor;

uniform float time;
uniform sampler2D the_texture;

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


bool on_grid(vec2 point, float freq){
	bool result = false;
	if        (fract(point.x/freq) < 0.1) {
		result = true;
	} else if (fract(point.y/freq) < 0.1) {
		result = true;
	}
	return result;
}


vec3 grid(vec2 uv) {
	vec3 result = vec3(1,1,1);
	if(on_grid(uv,0.05)){
		result = vec3(1,0,0);
	}
	return result;
}


vec3 circles(vec2 uv) {
	vec3 result = vec3(1,1,1);
	uv = fract(uv*10);
	uv = (uv*2.0)-1.0;
	if(length(uv) < 0.5){
		result = vec3(0,0,1);
	}
	return result;
}


vec3 rays(vec2 uv) {
	vec3 result = vec3(1,1,1);
	uv = fract(uv*10);
	uv = (uv*2.0)-1.0;
	float angle = atan(uv.x,uv.y);
	if(fract(angle*3.1415926) < 0.5){
		result = vec3(0,1,0);
	}
	return result;
}

vec3 lookup(vec2 uv) {
	return texture(the_texture, uv).rgb;
}


void main() {
	vec2 uv = vuv;
	//uv.t += time;
	//vec3 color = lookup(uv);
	vec3 color = circles(uv);
	pColor = vec4(color,1);
}

