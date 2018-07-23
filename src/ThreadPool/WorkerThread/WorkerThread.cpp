#include "ThreadPool/WorkerThread/WorkerThread.h"

namespace thread_pool {
	WorkerThread::WorkerThread() {
		this->idle = true;
		this->running = true;
		this->thread = std::thread(std::bind(&WorkerThread::run, this));
	}

	void WorkerThread::runTask(const TaskData& taskData) {
		this->taskData = taskData;
		this->idle = false;
		this->barrier.notify_one();
	}

	void WorkerThread::dispatchCallbacks() {
		{
			std::lock_guard<std::mutex> lock(this->finishedMutex);
			this->finishedTasks[0].swap(this->finishedTasks[1]);
		}
		for (auto& task : this->finishedTasks[0]) {
			task.runCallback();
		}
		this->finishedTasks[0].clear();
		
	}

	bool WorkerThread::isIdle() {
		return this->idle;
	}

	WorkerThread::~WorkerThread() {
		this->running = false;
		if (this->idle) {
			this->barrier.notify_one();
		}
		this->thread.join();
	}

	void WorkerThread::run() {
		this->waitForTasks();
		while (this->running) {
			this->taskData.runTask();
			{
				std::lock_guard<std::mutex> lock(this->finishedMutex);
				this->finishedTasks[1].push_back(this->taskData);
			}
			this->idle = true;
			this->waitForTasks();
		}
	}

	void WorkerThread::waitForTasks() {
		std::unique_lock<std::mutex> barrierLock(this->barrierMutex);
		this->barrier.wait(barrierLock);
	}
}