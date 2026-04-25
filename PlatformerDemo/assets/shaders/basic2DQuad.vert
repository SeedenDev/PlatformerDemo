#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;

out vec4 tileWorldPos;
out vec2 vertexTex;

uniform mat4 u_MVP = mat4(1);
uniform mat4 u_Model = mat4(1);

void main()
{
	tileWorldPos = u_Model * vec4(pos, 0, 1);
	vertexTex = tex;
	gl_Position = u_MVP * vec4(pos.x, pos.y, 0, 1);
}