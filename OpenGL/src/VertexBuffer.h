#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;		// opengl id

public:
	/* param: size in bytes */
	VertexBuffer(const void *data, unsigned int size);
	~VertexBuffer();
	void Bind() const;
	void Unbind() const;

};