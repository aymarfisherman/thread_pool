#include "ThreadPool/WorkerThread/WorkerThread.h"

namespace thread_pool {
	WorkerThread::WorkerThread() : barrier(2), idle(true), running(true) {
		this->thread = boost::thread(boost::bind(&WorkerThread::run, this));
	}

	void WorkerThread::runTask(const TaskData& taskData) {
		this->taskData = taskData;
		this->idle = false;
		this->barrier.wait();
	}

	void WorkerThread::dispatchCallbacks() {
		this->finishedMutex.lock();
		for (auto& task : this->finishedTasks) {
			task.runCallback();
		}
		this->finishedTasks.clear();
		this->finishedMutex.unlock();
	}

	bool WorkerThread::isIdle() {
		return this->idle;
	}

	WorkerThread::~WorkerThread() {
		this->running = false;
		if (this->idle) {
			this->barrier.wait();
		}
		this->thread.join();
	}

	void WorkerThread::run() {
		this->barrier.wait();
		while (this->running) {
			this->taskData.runTask();
			this->finishedMutex.lock();
			this->finishedTasks.push_back(this->taskData);
			this->finishedMutex.unlock();
			this->idle = true;
			this->barrier.wait();
		}
	}
}