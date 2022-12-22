#pragma once

#include <string>

#include "Structs.h"

class Texture {
private:
	int m_Width, m_Height, m_BPP;
	void* m_Data;
	size_t m_DataSize;

public:
	Texture(const std::string& path);
	~Texture();

	unsigned int PictureDescriptor;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};