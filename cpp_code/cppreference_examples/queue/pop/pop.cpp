// queue::push/pop
#include <iostream>       // std::cin, std::cout
#include <queue>          // std::queue

int main ()
{
	std::queue<int> myqueue;
	int myint;

	std::cout << "Please enter some integers (enter 0 to end):\n";

	do {
		std::cin >> myint;
		myqueue.push (myint);
	} while (myint);

	std::cout << "myqueue contains: ";
	while (!myqueue.empty())
	{
		std::cout << ' ' << myqueue.front();
		myqueue.pop();
	}
	std::cout << '\n';

	return 0;
}

/*
 * note:
 * std::queue::pop
 * void pop();
 *
 * Remove next element
 * Removes the next element in the queue, effectively reducing its size by one.
 *
 * The element removed is the "oldest" element in the queue whose value can be
 * retrieved by calling member queue::front.
 *
 * This calls the removed element's destructor.
 *
 * This member function effectively calls the member function pop_front of the
 * underlying container object.
 */
