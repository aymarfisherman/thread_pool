#include <gtest_wrapper.h>
#include <ThreadPool.h>

static void increment(std::atomic_int* i) {
	(*i)++;
}

TEST(ThreadPoolTestSuite, ThreadPoolTest) {
	thread_pool::ThreadPool threadPool(8);
	std::atomic_int i = 0;
	boost::function<void()> function = boost::bind(increment, &i);
	for (int i = 0; i < 100; ++i) {
		threadPool.queueTask(function);
	}
	threadPool.join();
	EXPECT_EQ(i, 100);
}