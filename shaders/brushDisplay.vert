#version 330


uniform vec2 screenSize; //in pixels

uniform float brushRadius; //in pixels
uniform ivec2 brushPos; //in pixels

out vec2 toBrush; //in the brush's system, [-1,1]x[-1,1] is inside
flat out float thickness; //in the brush's system


const float THICKNESS = 5.0; //in pixels

const vec2 corners[4] = vec2[](vec2(-1,-1),
                              vec2(+1,-1),
                              vec2(-1,+1),
                              vec2(+1,+1));

void main()
{
	thickness = THICKNESS / brushRadius;

	vec2 corner = corners[gl_VertexID];
	toBrush = -corner;
	
	/* Set corner in the brush local system */
	corner = (brushPos + brushRadius*corner) / screenSize;
	gl_Position = vec4(2.0*corner - 1.0, 0, 1);
}