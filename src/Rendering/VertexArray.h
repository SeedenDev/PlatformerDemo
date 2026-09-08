#pragma once

#include <glad/glad.h>
#include "VertexBuffer.h"
#include "VertexLayout.h"

class VertexArray
{
private:
	unsigned int m_HandlerID = 0;

public:
	VertexArray();
	~VertexArray();

	void ApplyLayout(VertexBuffer& vbo, VertexLayout& vLayout);
	void Bind() const;
	void Unbind() const;
};