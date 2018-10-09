#include "IObserver.h"
#include "ConcreteObservable.h"


#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>


class TimeObserver : public IObserver
{
public:
	TimeObserver(const std::string& name, TimeDataProvider& observable)
		: mName(name)
		, mTimeObservable(observable)
	{
		observable.addObserver(this);
	};
	virtual ~TimeObserver()
	{
		mTimeObservable.removeObserver(this);
	}
	virtual void update() override
	{
		auto tp = mTimeObservable.getTime();
		std::time_t time = std::chrono::system_clock::to_time_t(tp);
		std::tm timetm = *std::localtime(&time);
		std::cout << mName <<" reports: " << std::put_time(&timetm, "%c %Z") << " +"
			<< std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count() % 1000 << "ms" << std::endl;
	}

private:
	const std::string		mName;
	TimeDataProvider& mTimeObservable;
};