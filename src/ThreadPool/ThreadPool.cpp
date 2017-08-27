#include "ThreadPool/ThreadPool.h"

namespace thread_pool {
	ThreadPool::ThreadPool(int workerThreadsCount) : barrier(2), idle(true), running(true) {
		if (workerThreadsCount <= 0) {
			throw std::runtime_error("ThreadPool cannot be initiated with 0 or less worker threads.");
		}
		for (auto i = 0; i < workerThreadsCount; ++i) {
			auto workerThread = std::make_shared<WorkerThread>();
			this->workerThreads.push_back(workerThread);
		}
		this->thread = boost::thread(boost::bind(&ThreadPool::update, this));
	}

	void ThreadPool::queueTask(const function_type& task) {
		if (this->tryRunningTask(task)) {
			return;
		}
		this->addTaskToQueue(task);	
	}

	void ThreadPool::join() {
		while (!this->isIdle()) {}
	}

	bool ThreadPool::isIdle() {
		if (!this->idle) {
			return false;
		}
		this->threadsMutex.lock();
		for (auto& workerThread : this->workerThreads) {
			if (!workerThread->isIdle()) {
				this->threadsMutex.unlock();
				return false;
			}
		}
		this->threadsMutex.unlock();
		return true;
	}

	int ThreadPool::getWorkerThreadsCount() {
		return this->workerThreads.size();
	}

	ThreadPool::~ThreadPool() {
		this->running = false;
		if (this->idle) {
			this->barrier.wait();
		}
		this->thread.join();
	}

	void ThreadPool::update() {		
		this->barrier.wait();
		while (this->running) {
			this->handleQueuedTasks();
			if (!this->hasQueuedTasks()) {
				this->idle = true;
				this->barrier.wait();
			}
		}
	}

	void ThreadPool::addTaskToQueue(const function_type& task) {
		this->tasksMutex.lock();
		this->queuedTasks.push(task);
		this->tasksMutex.unlock();
		if (this->idle) {
			this->idle = false;
			this->barrier.wait();
		}
	}

	void ThreadPool::handleQueuedTasks() {
		this->tasksMutex.lock();
		function_type& task = this->queuedTasks.front();
		if (this->tryRunningTask(task)) {
			this->queuedTasks.pop();
		}
		this->tasksMutex.unlock();
	}

	bool ThreadPool::hasQueuedTasks() {
		this->tasksMutex.lock();
		bool hasTasks = !this->queuedTasks.empty();
		this->tasksMutex.unlock();
		return hasTasks;
	}

	bool ThreadPool::tryRunningTask(const function_type& task) {
		this->threadsMutex.lock();
		for (auto& workerThread : this->workerThreads) {
			if (!workerThread->isIdle()) {
				continue;
			}
			workerThread->runTask(task);
			this->threadsMutex.unlock();
			return true;
		}
		this->threadsMutex.unlock();
		return false;
	}
}