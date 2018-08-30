// vector::emplace_back
#include <iostream>
#include <vector>

int main ()
{
	std::vector<int> myvector = {10,20,30};

	myvector.emplace_back (100);
	myvector.emplace_back (200);

	std::cout << "myvector contains:";
	for (auto& x: myvector)
		std::cout << ' ' << x;
	std::cout << '\n';

	std::cout << "myvector[0]:" << myvector[0] << '\n';

	return 0;
}

/*
 * notes:
 * std::vector::emplace_back
 *
 * template <class... Args>
 *   void emplace_back (Args&&... args);
 *
 *   Construct and insert element at the end
 *   Inserts a new element at the end of the vector, right after its current
 *   last element. This new element is constructed in place using args as the
 *   arguments for its constructor.
 *
 *   This effectively increases the container size by one, which causes an
 *   automatic reallocation of the allocated storage space if -and only if- the
 *   new vector size surpasses the current vector capacity.
 *
 *   The element is constructed in-place by calling allocator_traits::construct
 *   with args forwarded.
 *
 *   A similar member function exists, push_back, which either copies or moves
 *   an existing object into the container.
 *
 *   Parameters:
 *   args
 *       Arguments forwarded to construct the new element.
 *
 */
