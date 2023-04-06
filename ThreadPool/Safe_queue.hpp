#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template<class Task>
class Safe_queue
{
private:
	std::queue<Task> taskList;	// ������� �����
	std::mutex taskLock;		// ������� ��� ���������� ������� �����
	std::condition_variable taskCondition; // ��� �����������

public:
	// ���������� � ������ ������� ����� ������, ��� ����
	// ����������� �������, � �� ��������� ��������
	// �������������� �������� ����������
	void push(const Task& task)
	{
		std::lock_guard<std::mutex> lock(taskLock);
		taskList.push(task);
		taskCondition.notify_one();
	}
	// ��������� � �������� ���� �� ������ �����������
	// �� �������� ����������. ��� ����������� ��������
	// ����������. ������ ����������� �� �������
	Task pop()
	{
		std::unique_lock<std::mutex> lock(taskLock);
		taskCondition.wait(lock, [this] { return !taskList.empty(); });

		Task task = taskList.front();
		taskList.pop();
		return task;
	}
	// ���������� true ���� ������� �����
	bool empty()
	{
		std::lock_guard<std::mutex> lock(taskLock);
		return taskList.empty();
	}
};
