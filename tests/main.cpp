#include <gtest_wrapper.h>
#include <ThreadPool.h>

static void increment(std::atomic_int* i) {
	(*i)++;
}

TEST(ThreadPoolTestSuite, ThreadPoolTestStaticFunc) {
	thread_pool::ThreadPool threadPool(8);
	std::atomic_int i = 0;
	boost::function<void()> function = boost::bind(increment, &i);
	for (int i = 0; i < 100; ++i) {
		threadPool.queueTask(function);
	}
	threadPool.join();
	EXPECT_EQ(i, 100);
}

class Foo {
public:
	Foo() {}

	void bar(std::atomic_int* i) {
		(*i)++;
	}

	virtual ~Foo() {}
};

TEST(ThreadPoolTestSuite, ThreadPoolTestClassMethod) {
	thread_pool::ThreadPool threadPool(8);
	std::atomic_int i = 0;
	Foo foo;
	boost::function<void()> function = boost::bind(&Foo::bar, &foo, &i);
	for (int i = 0; i < 100; ++i) {
		threadPool.queueTask(function);
	}
	threadPool.join();
	EXPECT_EQ(i, 100);
}