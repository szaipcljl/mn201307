// constructing queues
#include <iostream>       // std::cout
#include <deque>          // std::deque
#include <list>           // std::list
#include <queue>          // std::queue

int main ()
{
	std::deque<int> mydeck (3,100);        // deque with 3 elements
	std::list<int> mylist (2,200);         // list with 2 elements

	std::queue<int> first;                 // empty queue
	std::queue<int> second (mydeck);       // queue initialized to copy of deque

	std::queue<int,std::list<int> > third; // empty queue with list as underlying container
	std::queue<int,std::list<int> > fourth (mylist);

	std::cout << "size of first: " << first.size() << '\n';
	std::cout << "size of second: " << second.size() << '\n';
	std::cout << "size of third: " << third.size() << '\n';
	std::cout << "size of fourth: " << fourth.size() << '\n';

	return 0;
}

/*
 * notes:
 * public member function
 * <queue>
 * std::queue::queue
 *
 * explicit queue (const container_type& ctnr = container_type());
 *
 * Construct queue
 * Constructs a queue container adaptor object.
 */

/*
 * note:
 * class template
 * std::queue
 *
 * template <class T, class Container = deque<T> > class queue;
 *
 * FIFO queue
 * queues are a type of container adaptor, specifically designed to operate in a
 * FIFO context (first-in first-out), where elements are inserted into one end
 * of the container and extracted from the other.
 *
 * queues are implemented as containers adaptors, which are classes that use an
 * encapsulated object of a specific container class as its underlying
 * container, providing a specific set of member functions to access its
 * elements. Elements are pushed into the "back" of the specific container and
 * popped from its "front".
 *
 * The underlying container may be one of the standard container class template
 * or some other specifically designed container class. This underlying
 * container shall support at least the following operations:
 *
 *     empty
 *     size
 *     front
 *     back
 *     push_back
 *     pop_front
 *
 *
 * The standard container classes deque and list fulfill
 * these requirements. By default, if no container class
 * is specified for a particular queue class
 * instantiation, the standard container deque is used.
 */
