#version 110

uniform float stepSize;
uniform sampler3D baseTexture;

varying vec4 cameraPos;
varying vec4 vertexPos;


void main(void)
{

	vec3 direction = vertexPos.xyz - cameraPos.xyz;
	direction = normalize(direction);
	vec3 stepSizeTemp = direction*stepSize;

	vec3 startPos = vertexPos.xyz;
	vec4 destColor = vec4(0.0, 0.0, 0.0, 0.0);

	for(int i = 0; i<2048; i++)
	{
		vec4 value = texture3D(baseTexture, startPos);
		destColor.rgb = destColor.rgb*(1.0-value.a) + value.rgb*value.a;
		destColor.a += value.a;

		startPos += stepSizeTemp;
		
		if(destColor.a>1.0) 
		{
			destColor.a = 1.0;
			break;
		}
		if((startPos.x <0.0) || (startPos.x>1.0)) break;
		if((startPos.y <0.0) || (startPos.y>1.0)) break;
		if((startPos.z <0.0) || (startPos.z>1.0)) break;

	}

	gl_FragColor = destColor;

}
