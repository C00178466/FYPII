#include "SortArray.h"
#include <algorithm>

extern "C" {
	void TestSort(int a[], int length) {
		std::sort(a, a + length);
	}
}