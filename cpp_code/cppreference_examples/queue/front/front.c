// queue::front
#include <iostream>       // std::cout
#include <queue>          // std::queue

int main ()
{
	std::queue<int> myqueue;

	myqueue.push(77);
	myqueue.push(16);

	myqueue.front() -= myqueue.back();    // 77-16=61

	std::cout << "myqueue.front() is now " << myqueue.front() << '\n';

	return 0;
}

/*
 * note:
 * std::queue::front
 *       reference& front();
 *       const_reference& front() const;
 * Access next element
 * Returns a reference to the next element in the queue.
 *
 * The next element is the "oldest" element in the queue and the same element
 * that is popped out from the queue when queue::pop is called.
 *
 * This member function effectively calls member front of the underlying
 * container object.
 */
