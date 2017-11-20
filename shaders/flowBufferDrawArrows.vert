#version 330


uniform vec2 screenSize; //in pixels
uniform vec2 arrowSize; //in pixels

uniform sampler2D flowBuffer;

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
	
	vec2 localVert = strength * arrowSize * vert; //in pixels
	gl_Position = vec4(pos + rotateVector(localVert, c, s) / screenSize, 0, 1);
}