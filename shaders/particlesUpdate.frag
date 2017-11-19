#version 330
#define PI 3.14159265359
#define LIFETIME 5.0
#define BIRTHTIME 1.0

uniform sampler2D previousPosBuffer;
uniform sampler2D previousLooksBuffer;
uniform sampler2D initPosBuffer;
uniform sampler2D birthdateBuffer;
uniform sampler2D colorBuffer;

uniform sampler2D flowMap;

uniform float dt;
uniform float clock;

layout(location = 0) out vec4 nextPosValue;
layout(location = 1) out vec4 nextLooksValue;
layout(location = 2) out vec4 nextBirthdateValue;

void main()
{
	ivec2 texelCoords = ivec2(gl_FragCoord.x, gl_FragCoord.y);
	vec2 prevPos = texelFetch(previousPosBuffer, texelCoords, 0).rg;
	vec4 prevLooks = texelFetch(previousLooksBuffer, texelCoords, 0);
	float prevBirthdate = texelFetch(birthdateBuffer, texelCoords, 0).r;
	
	vec3 nextColor = prevLooks.rgb;
	
	float nextBirthdate = prevBirthdate;
	if (clock - prevBirthdate > LIFETIME) {
		prevPos = texelFetch(initPosBuffer, texelCoords, 0).rg;
		nextColor = texture(colorBuffer, prevPos).rgb;
		nextBirthdate = clock;
	}
	
	vec2 flow = texture(flowMap, prevPos).rg;
	vec2 nextPos = prevPos + 0.02*dt*flow;
	float nextOrientation = atan(flow.y, flow.x) / (2.0 * PI) + 0.5;
	
	nextPosValue = vec4(nextPos, 0, 0);
	nextLooksValue = vec4(nextColor, nextOrientation);
	nextBirthdateValue = vec4(nextBirthdate, 0, 0, 1);
}
