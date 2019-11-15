#include <mutex>

#include "../includes/externs.h"
#include "../includes/baker.h"
#include "../includes/logger.h"
using namespace std;

Logger baker_log("Baker_log.txt");

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

	baker_log.log("Baker id: " + std::to_string(id) + " Beginning loading donuts for order" + std::to_string(anOrder.order_number) + "\n");

	while(remaining_donuts > 0){
		Box box;
		DONUT donut;

		while(box.addDonut(donut) == true){
			baker_log.log("Baker id: " + std::to_string(id) + " Remaining donuts: " + std::to_string(remaining_donuts) + "\n");
			remaining_donuts--;

			if (remaining_donuts <= 0) {
				break;
			}
		}

		anOrder.boxes.push_back(box);
	}

	baker_log.log("Baker id: " + std::to_string(id) + " Finished loading donuts for order" + std::to_string(anOrder.order_number) + " Number of boxes created: " + std::to_string(anOrder.boxes.size()) + "\n");

	//lock access
	unique_lock<mutex> lck(mutex_order_outQ);
	order_out_Vector.push_back(anOrder);
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
	while (true) {
		unique_lock<mutex> lck(mutex_order_inQ);

		if (order_in_Q.empty() && b_WaiterIsFinished == true) {
			baker_log.log("No more orders for id: " + std::to_string(id) + ", exiting \n");
			break;
		}

		while (order_in_Q.empty() && b_WaiterIsFinished == false) {
			cv_order_inQ.wait(lck);
		}

		if (!order_in_Q.empty()) {
			ORDER currOrder = order_in_Q.front();
			baker_log.log("Baker id: " + std::to_string(id) + " claimed order: " + std::to_string(currOrder.order_number) + "\n");
			order_in_Q.pop();
			lck.unlock(); // unlock
			bake_and_box(currOrder);
		}
	}
}
