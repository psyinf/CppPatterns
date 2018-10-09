#pragma once


#include <algorithm>
#include <memory>
#include <vector>

class IObserver;

class IObservable
{
public:
	using ObserverPtr = IObserver*;

	
	virtual void addObserver(const ObserverPtr& observer) = 0;
	virtual void removeObserver(const ObserverPtr& observer) = 0;
	virtual void notify() = 0;
};