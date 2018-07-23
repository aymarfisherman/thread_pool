#pragma once

#include <list>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "TaskData/TaskData.h"

namespace thread_pool {
	class WorkerThread {
	public:
		WorkerThread();
				
		void runTask(const TaskData& taskData);
		void dispatchCallbacks();
		bool isIdle();

		virtual ~WorkerThread();

	private:		
		TaskData taskData;
		std::list<TaskData> finishedTasks[2];
		std::mutex finishedMutex;

		std::thread thread;
		std::mutex barrierMutex;
		std::condition_variable barrier;
		std::atomic_bool idle;
		std::atomic_bool running;

	private:
		void run();
		void waitForTasks();
	};
}