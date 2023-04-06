#include "Thread_pool.h"

Thread_pool::Thread_pool()
{
	// -1 �����, �.�.: main
	int numThr(std::thread::hardware_concurrency() - 1);
	if (numThr <= 0) numThr = 1;	// ����� ���� ������ 2�
	pool.resize(numThr);			// ������������ ������ �������
	mode.assign(numThr, thr_free);
	
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
void Thread_pool::work(const int thrNum)
{
	while (true)
	{
		task_t task = squeue.pop();
		std::unique_lock<std::mutex> ml(modeLock);
		mode[thrNum] = thr_busy;
		ml.unlock();

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

		ml.lock();
		mode[thrNum] = thr_free;
	}
}
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
	for (auto& m : mode)
	{
		if (m == thr_busy) return true;
	}
	return false;
}
// ���� ���� ��� ������ �����������
void Thread_pool::wait()
{
	while (isBusy());
}
