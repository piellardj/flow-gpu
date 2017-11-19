#version 330
#define THICKNESS 0.03


uniform vec2 brushSize; //normalized
uniform vec2 brushPos; //normalized

out vec2 toBrush; //in the brush' system, [-1,1]x[-1,1] is inside
flat out float thickness;


const vec2 corners[4] = vec2[](vec2(-1,-1),
                               vec2(+1,-1),
                               vec2(-1,+1),
                               vec2(+1,+1));

void main()
{
	vec2 corner = corners[gl_VertexID];
	
	vec2 v = brushPos + 0.5*brushSize*corner;
	toBrush = -corner;
	
	thickness = THICKNESS / brushSize.x;
	
	gl_Position = vec4(2.0*v - 1.0, 0, 1);
}