#include "SecondaryFunction.h"
#include "Thread_pool.h"		// тут для наглядности -> #define DEBUG
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
std::mutex consoleLock;

int main(int argc, char** argv)
{
	printHeader(L"Курсовой проект «Потокобезопасная очередь»");

	int count(0);

	auto fun = [&count]() {
		std::unique_lock<std::mutex> lock(consoleLock);
		consoleCol(col::br_blue + count);
		std::wcout << "id: " << std::this_thread::get_id()<< " -> " << ++count << "\n";
		consoleCol(col::cancel);
		lock.unlock();
		std::this_thread::sleep_for(std::chrono::microseconds(3));
	};
	Thread_pool tp;
	for (size_t i = 0; i < 7; i++) tp.submit(fun);
	tp.wait();
	std::wcout << "\n";

	auto f1 = []() {
		std::lock_guard<std::mutex> lock(consoleLock);
		consoleCol(col::br_green);
		std::wcout << L"-= Я функция 1, мой поток: " << std::this_thread::get_id() << " =-\n";
		consoleCol(col::cancel);
	};
	auto f2 = []() {
		std::lock_guard<std::mutex> lock(consoleLock);
		consoleCol(col::br_yellow);
		std::wcout << L"-= Я функция 2, мой поток: " << std::this_thread::get_id() << " =-\n\n";
		consoleCol(col::cancel);
	};
	for (size_t i(0); i < 5; ++i)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		tp.submit(f1);
		tp.submit(f2);
	}
	tp.wait();

	std::wcout << L"Конец программы\n";
	return 0;
}

