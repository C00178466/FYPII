#pragma once
#include "NativeObject.h"

#ifndef LIBRARY_NATIVE_WRAPPER_H
#define LIBRARY_EXPORT __declspec(dllexport)

//! Constructor
extern "C" LIBRARY_EXPORT NativeObject* new_NativeObject(int value);

//! Destructor
extern "C" LIBRARY_EXPORT void delete_NativeObject(NativeObject* instance);

//! Get value function
extern "C" LIBRARY_EXPORT int get_value(NativeObject* instance);

//! Set value function
extern "C" LIBRARY_EXPORT void set_value(NativeObject* instance, int value);

#endif //LIBRARY_NATIVE_WRAPPER_H
