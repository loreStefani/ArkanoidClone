#pragma once
#include <cassert>

namespace ArkanoidGame
{
	class TextureTileInfo
	{
	public:
		//ctors
		explicit TextureTileInfo(float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f);

		//dtor
		~TextureTileInfo() = default;

		//copy
		TextureTileInfo(const TextureTileInfo&) = default;
		TextureTileInfo& operator=(const TextureTileInfo&) = default;

		//move
		TextureTileInfo(TextureTileInfo&&) = default;
		TextureTileInfo& operator=(TextureTileInfo&&) = default;

		void transformToUVSpace(unsigned int atlasWidth, unsigned int atlasHeight);

		float x()const;
		float y()const;
		float width()const;
		float height()const;

		void setX(float x);
		void setY(float y);
		void setWidth(float width);
		void setHeight(float height);

	private:
		float m_x;
		float m_y;
		float m_width;
		float m_height;
	};

	inline TextureTileInfo::TextureTileInfo(float x, float y, float width, float height)
		: m_x{ x }, m_y{ y }, m_width{ width }, m_height{ height }
	{
	}

	inline void TextureTileInfo::transformToUVSpace(unsigned int atlasWidth, unsigned int atlasHeight)
	{
		assert(atlasWidth > 0 && atlasHeight > 0);

		m_x /= atlasWidth;
		m_y /= atlasHeight;
		m_width /= atlasWidth;
		m_height /= atlasHeight;
	}

	inline float TextureTileInfo::x()const
	{
		return m_x;
	}

	inline float TextureTileInfo::y()const
	{
		return m_y;
	}

	inline float TextureTileInfo::width()const
	{
		return m_width;
	}

	inline float TextureTileInfo::height()const
	{
		return m_height;
	}

	inline void TextureTileInfo::setX(float x)
	{
		m_x = x;
	}

	inline void TextureTileInfo::setY(float y)
	{
		m_y = y;
	}

	inline void TextureTileInfo::setWidth(float width)
	{
		m_width = width;
	}

	inline void TextureTileInfo::setHeight(float height)
	{
		m_height = height;
	}
}