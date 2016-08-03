#pragma once
#include <string>

namespace ArkanoidEngine
{
	namespace Utils
	{
		inline std::wstring makeWideString(const std::string& s)
		{
			return std::wstring{ s.begin(), s.end() };
		}

		//returns a pointer to a heap allocated buffer that needs to be deleted. 
		//returns nullptr upon failure.
		//fileSizeOutPtr is used to output the size of the buffer. it is untouched in case of failure.
		char* loadFile(const std::string& filePath, size_t* fileSizeOutPtr, bool isBinary = false);
	}
}