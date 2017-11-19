#version 330

uniform sampler2D strokeTexture;

uniform uint level;

in vec2 sampleCoords;
flat in vec4 color;

layout(location = 0) out vec4 fragColor;


void main()
{
	float alpha = texture(strokeTexture, sampleCoords).r;
	if (alpha < 0.5)
		discard;
	
    fragColor = vec4(color.rgb, 1);
	gl_FragDepth = 1.0 / float(level + 2u);
}
