#pragma once
#include <limits>
namespace ArkanoidEngine
{
	class IDType
	{
	public:
		using NativeIDType = size_t;

		//ctors
		explicit IDType() = default;
		explicit IDType(NativeIDType nativeID);

		//dtor
		~IDType() = default;

		//copy
		IDType(const IDType&) = default;
		IDType& operator=(const IDType&) = default;

		//move
		IDType(IDType&&) = default;
		IDType& operator=(IDType&&) = default;
		
		bool isValid()const;
		NativeIDType nativeID()const;

	private:
		static constexpr NativeIDType sk_invalidID = std::numeric_limits<NativeIDType>::max();

		NativeIDType m_nativeID{ sk_invalidID };
	};

	inline IDType::IDType(NativeIDType nativeID) : m_nativeID{ nativeID }
	{
	}

	inline IDType::NativeIDType IDType::nativeID()const
	{
		return m_nativeID;
	}

	inline bool IDType::isValid()const
	{
		return m_nativeID != sk_invalidID;
	}

	//operators

	inline bool operator==(IDType a, IDType b)
	{
		return a.nativeID() == b.nativeID();
	}

	inline bool operator!=(IDType a, IDType b)
	{
		return !(a == b);
	}

	inline bool operator<(IDType a, IDType b)
	{
		return a.nativeID() < b.nativeID();
	}

	inline bool operator>(IDType a, IDType b)
	{
		return b < a;
	}
}

//allows the use of ArkanoidEngine::IDType as key type in std map types
namespace std
{
	template<> struct hash<ArkanoidEngine::IDType>
	{
		using argument_type = ArkanoidEngine::IDType;
		using result_type = std::size_t;

		result_type operator()(argument_type const& s) const
		{
			return std::hash<argument_type::NativeIDType>()(s.nativeID());
		}
	};
}