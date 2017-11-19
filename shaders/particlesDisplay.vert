#version 330
#define PI 3.14159265359
#define LIFETIME 5.0
#define BIRTH_ANIM_TIME 1.0
#define DEATH_ANIM_TIME 1.0

uniform sampler2D posBuffer;
uniform sampler2D looksBuffer;
uniform sampler2D birthdateBuffer;

uniform float clock;

uniform vec2 strokeSize=vec2(0.015,0.003);

layout(location = 0) in ivec2 texelCoords; //where to sample the particles buffer
layout(location = 1) in vec2 corner; //{-1,1}x{-1,1}

out vec2 sampleCoords;
flat out vec4 color;

vec2 rotateVector(vec2 v, float radAngle)
{
	float c = cos(radAngle), s = sin(radAngle);
	return vec2(v.x * c - v.y * s,
				v.x * s + v.y * c);
}

void main()
{
	vec2 center = texelFetch(posBuffer, texelCoords, 0).rg;
	vec4 looks = texelFetch(looksBuffer, texelCoords, 0);
	color = vec4(looks.rgb, 1);
	float orientation = looks.a * 2.0 * PI; //in radians
	
	sampleCoords = 0.5 * corner + 0.5;
	
	float birthdate = texelFetch(birthdateBuffer, texelCoords, 0).r;
	float age = clock - birthdate;
	float birthSizeFactor = smoothstep(0.0, BIRTH_ANIM_TIME, age);
	float deathSizeFactor = smoothstep(0.0, DEATH_ANIM_TIME, LIFETIME-age);
	float sizeFactor = min(birthSizeFactor, deathSizeFactor);
	
	vec2 pos = center + rotateVector(sizeFactor * strokeSize * corner, orientation);
	gl_Position = vec4(2.0*pos - 1.0, 0, 1);
}