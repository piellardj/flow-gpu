#version 330


uniform bool onlyRing=false;

in vec2 toBrush; //in the brush' system, [-1,1]x[-1,1] is inside
flat in float thickness;

layout(location = 0) out vec4 fragColor;


void main()
{
	float dist = length(toBrush);
	float c = 1.0;
	
	if (onlyRing) {
		if (dist < 1.0-thickness || dist > 1.0)
			discard;
		
		float distToRing = distance(dist, 1.0 - 0.5*thickness);
		
		c = smoothstep(1.0-thickness, 1.0, dist);
	}
	else {
		c = 1.0 - smoothstep(0, 1, dist);
	}
	
	
    fragColor = vec4(1, 1, 1, c);
}
