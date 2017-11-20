#version 330


uniform sampler2D previousBuffer;
uniform sampler2D initBuffer;

uniform float factor;

in vec2 sampleCoord;

layout(location = 0) out vec4 fragColor;


void main()
{
	vec2 previous = texture(previousBuffer, sampleCoord).rg;
	vec2 init = texture(initBuffer, sampleCoord).rg;
	vec2 new = mix(previous, init, clamp(factor, 0, 1));

	fragColor = vec4(new, 0, 1);
}
