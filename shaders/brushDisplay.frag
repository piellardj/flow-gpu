#version 330


in vec2 toBrush; //in the brush' system, [-1,1]x[-1,1] is inside
flat in float thickness;

layout(location = 0) out vec4 fragColor;


void main()
{
	float dist = length(toBrush);
	
	if (dist < 1.0-thickness || dist > 1.0)
		discard;
	
	float distToRing = distance(dist, 1.0 - 0.5*thickness);
	
	float c = smoothstep(1.0-thickness, 1.0, dist);//1.0 - smoothstep(0.0, 0.5*thickness, distToRing);
    fragColor = vec4(1, 1, 1, c);
}
