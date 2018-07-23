#include "ThreadPool/ThreadPool.h"

namespace thread_pool {
	ThreadPool::ThreadPool(int workerThreadsCount) : idle(true), running(true) {
		if (workerThreadsCount <= 0) {
			throw std::runtime_error("ThreadPool cannot be initiated with 0 or less worker threads.");
		}
		for (auto i = 0; i < workerThreadsCount; ++i) {
			auto workerThread = std::make_shared<WorkerThread>();
			this->workerThreads.push_back(workerThread);
		}
		this->thread = std::thread(std::bind(&ThreadPool::update, this));
	}

	void ThreadPool::dispatchCallbacks() {
		for (auto& workerThread : this->workerThreads) {
			workerThread->dispatchCallbacks();
		}
	}

	void ThreadPool::queueTask(const function_type& task, const function_type& callback) {
		TaskData taskData(task, callback);
		if (this->tryRunningTask(taskData)) {
			return;
		}
		this->addTaskToQueue(taskData);
	}

	void ThreadPool::join() {
		while (!this->isIdle()) {
			this->dispatchCallbacks();
		}
		this->dispatchCallbacks();
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
			this->barrier.notify_one();
		}
		this->thread.join();
	}

	void ThreadPool::update() {		
		this->waitForTasks();
		while (this->running) {
			this->handleQueuedTasks();
			if (!this->hasQueuedTasks()) {
				this->idle = true;
				this->waitForTasks();
			}
		}
	}

	void ThreadPool::addTaskToQueue(const TaskData& taskData) {
		this->tasksMutex.lock();
		this->queuedTasks.push(taskData);
		this->tasksMutex.unlock();
		if (this->idle) {
			this->idle = false;
			this->barrier.notify_one();
		}
	}

	void ThreadPool::handleQueuedTasks() {
		this->tasksMutex.lock();
		TaskData& taskData = this->queuedTasks.front();
		if (this->tryRunningTask(taskData)) {
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

	bool ThreadPool::tryRunningTask(const TaskData& taskData) {
		this->threadsMutex.lock();
		for (auto& workerThread : this->workerThreads) {
			if (!workerThread->isIdle()) {
				continue;
			}
			workerThread->runTask(taskData);
			this->threadsMutex.unlock();
			return true;
		}
		this->threadsMutex.unlock();
		return false;
	}

	void ThreadPool::waitForTasks() {
		std::unique_lock<std::mutex> barrierLock(this->barrierMutex);
		this->barrier.wait(barrierLock);
	}
}