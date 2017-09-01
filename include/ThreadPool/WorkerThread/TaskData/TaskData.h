#pragma once

#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace thread_pool {
	class TaskData {
	public:
		typedef boost::function<void()> function_type;

		TaskData();
		TaskData(const function_type& task, const function_type& callback);

		void runTask();
		void runCallback();

		virtual ~TaskData();

	private:
		function_type task;
		function_type callback;
	};
}