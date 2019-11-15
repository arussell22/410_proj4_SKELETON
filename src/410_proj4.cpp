#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>

#include "../includes/logger.h"
#include "../includes/box.h"
#include "../includes/constants.h"
#include "../includes/waiter.h"
#include "../includes/baker.h"
#include "../includes/datastructs.h"
#include "../includes/PRINT.h"
#include "../includes/externs.h"

using namespace std;

//*************************************************
//NOTE:  most of these globals are needed
//by both bakers and waiters, so be sure to include
//externs.h in appropriate cpp implementations
//*************************************************
//the mutexes
mutex mutex_order_inQ;
mutex mutex_order_outQ;

//the condition variable
condition_variable cv_order_inQ;

//when true its time for the baker to quit
bool b_WaiterIsFinished = false;	

//where orders are stored
queue<ORDER> order_in_Q;
vector<ORDER> order_out_Vector;

//used to make PRINT statements work properly
mutex printMutex;

//*************************************************
//runs waiter until orders all read and placed
//on order_in_Q then exits
void doWaiter(int id,string fn) {
	Waiter myWaiter(id,fn);
	myWaiter.beWaiter();
}

//takes orders from order_in_Q, processes
//them and then puts them on order_out_Vector
void doBaker(int id) {
	Baker myBaker(id);
	myBaker.beBaker();
}

//prints what is in order_out_Vector
//DO NOT CALL THIS WHEN MULTIPLE THREADS ARE ACCESSING order_out_Vector
void audit_results() {
	std::vector<ORDER>::iterator itOrder;
	std::vector<Box>::iterator itBox;

	int total_donuts = 0;
	int total_orders = 0;
	int total_boxes  = 0;

	//first go through every order
	for (itOrder = order_out_Vector.begin(); itOrder != order_out_Vector.end(); itOrder++) {
		int numDonuts = 0;
		total_orders++;

		//for each order go through all the boxes and add up all the donuts
		for (itBox = (itOrder->boxes).begin(); itBox != (itOrder->boxes).end(); ++itBox) {
			total_boxes++;
			numDonuts += itBox->size();
			total_donuts += itBox->size();
		}

		//if one order was screwed up say so
		if (numDonuts != itOrder->number_donuts) 
			PRINT6("ERROR Order", itOrder->order_number, " Expected ", itOrder->number_donuts, " found ", numDonuts);
	}

	PRINT2("Total donuts made   = ", total_donuts);
	PRINT2("Total number boxes  = ", total_boxes);
	PRINT2("Total orders filled = ", total_orders);
}

int main()
{
	//Initialize variables
	vector<std::string> input_files;
	input_files.push_back("in1.txt");
	input_files.push_back("in2.txt");
	input_files.push_back("in3.txt");
	input_files.push_back("in4.txt");

	vector<thread> baker_threads;
	int numBakers = 6;

	Logger baker_log("Baker_log.txt");
	baker_log.clearlogfile();

	for (std::string file : input_files) {
		baker_log.log("\n Start testing for file: " + file + "\n");

		//Initialize bakers
		for (int i = 0; i < numBakers; i++) {
			PRINT2("Starting baker thread w/ id ", i);
			baker_threads.push_back(thread(doBaker, i));

		}

		PRINT1("Starting waiter thread");

		//Initialize waiter
		std::thread thread_waiter(doWaiter, 1, file);

		//Join waiter thread
		thread_waiter.join();

		//Join baker threads
		for (auto& baker : baker_threads) {
			baker.join();
		}
		baker_threads.clear();

		audit_results();

		PRINT2("Size of Out Vector: ", order_out_Vector.size());

		order_out_Vector.clear();

	}

	return SUCCESS;
}

