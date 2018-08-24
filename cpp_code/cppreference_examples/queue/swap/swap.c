// queue::swap
#include <iostream>       // std::cout
#include <queue>          // std::queue

int main ()
{
	std::queue<int> foo,bar;
	foo.push (10); foo.push(20); foo.push(30);
	bar.push (111); bar.push(222);

	foo.swap(bar);

	std::cout << "size of foo: " << foo.size() << '\n';
	std::cout << "size of bar: " << bar.size() << '\n';

	return 0;
}

/*
 * std::queue::swap
 * void swap (queue& x) noexcept();
 *
 * Swap contents
 * Exchanges the contents of the container adaptor (*this) by those of x.
 *
 * This member function calls the non-member function swap (unqualified) to swap the underlying containers.
 *
 * The noexcept specifier matches the swap operation on the underlying container.
 */
