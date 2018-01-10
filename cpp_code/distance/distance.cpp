#include <iostream>
#include <iterator>
#include <vector>

int main()
{
	std::vector<int> v{ 3, 1, 4 };

	std::cout << "distance(first, last) = "
		<< std::distance(v.begin(), v.end()) << '\n'
		<< "distance(last, first) = "
		<< std::distance(v.end(), v.begin()) << '\n';
	//the behavior is undefined (until C++11)

	std::cout << "v[1] = " << v[1] << '\n';
	std::cout << "v[2]++ = " << v[2]++ << '\n';
	std::cout << "v[2] = " << v[2] << '\n';
}
