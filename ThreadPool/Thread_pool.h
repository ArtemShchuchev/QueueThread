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
	// вектор потоков
	// инициализация в конструкторе класса
	// уничтожение в деструкторе
	std::vector<std::thread> pool;
	std::vector<thread_mode> mode;
	Safe_queue<task_t> squeue;	// очередь задач
	std::mutex modeLock;
#ifdef DEBUG
	std::mutex consoleLock;
#endif // DEBUG


	// выбирает из очереди очередную задачу и выполняет ее
	// данный метод передается конструктору потоков для исполнения
	void work(const int thrNum);
	// возвращает true если в очереди или хоть в одном потоке есть задачи
	bool isBusy();

public:
	Thread_pool();
	~Thread_pool();
	// помещает в очередь очередную задачу, метод принимает
	// объект шаблона std::function или 
	// объект шаблона std::packaged_task
	void submit(const task_t& task);
	// ждет пока все потоки освободятся
	void wait();
};