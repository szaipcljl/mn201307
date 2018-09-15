#include <iostream>
#include <array>
#include <algorithm> //max_element

static constexpr size_t total_pages = 256;

int main(int argc, const char *argv[])
{
#define NOT_USE_ARRAY
#ifdef NOT_USE_ARRAY
	unsigned long index_heat[total_pages] = {0};
	unsigned long index_max[2] = {0};
#else
	std::array<unsigned long, total_pages>  index_heat;
	index_heat.fill(0);
#endif

	for (int i = 0; i < 256; i++) {
		index_heat[i] = i;
	}

#ifdef NOT_USE_ARRAY
	for (int i = 0; i < 256; i++) {
		if (index_heat[i] > index_max[0]) {
			index_max[0] = index_heat[i];
			index_max[1] = i;
		}
	}
#endif

#ifdef NOT_USE_ARRAY
	std::cout << "not use array" << std::endl;
	std::cout << "###-adr:" << index_max[1] << std::endl;
#else
	std::cout << "use array" << std::endl;
	std::cout << "###:" << std::distance(index_heat.begin(), std::max_element(index_heat.begin(), index_heat.end())) << std::endl;
#endif

	return 0;
}
