#version 330

uniform vec2 brushSize; //normalized
uniform vec2 brushPos; //normalized

out vec2 toBrush; //in the brush' system, [-1,1]x[-1,1] is inside
out vec2 sampleCoord; //in the buffer's system, in [0,1]x[0,1]

const vec2 corners[4] = vec2[](vec2(-1,-1),
                               vec2(+1,-1),
                               vec2(-1,+1),
                               vec2(+1,+1));

void main()
{
	vec2 corner = corners[gl_VertexID];
	
	toBrush = brushPos - (0.5*corner + 0.5);
	toBrush *= 2.0 / brushSize; // in the brush' system, [-1,1]x[-1,1] is inside
	
	sampleCoord = corner * 0.5 + 0.5;
	
	gl_Position = vec4(corner, 0, 1);
}