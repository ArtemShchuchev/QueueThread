#include "Thread_pool.h"


Thread_pool::Thread_pool()
{
	// -1 поток, т.к.: main
	int numThr(std::thread::hardware_concurrency() - 1);
	if (numThr <= 0) numThr = 1;	// вдруг ядер меньше 2х
	pool.resize(numThr);			// устанавливаю размер вектора
	mode.assign(numThr, thread_mode::free);
	
	// выборка задач из очереди, происходит в потоках
	for (int i(0); i<numThr; ++i) pool[i] = std::thread([this, i] { work(i); });
	/*
	pool.reserve(numThr);			// устанавливаю размер вектора
	for (int i(0); i < numThr; ++i) pool.push_back(std::thread([this, i] { work(i); }));
	*/
}

Thread_pool::~Thread_pool()
{
	for (auto& pt : pool) squeue.push(nullptr);	// остановить ожидание пустой очереди
	for (auto& pt : pool) pt.join();
}
// выбирает из очереди очередную задачу и выполняет ее
#define ATOMIC_MODE(fn) modeLock.lock();mode[thrNum]=(fn);modeLock.unlock()
void Thread_pool::work(const int thrNum)
{
	while (true)
	{
		ATOMIC_MODE(thread_mode::free);
		task_t task = squeue.pop();
		ATOMIC_MODE(thread_mode::busy);

#ifdef DEBUG
		std::lock_guard<std::mutex> lock(consoleLock);
		std::wcout << L"< Поток: " << thrNum;
#endif // DEBUG

		if (task)
		{
#ifdef DEBUG
			std::wcout << L" - в работе >  ";
#endif // DEBUG
			task();
		}
		else
		{
#ifdef DEBUG
			std::wcout << L" - завершил работу >\n";
#endif // DEBUG
			break;	// больше задач не будет СТОП поток
		}
	}
}
#undef ATOMIC_MODE
// помещает в очередь очередную задачу
void Thread_pool::submit(const task_t& task)
{
	squeue.push(task);
}
// возвращает true если в очереди или хоть в одном потоке есть задачи
bool Thread_pool::isBusy()
{
	if (!squeue.empty()) return true;

	std::lock_guard<std::mutex> ml(modeLock);
	for (auto& status : mode)
	{
		if (status == thread_mode::busy) return true;
	}
	return false;
}
// ждет пока все потоки освободятся
void Thread_pool::wait()
{
	while (isBusy());
}
