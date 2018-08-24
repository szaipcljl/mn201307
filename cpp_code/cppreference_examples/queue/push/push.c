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
 * std::queue::push
 * C++11:
 * void push (const value_type& val);
 * void push (value_type&& val);
 *
 * Insert element
 * Inserts a new element at the end of the queue, after its current last
 * element. The content of this new element is initialized to val.
 *
 * This member function effectively calls the member function push_back of the
 * underlying container object.
 */
