#include "MemoryCommon.h"
#include "Image.h"
#include "third_party/stb_image.h"
#include "Resources.h"
#include <cassert>

using namespace ArkanoidEngine;

Image Image::loadImageFromFile(const std::string& imageFileName)
{
	int width = 0;
	int height = 0;
	int componentsCount = 0;

	unsigned char* imageData = stbi_load((gk_texturesPath + imageFileName).c_str(), &width, &height, &componentsCount, 4);

	unsigned int uWidth = 0;
	unsigned int uHeight = 0;

	if (imageData != nullptr)
	{
		uWidth = static_cast<unsigned int>(width);
		uHeight = static_cast<unsigned int>(height);
	}
	
	return Image{ imageData, uWidth, uHeight };
}

Image::Image(unsigned char* imageData, unsigned int width, unsigned int height)
	: m_imageData{ imageData }, m_width{ width }, m_height{ height }
{
}

Image::~Image()
{
	delete[] m_imageData; //could be nullptr
}

Image::Image(Image&& other)
	: m_imageData{ other.m_imageData }, m_width{ other.m_width }, m_height{ other.m_height }
{
	other.m_imageData = nullptr;
	other.m_width = 0;
	other.m_height = 0;
}

Image& Image::operator=(Image&& rhs)
{
	if (&rhs == this)
	{
		return *this;
	}

	delete[] m_imageData; //could be nullptr

	m_imageData = rhs.m_imageData;
	m_width = rhs.m_width;
	m_height = rhs.m_height;

	rhs.m_imageData = nullptr;
	rhs.m_width = 0;
	rhs.m_height = 0;

	return *this;
}