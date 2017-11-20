#version 330


uniform sampler2D background;
uniform uint layer;

in vec2 sampleCoords;


layout(location = 0) out vec4 fragColor;


void main()
{
	float alpha = texture(background, sampleCoords).a;
	if (alpha < 0.05)
		discard;
	
	fragColor = vec4(float(layer) / 255.0);
	
	gl_FragDepth = 1.0 / float(layer+ 2u);
}
