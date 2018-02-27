#pragma once
#define TESTSORTARRAY_API __declspec(dllexport)

extern "C" {
	TESTSORTARRAY_API void TestSort(int a[], int length);
}