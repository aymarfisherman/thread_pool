#include "ThreadPool/WorkerThread/TaskData/TaskData.h"

namespace thread_pool {
	TaskData::TaskData() {}

	TaskData::TaskData(const function_type& task, const function_type& callback)
		: task(task), callback(callback) {}

	void TaskData::runTask() {
		this->task();
	}

	void TaskData::runCallback() {
		this->callback();
	}

	TaskData::~TaskData() {}
}