//Authors: Anthony Russell & Blake Carson
//Work done: Entire program completed by Pair Programming
//Blake drove for Waiter.cpp, Anthony drove for Baker.cpp, split up Logger.cpp and 410_proj4.cpp

#include <string>
#include "stdlib.h"

#include "../includes/externs.h"
#include "../includes/waiter.h"

using namespace std;

//ID is just a number used to identify this particular baker
//(used with PRINT statements)
//filename is what waiter reads in orders from
Waiter::Waiter(int id,std::string filename):id(id),myIO(filename){
}

Waiter::~Waiter()
{
}

//gets next Order from file_IO
//if return == SUCCESS then anOrder
//contains new order
//otherwise return contains fileIO error
int Waiter::getNext(ORDER &anOrder){
	return myIO.getNext(anOrder);
}

//contains a loop that will get orders from filename one at a time
//then puts them in order_in_Q then signals baker(s) using cv_order_inQ
//so they can be consumed by baker(s)
//when finished exits loop and signals baker(s) using cv_order_inQ that
//it is done using b_WaiterIsFinished
void Waiter::beWaiter() {
	ORDER currOrder;
	b_WaiterIsFinished = false;

	while(getNext(currOrder) == SUCCESS){
		{
			unique_lock<mutex> lck(mutex_order_inQ);
			order_in_Q.push(currOrder);
		}

		cv_order_inQ.notify_all();
	}

	{
		unique_lock<mutex> lck(mutex_order_inQ);
		b_WaiterIsFinished = true;
	}

	cv_order_inQ.notify_all();
}

