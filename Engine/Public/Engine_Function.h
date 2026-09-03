#ifndef Engine_Function_h__
#define Engine_Function_h__

#include "Engine_Typedef.h"
#include "Engine_Struct.h"

namespace Engine
{
	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}
	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete[] Pointer;
			Pointer = nullptr;
		}
	}
}

#endif // Engine_Function_h__
