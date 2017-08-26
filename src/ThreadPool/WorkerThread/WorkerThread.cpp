#include "ThreadPool/WorkerThread/WorkerThread.h"

namespace thread_pool {
	WorkerThread::WorkerThread() : barrier(2), idle(true), running(true) {
		this->thread = boost::thread(boost::bind(&WorkerThread::run, this));
	}

	void WorkerThread::runTask(const function_type& task) {
		this->task = task;
		this->idle = false;
		this->barrier.wait();
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
			this->task();
			this->idle = true;
			this->barrier.wait();
		}
	}
}