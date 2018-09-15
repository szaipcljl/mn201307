// queue::emplace
#include <iostream>       // std::cin, std::cout
#include <queue>          // std::queue
#include <string>         // std::string, std::getline(string)

int main ()
{
	std::queue<std::string> myqueue;

	myqueue.emplace ("First sentence");
	myqueue.emplace ("Second sentence");

	std::cout << "myqueue contains:\n";
	while (!myqueue.empty())
	{
		std::cout << myqueue.front() << '\n';
		myqueue.pop();
	}

	return 0;
}

/*
 * note:
 * std::queue::emplace
 * template <class... Args> void emplace (Args&&... args);
 *
 * Construct and insert element:
 * Adds a new element at the end of the queue, after its current last element.
 * This new element is constructed in place passing args as the arguments for
 * its constructor.
 *
 * This member function effectively calls the member function emplace_back of
 * the underlying container, forwarding args.
 */
