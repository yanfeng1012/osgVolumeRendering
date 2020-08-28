#version 110

uniform float stepSize;
uniform sampler3D baseTexture;

varying vec4 cameraPos;
varying vec4 vertexPos;


void main(void)
{
	gl_Position = ftransform();

	cameraPos = gl_ModelViewMatrixInverse*vec4(0, 0, 0, 1);
	vertexPos = gl_Vertex;
}
