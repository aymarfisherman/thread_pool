# thread_pool
Easy-to-use ThreadPool for modern C++

Take a look, see how easy it is to spawn a number of worker threads and assign them jobs:
```
#include <gtest_wrapper.h>
#include <ThreadPool.h>

static void increment(std::atomic_int* i) {
	(*i)++;
}

TEST(ThreadPoolTestSuite, ThreadPoolTestStaticFunc) {
	thread_pool::ThreadPool threadPool(8);
	std::atomic_int i = 0;
	std::function<void()> function = std::bind(increment, &i);
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
	std::function<void()> function = std::bind(&Foo::bar, &foo, &i);
	for (int i = 0; i < 100; ++i) {
		threadPool.queueTask(function);
	}
	threadPool.join();
	EXPECT_EQ(i, 100);
}

class Bar {
public:
	Bar() : reported(false) {}

	void work() {
		this->v = 0;
		for (auto i = 0; i < 1e8; ++i) {
			this->v++;
		}
	}

	void report() {
		std::cout << "Computed value is " << this->v << "\n.";
		this->reported = (this->v == 1e8);
	}

	bool isOk() {
		return this->reported;
	}

	virtual ~Bar() {}

private:
	int v; //doesn't need to be atomic this time;
	std::atomic_bool reported;
};

TEST(ThreadPoolTestSuite, ThreadPoolTestClassMethodCallback) {
	thread_pool::ThreadPool threadPool(1);
	Bar bar;
	// work() will be run in a separate thread, report() will be run in the main thread, after work() has finished;
	threadPool.queueTask(boost::bind(&Bar::work, &bar), boost::bind(&Bar::report, &bar));
	while (!bar.isOk()) {
		threadPool.dispatchCallbacks();
	}
	EXPECT_TRUE(bar.isOk());
}
```