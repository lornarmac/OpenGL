#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

// ties together a vertex buffer (just bytes ) with it's layout
class VertexArray
{
private:
	unsigned int m_RendererID;		// opengl id

public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, VertexBufferLayout& layout);

	void Bind() const;
	void Unbind() const;

};