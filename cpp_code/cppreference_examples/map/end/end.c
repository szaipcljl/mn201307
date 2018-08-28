// map::begin/end
#include <iostream>
#include <map>

int main ()
{
	std::map<char,int> mymap;

	mymap['b'] = 100;
	mymap['a'] = 200;
	mymap['c'] = 300;

	// show content:
	for (std::map<char,int>::iterator it=mymap.begin();
			it!=mymap.end(); ++it)
		std::cout << it->first << " => " << it->second << '\n';

	return 0;
}

/*
 * std::map::end
 *
 *       iterator end() noexcept;
 * const_iterator end() const noexcept;
 *
 * Return iterator to end
 * Returns an iterator referring to the past-the-end element in the map
 * container.
 *
 * The past-the-end element is the theoretical element that would follow
 * the last element in the map container. It does not point to any
 * element, and thus shall not be dereferenced.
 *
 * Because the ranges used by functions of the standard library do not
 * include the element pointed by their closing iterator, this function is
 * often used in combination with map::begin to specify a range including
 * all the elements in the container.
 *
 * If the container is empty, this function returns the same as
 * map::begin.
 *
 * Return Value
 * An iterator to the past-the-end element in the container.
 *
 * If the map object is const-qualified, the function returns a const_iterator.
 * Otherwise, it returns an iterator.
 *
 * Member types iterator and const_iterator are bidirectional iterator types
 * pointing to elements.
 */
