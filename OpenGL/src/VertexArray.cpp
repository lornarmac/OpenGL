#include "VertexArray.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
	//GLCall(glBindVertexArray(m_RendererID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer & vb, VertexBufferLayout & layout)
{
	Bind();
	vb.Bind();
	const auto &elements = layout.GetElements();
	unsigned int offset = 0;

	for(unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		// specify layout of the vertex buffer (so opengl knows how to interpret the data):
		// enable index 0 of the vertex arrays (we only have one array) 
		GLCall(glEnableVertexAttribArray(i));
		// "bind index 0 of the vertex array (which in our sample code only has 1 array) to the currently bound vertex buffer" (...with this layout?)
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalised, layout.GetStride(), (const void*) offset));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}

}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}
