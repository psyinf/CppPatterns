#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <string>

/**
 * \brief	thread safe queue, sort of
 * \author	smesserschmidt
 * \date	02.10.2018
 * \tparam	T	Generic type parameter.
 */

template <typename T> 
class ThreadSafeQueue
{
public:
	void try_lock()
	{
		mQueueMutex.try_lock();
	}
	void lock() 
	{
		mQueueMutex.lock();
	}
	void unlock()
	{
		mQueueMutex.unlock();
	}
	T receive()
	{
		auto empty = [this]()
		{
			std::lock_guard<ThreadSafeQueue> sl(*this);
			return mQueue.empty();
		};
		while (empty())
		{
			std::this_thread::yield();
			//spin
		}
		auto t = std::move(mQueue.front());
		mQueue.pop_front();
		std::cout << mQueue.size();
		return t;
	}

	auto try_receive()
	{
		std::lock_guard<ThreadSafeQueue> sl(*this);
		auto non_empty = !mQueue.empty();
		
		auto ret = non_empty ? std::optional<T>(std::move(mQueue.front())): std::nullopt;
		if (non_empty)
		{
			mQueue.pop_front();
		}
		return ret;
	}

	void send(T m)
	{
		std::lock_guard<ThreadSafeQueue> sl(*this);
		mQueue.push_back(std::move(m));
	}
	std::deque<T> mQueue;
	std::mutex mQueueMutex;
};

//messaging between threads is better kept
//http://www.drdobbs.com/parallel/prefer-using-active-objects-instead-of-n/225700095
class Active
{
public:
	using Message = std::function<void()>;
	

public:
	Active()
	{
		mThread = std::make_unique<std::thread>(&Active::run, this);
	}
	~Active()
	{
		send([&] {mDone = true; });
		mThread->join();
	}
	void send(Message m)
	{
		mMessageQueue.send(std::move(m));
	}
private:
	void run()
	{
		auto noop = [] {std::this_thread::yield(); };
		while (!mDone)
		{
			mMessageQueue.try_receive().value_or(noop)(); //execute received message
		}
	}


private:
	bool										mDone{false};	//< flag signaling end of work
	ThreadSafeQueue<Message>					mMessageQueue;	//< ThreadSafeQueue
	std::unique_ptr<std::thread>				mThread;
};

/*************************************************************************************/

class BackgroundTask 
{
public:
	void perform(std::string some_names)
	{
		mActive.send([message{ std::move(some_names)}]() {do_terribly_long_work(); std::cout << message << std::endl; });
	}
	static void do_terribly_long_work()
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
private:
	Active mActive; //this must be the last member. If Background is destroyed, it will wait until all tasks are done
};


int main()
{
	
	if (1)
	{

		BackgroundTask bt;
		bt.perform("Task1");
		bt.perform("Task2");
		bt.perform("Task3");
		std::cout << "work load sent!\n";
	}
	




	return 0;
}