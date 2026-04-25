#pragma once

#include <GL/glew.h>

class VertexBuffer
{
private:
	unsigned int m_HandlerID = 0;

public:
	VertexBuffer(const float* vertices, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};