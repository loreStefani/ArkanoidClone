#pragma once

namespace ArkanoidEngine
{
	class DepthState
	{
	public:
		//ctors
		explicit DepthState(bool depthTestEnabled = true);

		//dtor
		~DepthState() = default;

		//copy
		DepthState(const DepthState&) = default;
		DepthState& operator=(const DepthState&) = default;

		//move
		DepthState(DepthState&&) = default;
		DepthState& operator=(DepthState&&) = default;

		bool depthTestEnabled()const;

		void setDepthTestEnabled(bool depthTestEnabled);

	private:
		bool m_depthTestEnabled;
	};

	inline DepthState::DepthState(bool depthTestEnabled)
		: m_depthTestEnabled{ depthTestEnabled }
	{
	}

	inline bool DepthState::depthTestEnabled()const
	{
		return m_depthTestEnabled;
	}

	inline void DepthState::setDepthTestEnabled(bool depthTestEnabled)
	{
		m_depthTestEnabled = depthTestEnabled;
	}
}
