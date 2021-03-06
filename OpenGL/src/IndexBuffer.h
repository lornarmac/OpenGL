#pragma once

class IndexBuffer
{
private:
	unsigned int m_RendererID;		// opengl id
	unsigned int m_Count;			// number of indices

public:
	IndexBuffer(const unsigned int *data, unsigned int count);
	~IndexBuffer();
	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const;
};