#pragma once

class NativeObject {

public:
	NativeObject(int value);
	~NativeObject(void);

	int get_value();
	void set_value(int value);

private:
	int m_value;
};