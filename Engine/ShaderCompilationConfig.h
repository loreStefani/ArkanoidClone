#pragma once
#include <vector>
#include <string>
#include <utility>

namespace ArkanoidEngine
{	
	class ShaderCompilationConfig
	{
	public:
		//ctors
		explicit ShaderCompilationConfig() = default;

		//dtor
		~ShaderCompilationConfig() = default;

		//copy
		ShaderCompilationConfig(const ShaderCompilationConfig&) = default;
		ShaderCompilationConfig& operator=(const ShaderCompilationConfig&) = default;

		//move
		ShaderCompilationConfig(ShaderCompilationConfig&&) = default;
		ShaderCompilationConfig& operator=(ShaderCompilationConfig&&) = default;

		ShaderCompilationConfig& define(const std::string& name, const std::string& value = "");

		using DefineElement = std::pair<std::string, std::string>;
		using DefinesList = std::vector<DefineElement>;
		
		const DefinesList& defines()const;
		bool hasDefines()const;
		size_t definesCount()const;
		DefinesList::const_iterator begin()const;
		DefinesList::const_iterator end()const;

	private:
		DefinesList m_defines{};
	};	

	inline ShaderCompilationConfig& ShaderCompilationConfig::define(const std::string& name, const std::string& value)
	{
		m_defines.push_back(DefineElement{ name, value });
		return *this;
	}

	inline ShaderCompilationConfig::DefinesList::const_iterator ShaderCompilationConfig::begin() const
	{
		return m_defines.cbegin();
	}

	inline ShaderCompilationConfig::DefinesList::const_iterator ShaderCompilationConfig::end() const
	{
		return m_defines.cend();
	}

	inline const ShaderCompilationConfig::DefinesList& ShaderCompilationConfig::defines() const
	{
		return m_defines;
	}

	inline size_t ShaderCompilationConfig::definesCount()const
	{
		return m_defines.size();
	}

	inline bool ShaderCompilationConfig::hasDefines() const
	{
		return definesCount() > 0;
	}		
}
