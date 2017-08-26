#pragma once

#include <queue>
#include <memory>
#include <ptr_containers.h>
#include "WorkerThread/WorkerThread.h"

namespace thread_pool {
	class ThreadPool {
	public:
		typedef boost::function<void()> function_type;
		typedef std::queue<function_type> function_queue_type;
		
		ThreadPool(int workerThreadsCount);
		
		void queueTask(const function_type& task);
		void join();

		bool isIdle();
		int getWorkerThreadsCount();		

		virtual ~ThreadPool();

	private:
		ptr_containers::ptr_vector<std::shared_ptr, WorkerThread> workerThreads;
		function_queue_type queuedTasks;
		boost::thread thread;
		boost::barrier barrier;
		boost::mutex threadsMutex;
		boost::mutex tasksMutex;
		std::atomic_bool idle;
		std::atomic_bool running;

	private:
		void update();
		void addTaskToQueue(const function_type& task);
		void handleQueuedTasks();
		bool hasQueuedTasks();
		bool tryRunningTask(const function_type& task);
	};
}