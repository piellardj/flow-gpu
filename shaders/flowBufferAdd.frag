#version 330

uniform sampler2D previousBuffer;

uniform vec2 flow;

in vec2 toBrush; //in the brush' system, [-1,1]x[-1,1] is inside
in vec2 sampleCoord; //in the buffer's system, in [0,1]x[0,1]

layout(location = 0) out vec4 fragColor;


float influenceFunction1(float sqDist)
{
	return 1.0 - step(1.0, sqDist);
}

float influenceFunction2(float sqDist)
{
	sqDist = clamp(sqDist, 0.0, 1.0);
	
	float factor = exp(1.0 - (1.0/(1.0-sqDist)));
	factor = pow(factor, 0.1);
	return factor;
}

void main()
{
	float sqDist = dot(toBrush,toBrush); //square distance to the brush, < 1 if inside

	float factor = influenceFunction1(sqDist);

	vec2 previousFlow = texture(previousBuffer, sampleCoord).rg;
	
	vec2 nextFlow = previousFlow + 20.0 * factor * flow;
	if (dot(nextFlow, nextFlow) > 1.0)
		nextFlow = normalize(nextFlow);

    fragColor = vec4(nextFlow, 0, 1);
}
