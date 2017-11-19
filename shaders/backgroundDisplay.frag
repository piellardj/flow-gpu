#version 330


uniform sampler2D backgroundTexture;

uniform float brightness;

in vec2 sampleCoords;

layout(location = 0) out vec4 fragColor;


void main()
{
	vec4 texel = texture(backgroundTexture, sampleCoords);
	
	fragColor = vec4(brightness*texel.rgb, texel.a);
}
