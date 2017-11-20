#version 330


uniform sampler2D flowBuffer;

in vec2 sampleCoords;

layout(location = 0) out vec4 fragColor;


void main()
{
	vec2 flow = texture(flowBuffer, sampleCoords).rg; //in [-1,1]x[-1,1]
    fragColor = vec4(flow*0.5+0.5, 0, 1);
}
