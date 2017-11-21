#version 330

uniform sampler2D previousBuffer;

uniform bool setNull=false;
uniform vec2 movement;

in vec2 toBrush; //in the brush' system, [-1,1]x[-1,1] is inside
in vec2 sampleCoord; //in the buffer's system, in [0,1]x[0,1]

layout(location = 0) out vec4 fragColor;

/* Returns a value between [0,1] */
float influenceFunction(float sqDist)
{
	//return 1.0 - step(1.0, sqDist);
	return 1.0 - smoothstep(0, 1, pow(sqDist, 2.0));
}

void main()
{
	float sqDist = dot(toBrush,toBrush); //square distance to the brush, < 1 if inside

	float factor = influenceFunction(sqDist);

	vec2 previousFlow = texture(previousBuffer, sampleCoord).rg;
	vec2 nextFlow = previousFlow;
	
	if (setNull)
		nextFlow *= 1.0 - factor;
	else
		nextFlow += 20.0 * factor * movement;
		
	/* Normalize if too big */
	nextFlow /= max(1.0, length(nextFlow));

    fragColor = vec4(nextFlow, 0, 1);
}
