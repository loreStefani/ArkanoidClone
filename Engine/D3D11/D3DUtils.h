#pragma once
#include <comdef.h>
#include <cassert>
namespace ArkanoidEngine
{
	namespace D3D11
	{
		inline void checkHRESULT(HRESULT hr)
		{
			if (FAILED(hr))
			{
				_com_error comError{ hr };
				OutputDebugString(comError.ErrorMessage());
				assert(false);
			}
		}

#define COM_RELEASE(x) if(x != nullptr) { x->Release(); x = nullptr;}
#define HRESULT_CHECK(hr) checkHRESULT((hr));

	}
}