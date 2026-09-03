#pragma once
#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL ThreadPool {
private:
	ThreadPool();
public:
	virtual ~ThreadPool();

public:
	HRESULT Ready_ThreadPool();

	VOID	Enqueue_Task(THREAD_TYPE _TTYPE, function<void()> _Task);

	static	unique_ptr<ThreadPool>	Create();

private:
	VOID	Worker_Thread(THREAD_TYPE _TTYPE);
	VOID	Release_ThreadPool();

private:
	vector<thread> MainThread;
	vector<thread> SubThread;

	queue<function<void()>>	MainThreadTask;
	queue<function<void()>>	SubThreadTask;
	mutex					MTX;
	condition_variable		CVR;


	// mutex - 멀티 프로세스를 대상으로 쓰레드 동기화
	// CriSec - 단일 프로세스를 대상으로 쓰레드 동기화 (조금 더 가볍게 활용)
	_bool					SHUTDOWN;
};

END