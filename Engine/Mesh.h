#pragma once
#include <vector>
#include <utility>
#include "MathCommon.h"
#include <cassert>

namespace ArkanoidEngine
{
	class Mesh
	{
	public:
		//ctors
		explicit Mesh() = default;

		//dtor
		~Mesh() = default;

		//copy
		Mesh(const Mesh&) = default;
		Mesh& operator=(const Mesh&) = default;

		//move
		Mesh(Mesh&&) = default;
		Mesh& operator=(Mesh&&) = default;

		using PositionType = XMFLOAT3;
		using NormalType = XMFLOAT3;
		using TextCoordType = XMFLOAT2;

		void setPositions(PositionType* positions, size_t count);
		void setIndices(uint64_t* indices, size_t count);
		void setNormals(NormalType* normals, size_t count);
		void setTextCoords(TextCoordType* textCoords, size_t count);

		void setPositions(std::vector<PositionType>&& positions);
		void setIndices(std::vector<uint64_t>&& indices);
		void setNormals(std::vector<NormalType>&& normals);
		void setTextCoords(std::vector<TextCoordType>&& textCoords);

		template<typename It>
		void setPositions(It beg, It end);
		template<typename It>
		void setIndices(It beg, It end);
		template<typename It>
		void setNormals(It beg, It end);
		template<typename It>
		void setTextCoords(It beg, It end);

		std::vector<PositionType>& getPositions();
		std::vector<uint64_t>& getIndices();
		std::vector<NormalType>& getNormals();
		std::vector<TextCoordType>& getTextCoords();

		const std::vector<PositionType>& getPositions() const;
		const std::vector<uint64_t>& getIndices() const;
		const std::vector<NormalType>& getNormals() const;
		const std::vector<TextCoordType>& getTextCoords() const;

		bool hasPositions()const;
		bool hasTextCoords()const;
		bool hasNormals()const;
		bool hasIndices()const;

		size_t vertexCount()const;
		size_t indexCount()const;

		//removes all vertices, texture coordinates, normals and indices
		void clear();
		
	private:

		template<typename T, typename It>
		void set(std::vector<T>& dest, It beg, It end);

		template<typename T>
		void set(std::vector<T>& dest, T* src, size_t count);

		std::vector<uint64_t> m_indices{};
		std::vector<PositionType> m_positions{};
		std::vector<NormalType> m_normals{};
		std::vector<TextCoordType> m_textCoords{};
	};

	template<typename It>
	inline void Mesh::setPositions(It beg, It end)
	{
		set(m_positions, beg, end);
	}

	template<typename It>
	inline void Mesh::setIndices(It beg, It end)
	{
		set(m_indices, beg, end);
	}

	template<typename It>
	inline void Mesh::setNormals(It beg, It end)
	{
		set(m_normals, beg, end);
	}

	template<typename It>
	inline void Mesh::setTextCoords(It beg, It end)
	{
		set(m_textCoords, beg, end);
	}

	inline void Mesh::setPositions(PositionType* positions, size_t count)
	{
		set(m_positions, positions, count);
	}

	inline void Mesh::setIndices(uint64_t* indices, size_t count)
	{
		set(m_indices, indices, count);
	}

	inline void Mesh::setNormals(NormalType* normals, size_t count)
	{
		set(m_normals, normals, count);
	}

	inline void Mesh::setTextCoords(TextCoordType* textCoords, size_t count)
	{
		set(m_textCoords, textCoords, count);
	}

	inline void Mesh::setPositions(std::vector<PositionType>&& positions)
	{
		m_positions = std::move(positions);
	}

	inline void Mesh::setIndices(std::vector<uint64_t>&& indices)
	{
		m_indices = std::move(indices);
	}

	inline void Mesh::setNormals(std::vector<NormalType>&& normals)
	{
		m_normals = std::move(normals);
	}

	inline void Mesh::setTextCoords(std::vector<TextCoordType>&& textCoords)
	{
		m_textCoords = std::move(textCoords);
	}

	inline std::vector<Mesh::PositionType>& Mesh::getPositions()
	{
		return m_positions;
	}

	inline const std::vector<Mesh::PositionType>& Mesh::getPositions() const
	{
		return m_positions;
	}

	inline std::vector<uint64_t>& Mesh::getIndices()
	{
		return m_indices;
	}

	inline const std::vector<uint64_t>& Mesh::getIndices() const
	{
		return m_indices;
	}

	inline std::vector<Mesh::NormalType>& Mesh::getNormals()
	{
		return m_normals;
	}

	inline const std::vector<Mesh::NormalType>& Mesh::getNormals() const
	{
		return m_normals;
	}

	inline std::vector<Mesh::TextCoordType>& Mesh::getTextCoords()
	{
		return m_textCoords;
	}

	inline const std::vector<Mesh::TextCoordType>& Mesh::getTextCoords() const
	{
		return m_textCoords;
	}

	inline void Mesh::clear()
	{
		m_positions.clear();
		m_indices.clear();
		m_normals.clear();
	}

	inline bool Mesh::hasPositions()const
	{
		return m_positions.size() > 0;
	}

	inline bool Mesh::hasTextCoords()const
	{
		return m_textCoords.size() > 0;
	}

	inline bool Mesh::hasNormals()const
	{
		return m_normals.size() > 0;
	}

	inline bool Mesh::hasIndices()const
	{
		return m_indices.size() > 0;
	}

	inline size_t Mesh::vertexCount()const
	{
		return m_positions.size();
	}

	inline size_t Mesh::indexCount()const
	{
		return m_indices.size();
	}

	template<typename T, typename It>
	inline void Mesh::set(std::vector<T>& dest, It beg, It end)
	{
		dest.clear();
		dest.assign(beg, end);
	}

	template<typename T>
	inline void Mesh::set(std::vector<T>& dest, T* src, size_t count)
	{
		assert(src != nullptr);
		dest.clear();
		dest.reserve(count);
		for (size_t i = 0; i < count; ++i)
		{
			dest.push_back(src[i]);
		}			
	}

}