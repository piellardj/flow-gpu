#version 330


uniform sampler2D backgroundTexture;

uniform float brightness;

in vec2 sampleCoords;

layout(location = 0) out vec4 fragColor;


void main()
{
	vec3 color = texture(backgroundTexture, sampleCoords).rgb;
	
	fragColor = vec4(brightness*color, 1);
}
