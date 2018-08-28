// map::count
#include <iostream>
#include <map>

int main ()
{
	std::map<char,int> mymap;
	char c;

	mymap ['a']=101;
	mymap ['c']=202;
	mymap ['f']=303;

	for (c='a'; c<'h'; c++)
	{
		std::cout << c;
		if (mymap.count(c)>0)
			std::cout << " is an element of mymap.\n";
		else
			std::cout << " is not an element of mymap.\n";
	}

	return 0;
}

/*
 * std::map::count
 * size_type count (const key_type& k) const;
 *
 * Count elements with a specific key
 * Searches the container for elements with a key equivalent to k and returns
 * the number of matches.
 *
 * Because all elements in a map container are unique, the function can only
 * return 1 (if the element is found) or zero (otherwise).
 *
 * Two keys are considered equivalent if the container's comparison object
 * returns false reflexively (i.e., no matter the order in which the keys are
 * passed as arguments).
 *
 * Parameters:
 *
 * k
 *     Key to search for.
 *     Member type key_type is the type of the element keys in the
 *     container, defined in map as an alias of its first template parameter
 *     (Key).
 */
