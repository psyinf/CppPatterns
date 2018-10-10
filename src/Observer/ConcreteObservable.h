#pragma once
#include "IObservable.h"
#include "IObserver.h"

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

class TimeDataProvider : public IObservable
{
	
	using ObserverList = std::vector<ObserverPtr>;
	using TimeData = std::chrono::system_clock::time_point;
	
public:
	TimeDataProvider()
		:mCurrentTime(std::chrono::system_clock::now())
	{
		mRunning = true;
		mUpdateThread = std::thread( &TimeDataProvider::run, this);
	}
	~TimeDataProvider()
	{
		mRunning = false;
		mUpdateThread.join();
	}
	void run()
	{
		while (mRunning)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			auto measured_time = std::chrono::system_clock::now();
			if ((measured_time - mCurrentTime.load()) > std::chrono::milliseconds(1000))
			{
				mCurrentTime = measured_time;
				notify();
			}
		}
	}

	

	void addObserver(const ObserverPtr &observer)  override
	{
		std::lock_guard sl(mObserversMutex);
		mObservers.push_back(observer);
	}
	void removeObserver(const ObserverPtr& observer)  override
	{
		std::lock_guard sl(mObserversMutex);
		
		mObservers.erase(std::find(mObservers.begin(), mObservers.end(), observer));
	}
	
	virtual void notify() override
	{
		std::lock_guard sl(mObserversMutex);
		for (auto& observer : mObservers)
		{
			observer->update();
		}
	}

	TimeData getTime() const
	{
		return mCurrentTime;
	}

protected:
	std::thread				mUpdateThread;
	std::mutex				mObserversMutex;
	std::atomic<TimeData>		mCurrentTime;
	std::atomic_bool		mRunning;
	ObserverList			mObservers;
};
