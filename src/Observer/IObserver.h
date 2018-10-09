#pragma once
#include "IObservable.h"
#include <memory>

class IObserver 
{

public:
	virtual void update() = 0;
};

