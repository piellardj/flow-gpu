#version 330
#define PI 3.14159265359


uniform sampler2D flowBuffer;

uniform float arrowSize;

layout(location = 0) in vec2 vert;
layout(location = 1) in vec2 pos;


vec2 rotateVector(vec2 v, float c, float s)
{
	return vec2(v.x * c - v.y * s,
				v.x * s + v.y * c);
}

vec2 rotateVector(vec2 v, float radAngle)
{
	float c = cos(radAngle), s = sin(radAngle);
	return rotateVector(v, c, s);
}

void main()
{
	vec2 flow = texture(flowBuffer, 0.5*pos + 0.5).rg;
	float strength = length(flow);
	flow = flow / strength;
	float c = flow.x, s = flow.y;
	
	vec2 size = arrowSize * vec2(strength);
	gl_Position = vec4(pos + rotateVector(size*vert, c, s), 0, 1);
}