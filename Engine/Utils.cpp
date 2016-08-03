#include "MemoryCommon.h"
#include "Utils.h"
#include <fstream>
#include <cassert>

using namespace ArkanoidEngine;

static auto istreamSize(std::istream& is)
{
	auto startGetPos = is.tellg();
	is.seekg(0, std::ios::end);

	auto size = is.tellg();

	is.seekg(startGetPos);

	return size;
}

char* Utils::loadFile(const std::string& filePath, size_t* fileSizePtr, bool isBinary)
{
	assert(fileSizePtr != nullptr);

	std::ifstream fileStream{};
	
	if (isBinary)
	{
		fileStream.open(filePath, std::ios::binary);
	}
	else
	{
		fileStream.open(filePath);
	}

	if (!fileStream.is_open())
	{
		return nullptr;
	}

	size_t fileSize = static_cast<size_t>(istreamSize(fileStream));

	char* fileDataBuffer = new(std::nothrow) char[fileSize];
	
	if (fileDataBuffer != nullptr)
	{
		fileStream.read(fileDataBuffer, fileSize);
		*fileSizePtr = fileSize;
	}

	return fileDataBuffer;
}