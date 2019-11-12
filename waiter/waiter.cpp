#include <string>
#include "stdlib.h"

#include "../includes/externs.h"
#include "../includes/waiter.h"

using namespace std;

Waiter::Waiter(int id,std::string filename):id(id),myIO(filename){
}

Waiter::~Waiter()
{
}

//gets next Order(s) from file_IO
int Waiter::getNext(ORDER &anOrder){
	return myIO.getNext(anOrder);
}

void Waiter::beWaiter() {
	ORDER currOrder;

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

