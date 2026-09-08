#version 330 core

out vec4 outColor;

in vec4 tileWorldPos;
in vec2 vertexTex;

uniform sampler2D u_Texture;
uniform vec2 u_PlayerPos;

float innerCut = radians(50);
float outerCut = radians(470);
float epsilon = innerCut - outerCut;

void main()
{
	vec2 worldPos = tileWorldPos.xy;
	float dist = distance(worldPos, u_PlayerPos);

	vec4 finalColor = texture(u_Texture, vertexTex);

	float lightIntensity = clamp((dist-outerCut)/epsilon, 0.0, 1.0);
	vec4 lightColor = vec4(0.741, 0.509, 0.482, 0.9) * lightIntensity;

	outColor = finalColor * lightColor;
}