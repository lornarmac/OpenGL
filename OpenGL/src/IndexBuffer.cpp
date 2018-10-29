#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer(const unsigned int * data, unsigned int count)
	:m_Count(count)
{
	// assuming we're on platform where unsigned int is 32bytes. Cherno prefers not to use GL specific types in code
	ASSERT(sizeof(unsigned int) == sizeof(GLuint));

 	GLCall(glGenBuffers(1, &m_RendererID));	// get a buffer id
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));		// select the buffer
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));	// assign data to the buffer
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));		// select the buffer
}

void IndexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));		// select the buffer
}

inline unsigned int IndexBuffer::GetCount() const
{
	return m_Count;
}
