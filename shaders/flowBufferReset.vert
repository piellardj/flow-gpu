#version 330


out vec2 sampleCoord;


const vec2 corners[4] = vec2[](vec2(-1,-1),
                               vec2(+1,-1),
                               vec2(-1,+1),
                               vec2(+1,+1));

void main()
{
	vec2 corner = corners[gl_VertexID];
	sampleCoord = 0.5 * corner + 0.5;
	gl_Position = vec4(corner, 0, 1);
}