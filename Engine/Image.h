#pragma once
#include <string>

namespace ArkanoidEngine
{
	//each Image has a fixed format: r8g8b8a8
	class Image
	{
	public:
		//ctors
		//imageData points to a heap-allocated buffer. 
		//The pointer is copied and the memory deleted at destruction time.
		explicit Image(unsigned char* imageData, unsigned int width, unsigned int height);

		//dtor
		~Image();

		//copy
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;

		//move
		Image(Image&&);
		Image& operator=(Image&&);

		unsigned char* imageData();
		const unsigned char* imageData()const;
		unsigned int width()const;
		unsigned int height()const;

		static Image loadImageFromFile(const std::string& imageFileName);

	private:
		unsigned char* m_imageData{ nullptr };
		unsigned int m_width{ 0 };
		unsigned int m_height{ 0 };
	};

	inline unsigned char* Image::imageData()
	{
		return m_imageData;
	}

	inline const unsigned char* Image::imageData()const
	{
		return m_imageData;
	}

	inline unsigned int Image::width()const
	{
		return m_width;
	}
	
	inline unsigned int Image::height()const
	{
		return m_height;
	}
}