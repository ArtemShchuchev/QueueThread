#include "Thread_pool.h"


Thread_pool::Thread_pool()
{
	// -1 �����, �.�.: main
	int numThr(std::thread::hardware_concurrency() - 1);
	if (numThr <= 0) numThr = 1;	// ����� ���� ������ 2�
	pool.resize(numThr);			// ������������ ������ �������
	mode.assign(numThr, thread_mode::free);
	
	// ������� ����� �� �������, ���������� � �������
	for (int i(0); i<numThr; ++i) pool[i] = std::thread([this, i] { work(i); });
	/*
	pool.reserve(numThr);			// ������������ ������ �������
	for (int i(0); i < numThr; ++i) pool.push_back(std::thread([this, i] { work(i); }));
	*/
}

Thread_pool::~Thread_pool()
{
	for (auto& pt : pool) squeue.push(nullptr);	// ���������� �������� ������ �������
	for (auto& pt : pool) pt.join();
}
// �������� �� ������� ��������� ������ � ��������� ��
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
		std::wcout << L"< �����: " << thrNum;
#endif // DEBUG

		if (task)
		{
#ifdef DEBUG
			std::wcout << L" - � ������ >  ";
#endif // DEBUG
			task();
		}
		else
		{
#ifdef DEBUG
			std::wcout << L" - �������� ������ >\n";
#endif // DEBUG
			break;	// ������ ����� �� ����� ���� �����
		}
	}
}
#undef ATOMIC_MODE
// �������� � ������� ��������� ������
void Thread_pool::submit(const task_t& task)
{
	squeue.push(task);
}
// ���������� true ���� � ������� ��� ���� � ����� ������ ���� ������
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
// ���� ���� ��� ������ �����������
void Thread_pool::wait()
{
	while (isBusy());
}
