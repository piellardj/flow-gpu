#version 330


uniform sampler2D backgroundTexture;

uniform float alpha;

in vec2 sampleCoords;

layout(location = 0) out vec4 fragColor;


void main()
{
	vec4 texel = texture(backgroundTexture, sampleCoords);
	
	fragColor = vec4(texel.rgb, alpha * texel.a);
}
