// map::find
#include <iostream>
#include <map>

int main ()
{
	std::map<char,int> mymap;
	std::map<char,int>::iterator it;

	mymap['a']=50;
	mymap['b']=100;
	mymap['c']=150;
	mymap['d']=200;

	it = mymap.find('b');
	if (it != mymap.end())
		mymap.erase (it);

	// print content:
	std::cout << "elements in mymap:" << '\n';
	std::cout << "a => " << mymap.find('a')->second << '\n';
	std::cout << "c => " << mymap.find('c')->second << '\n';
	std::cout << "d => " << mymap.find('d')->second << '\n';

	return 0;
}

/*
 * note:
 *
 * public member function
 * <map>
 * std::map::find
 *
 * iterator find (const key_type& k);
 * const_iterator find (const key_type& k) const;
 *
 * Get iterator to element
 * Searches the container for an element with a key equivalent to k and
 * returns an iterator to it if found, otherwise it returns an iterator to
 * map::end.
 *
 * Two keys are considered equivalent if the container's comparison object
 * returns false reflexively (i.e., no matter the order in which the
 * elements are passed as arguments).
 *
 * Another member function, map::count, can be used to just check whether
 * a particular key exists.
 *
 * Parameters
 *
 * k
 *     Key to be searched for.
 *     Member type key_type is the type of the keys for the elements
 *     in the container, defined in map as an alias of its first
 *     template parameter (Key).
 *
 *
 *     Return value
 *     An iterator to the element, if an element with specified key is
 *     found, or map::end otherwise.
 *
 *     If the map object is const-qualified, the function returns a
 *     const_iterator. Otherwise, it returns an iterator.
 *
 *     Member types iterator and const_iterator are bidirectional
 *     iterator types pointing to elements (of type value_type).
 *     Notice that value_type in map containers is an alias of
 *     pair<const key_type, mapped_type>.
 */
