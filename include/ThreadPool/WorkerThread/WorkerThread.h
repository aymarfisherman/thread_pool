#pragma once

#include <atomic>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/barrier.hpp>

namespace thread_pool {
	class WorkerThread {
	public:
		typedef boost::function<void()> function_type;

		WorkerThread();
				
		void runTask(const function_type& task);
		bool isIdle();

		virtual ~WorkerThread();

	private:
		function_type task;
		boost::thread thread;
		boost::barrier barrier;
		std::atomic_bool idle;
		std::atomic_bool running;

	private:
		void run();
	};
}