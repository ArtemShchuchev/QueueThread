#include "SecondaryFunction.h"
#include "Thread_pool.h"
#include "Safe_queue.hpp"

/*
Что нужно сделать:
Создать потокобезопасную очередь, хранящую функции, предназначенные для исполнения.
На основе данной очереди реализовать пул потоков. Данный пул состоит из фиксированного
числа рабочих потоков (равного количеству аппаратных ядер). Когда у программы появляется
какая-то работа, она вызывает функцию, которая помещает эту работу в очередь.
Рабочий поток забирает работу из очереди, выполняет указанную в ней задачу, после чего
проверяет, есть ли в очереди другие работы.

Алгоритм работы
1. Объявить объект класса thread_pool.
2. Описать несколько тестовых функций, выводящих в консоль свое имя.
3. Раз в секунду класть в пул одновременно 2 функции и проверять их исполнение.
*/

static task_t make_task(std::shared_ptr<std::mutex> m, int i);
static task_t make_task2(std::shared_ptr<std::mutex>, int i);

int main(int argc, char** argv)
{
	printHeader(L"Курсовой проект «Потокобезопасная очередь»");

	auto consoleLock = std::make_shared<std::mutex>();
	const int numThr(std::thread::hardware_concurrency());
	//const int numThr(7);
	Thread_pool tp(numThr);
	
	for (int i = 0; i < 7; ++i) {
		tp.add(make_task(consoleLock, i));
	}
	//tp.wait();
	//std::wcout << "\n";

	for (int i = 0; i < 100; ++i) {
		tp.add(make_task2(consoleLock, i));
	}
	//tp.wait();
	//std::wcout << "\n";

	auto f1 = [consoleLock]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		std::lock_guard<std::mutex> lock(*consoleLock);
		consoleCol(col::br_green);
		std::wcout << L"-= Я функция 1, мой поток: " << std::this_thread::get_id() << " =-\n";
		consoleCol(col::cancel);
	};
	auto f2 = [consoleLock]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		std::lock_guard<std::mutex> lock(*consoleLock);
		consoleCol(col::br_yellow);
		std::wcout << L"-= Я функция 2, мой поток: " << std::this_thread::get_id() << " =-\n\n";
		consoleCol(col::cancel);
	};
	for (size_t i(0); i < 7; ++i)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		tp.add(f1);
		tp.add(f2);
	}
	//tp.wait();

	tp.add([consoleLock] {
		consoleLock->lock();
		consoleCol(col::br_red);
		std::wcout << L"-= Зависшая программа, мой поток: " << std::this_thread::get_id() << " =-\n";
		consoleCol(col::cancel);
		consoleLock->unlock();
		while (true); });

	consoleLock->lock();
	std::wcout << L"Конец main\n";
	consoleLock->unlock();
	return 0;
}


static task_t make_task(std::shared_ptr<std::mutex> m, int i)
{
	auto t = [m, i]() {
		std::unique_lock<std::mutex> lock(*m);
		consoleCol(col::br_blue + i);
		std::wcout << "id: " << std::this_thread::get_id()
			<< " -> " << i + 1 << "\n";
		consoleCol(col::cancel);
		};
	return t;
}
static task_t make_task2(std::shared_ptr<std::mutex> m, int i)
{
	auto t = [m, i]() {
		m->lock();
		std::wcout << L"Мой поток: " << std::this_thread::get_id()
			<< L", запрос: " << i + 1 << '\n';
		m->unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds{ 250 });
	};
	return t;
}
