// queue::back
#include <iostream>       // std::cout
#include <queue>          // std::queue

int main ()
{
	std::queue<int> myqueue;

	myqueue.push(12);
	myqueue.push(75);   // this is now the back

	myqueue.back() -= myqueue.front();

	std::cout << "myqueue.back() is now " << myqueue.back() << '\n';

	return 0;
}

/*
 * note:
 * std::queue::back
 * c++11:
 *       reference& back();
 *       const_reference& back() const;
 * Access last element
 * Returns a reference to the last element in the queue. This is the "newest"
 * element in the queue (i.e. the last element pushed into the queue).
 *
 * This member function effectively calls member back of the underlying
 * container object.
 */
