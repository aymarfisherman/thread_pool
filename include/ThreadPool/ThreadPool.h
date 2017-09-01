#pragma once

#include <queue>
#include <memory>
#include "WorkerThread/WorkerThread.h"

namespace thread_pool {
	static void noCallback() {}

	class ThreadPool {
	public:
		typedef TaskData::function_type function_type;

		ThreadPool(int workerThreadsCount);
		
		void dispatchCallbacks();

		void queueTask(const function_type& task, const function_type& callback = std::bind(noCallback));
		void join();

		bool isIdle();
		int getWorkerThreadsCount();		

		virtual ~ThreadPool();

	private:
		std::vector<std::shared_ptr<WorkerThread> > workerThreads;
		std::queue<TaskData> queuedTasks;
		boost::thread thread;
		boost::barrier barrier;
		boost::mutex threadsMutex;
		boost::mutex tasksMutex;
		std::atomic_bool idle;
		std::atomic_bool running;

	private:
		void update();
		void addTaskToQueue(const TaskData& taskData);
		void handleQueuedTasks();
		bool hasQueuedTasks();
		bool tryRunningTask(const TaskData& taskData);
	};
}