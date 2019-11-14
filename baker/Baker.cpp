#include <mutex>

#include "../includes/externs.h"
#include "../includes/baker.h"
using namespace std;

Baker::Baker(int id):id(id)
{
}

Baker::~Baker()
{
}

//bake, box and append to anOrder.boxes vector
//if order has 13 donuts there should be 2 boxes
//1 with 12 donuts, 1 with 1 donut
void Baker::bake_and_box(ORDER &anOrder) {
	int remaining_donuts = anOrder.number_donuts;
//	int total_boxes = remaining_donuts / DOZEN;
//	if(remaining_donuts % DOZEN == 0) {
//		total_boxes++;
//	}
	
	/*Box box;
	DONUT donut;
	box.addDonut(donut);
	for(int i = 0; i < total_boxes; i++) {
		// remaining_donuts - dozen
	}
	*/
	while(remaining_donuts > 0){
		Box box;
		DONUT donut;
		while(box.addDonut(donut) == true){
			remaining_donuts--;
		}

		//lock access
		unique_lock<mutex> lck(mutex_order_outQ);
		anOrder.boxes.push_back(box);
	}
}

//as long as there are orders in order_in_Q then
//for each order:
//	create box(es) filled with number of donuts in the order
//  then place finished order on order_outvector
//  if waiter is finished (b_WaiterIsFinished) then
//  finish up remaining orders in order_in_Q and exit
//
//You will use cv_order_inQ to be notified by waiter
//when either order_in_Q.size() > 0 or b_WaiterIsFinished == true
//hint: wait for something to be in order_in_Q or b_WaiterIsFinished == true
void Baker::beBaker() {
	/*{
		unique_lock<mutex> lck(mutex_order_inQ);
		while (order_in_Q.empty()) {
			cv_order_inQ.wait(lck);
		}
	}*/

	while (true) {
		unique_lock<mutex> lck(mutex_order_inQ);

		if (order_in_Q.empty() && b_WaiterIsFinished == true) {
			break;
		}

		while (order_in_Q.empty() && b_WaiterIsFinished == false) {
			cv_order_inQ.wait(lck); //redundant
		}

		if (!order_in_Q.empty()) {
			ORDER currOrder = order_in_Q.front();
			order_in_Q.pop();
			lck.unlock(); // unlock
			bake_and_box(currOrder);

		}
		else {
			//add Logger stuff maybe everywhere
		}
	}
}
