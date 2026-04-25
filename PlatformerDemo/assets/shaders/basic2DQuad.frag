#version 330 core

out vec4 outColor;

in vec4 tileWorldPos;
in vec2 vertexTex;

uniform sampler2D u_Texture;
uniform vec2 u_PlayerPos;

float innerCut = radians(75);
float outerCut = radians(400);
float epsilon = innerCut - outerCut;

void main()
{
	vec2 worldPos = tileWorldPos.xy;
	float dist = distance(worldPos, u_PlayerPos);

	vec4 finalColor = texture(u_Texture, vertexTex);

	float lightIntensity = clamp((dist-outerCut)/epsilon, 0.0, 1.0);
	vec4 lightColor = vec4(1) * lightIntensity;

	outColor = finalColor * finalColor;
}