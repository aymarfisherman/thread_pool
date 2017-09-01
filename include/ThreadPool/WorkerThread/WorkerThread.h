#pragma once

#include <atomic>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/barrier.hpp>
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
		std::list<TaskData> finishedTasks;
		boost::mutex finishedMutex;

		boost::thread thread;
		boost::barrier barrier;
		std::atomic_bool idle;
		std::atomic_bool running;

	private:
		void run();
	};
}