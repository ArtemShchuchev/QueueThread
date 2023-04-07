#pragma once

#include <iostream>
#include <future>
#include <vector>
#include <thread>
#include "Safe_queue.hpp"

#define DEBUG

using task_t = std::function<void()>;

class Thread_pool
{
private:
	enum class thread_mode { free, busy };
	// ������ �������
	// ������������� � ������������ ������
	// ����������� � �����������
	std::vector<std::thread> pool;
	std::vector<thread_mode> mode;
	Safe_queue<task_t> squeue;	// ������� �����
	std::mutex modeLock;
#ifdef DEBUG
	std::mutex consoleLock;
#endif // DEBUG


	// �������� �� ������� ��������� ������ � ��������� ��
	// ������ ����� ���������� ������������ ������� ��� ����������
	void work(const int thrNum);
	// ���������� true ���� � ������� ��� ���� � ����� ������ ���� ������
	bool isBusy();

public:
	Thread_pool();
	~Thread_pool();
	// �������� � ������� ��������� ������, ����� ���������
	// ������ ������� std::function ��� 
	// ������ ������� std::packaged_task
	void submit(const task_t& task);
	// ���� ���� ��� ������ �����������
	void wait();
};