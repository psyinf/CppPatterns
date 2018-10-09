#include "ConcreteObservable.h"
#include "ConcreteObserver.h"
int main(void)
{
	TimeDataProvider tdp;

	auto t1 = std::make_shared<TimeObserver>("t1",tdp);
	auto t2 = std::make_shared<TimeObserver>("t2", tdp);

	std::this_thread::sleep_for(std::chrono::seconds(10));
	return 0;
}