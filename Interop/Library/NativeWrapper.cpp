#include "NativeWrapper.h"

//! Constructor
extern "C" LIBRARY_EXPORT NativeObject* new_NativeObject(int value)
{
	//return a pointer to a new instance
	return new NativeObject(value);
}

//! Destructor
extern "C" LIBRARY_EXPORT void delete_NativeObject(NativeObject* instance)
{
	//delete the instance
	delete instance;
}

//! Get value function
extern "C" LIBRARY_EXPORT int get_value(NativeObject* instance)
{
	//call member function
	return instance->get_value();
}

//! Set value function
extern "C" LIBRARY_EXPORT void set_value(NativeObject* instance, int value)
{
	//call member function
	instance->set_value(value);
}