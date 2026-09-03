#include "ThreadPool.h"

ThreadPool:: ThreadPool() : SHUTDOWN(FALSE)	{			}
ThreadPool::~ThreadPool() { Release_ThreadPool(); }
HRESULT ThreadPool::Ready_ThreadPool() {
	MainThread.reserve(5);
	SubThread .reserve(5);

	for (INT IDX = 0; IDX < MainThread.size(); ++IDX)	{ MainThread.emplace_back	([this]	()	{ Worker_Thread(THREAD_TYPE::MAIN_THREAD);	});};
	for (INT IDX = 0; IDX < SubThread.size() ; ++IDX)	{ SubThread.emplace_back	([this]	()	{ Worker_Thread(THREAD_TYPE::SUB_THREAD);	});};
	
	return S_OK;
}

VOID ThreadPool::Enqueue_Task(THREAD_TYPE _TTYPE, function<void()> _Task) {
	if (SHUTDOWN) { throw runtime_error("All Thread has been shutdowned."); }

	lock_guard<mutex> lock(MTX);

	if		(_TTYPE == THREAD_TYPE::MAIN_THREAD) { MainThreadTask.push(move(_Task)); }
	else if (_TTYPE == THREAD_TYPE::SUB_THREAD ) { SubThreadTask .push(move(_Task)); }

	CVR.notify_one();
}
VOID ThreadPool::Worker_Thread(THREAD_TYPE _TTYPE) {
	while (TRUE) {
		function<void()> Task;
		if (_TTYPE == THREAD_TYPE::MAIN_THREAD) {
			unique_lock<mutex> lock(MTX);
			CVR.wait(lock, [this]() { return !this->MainThreadTask.empty() || SHUTDOWN; });
			if (this->MainThreadTask.empty() && SHUTDOWN) break;

			Task = move(MainThreadTask.front());
			MainThreadTask.pop();
			lock.unlock();
			Task();
		}
		else {
			unique_lock<mutex> lock(MTX);
			CVR.wait(lock, [this]() { return !this->SubThreadTask.empty() || SHUTDOWN; });
			if (this->SubThreadTask.empty() && SHUTDOWN) break;

			Task = move(SubThreadTask.front());
			SubThreadTask.pop();
			lock.unlock();
			Task();
		}
	}
}

unique_ptr<ThreadPool>	ThreadPool::Create() {
	auto Instance = unique_ptr<ThreadPool>(new ThreadPool());
	if (FAILED(Instance->Ready_ThreadPool( ))) {
		MSG_BOX("Cannot Create ThreadPool.");
		return nullptr;
	}
	return Instance;
}

void ThreadPool::Release_ThreadPool() {
	SHUTDOWN = TRUE;
	CVR.notify_all();
	for (auto& TRD : MainThread) TRD.join();
	for (auto& TRD : SubThread)  TRD.join();
}
