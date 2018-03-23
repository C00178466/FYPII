#include "NativeObject.h"

NativeObject::NativeObject(int value) :
	m_value(value)
{

}

NativeObject::~NativeObject(void)
{

}

int NativeObject::get_value()
{
	return m_value;
}

void NativeObject::set_value(int value)
{
	m_value = value;
}