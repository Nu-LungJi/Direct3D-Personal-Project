#pragma once
#include "Engine_Define.h"
#include "GameObject.h"

BEGIN(Engine)

template <typename T>
class ENGINE_DLL ObjectPool {
public:
	ObjectPool() = default;
	ObjectPool(size_t _OPSIZE) {
		for (size_t SZ = 0; SZ < _OPSIZE; ++SZ)
			PoolSet.emplace(unique_ptr<T>(new T));

		PoolSize = PoolSet.size();
	}
	~ObjectPool() {
		while (!PoolSet.empty()) {
			unique_ptr<T> OBJ = move(PoolSet.top());
			PoolSet.pop();
			OBJ.reset();
		}
	}

	using  PoolPtr = unique_ptr<T, function<void(T*)>> ;

public:
	VOID	Expand_ObjectPool(size_t _PLUS_SIZE) {		// 메모리 부족 시, 오브젝트 풀 확장
		for (size_t SZ = 0; SZ < _PLUS_SIZE; ++SZ)
			PoolSet.emplace(new T());

		PoolSize += _PLUS_SIZE;
	}		
	INT		Get_ObjectPoolSize() { return PoolSize; }	// 현재 풀 사이즈

	PoolPtr Acquire_Object() {							// 오브젝트 재할당 (사용 후, 초기화 한 다음 다시 넣음)
		lock_guard<mutex> lock(MTX);

		T* PTR = PoolSet.top().release();
		PoolSet.pop();
	
		return PoolPtr(PTR, [this](T* ptr) {
			lock_guard<mutex> inner_lock(MTX);
			PoolSet.emplace(unique_ptr<T>(ptr));
			});
	}

private:
	stack<unique_ptr<T>>		PoolSet;
	atomic<INT>					PoolSize;
	mutex						MTX;
};


END