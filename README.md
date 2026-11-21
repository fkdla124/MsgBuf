# 직렬화버퍼
* 프로젝트에 사용할 자료구조입니다.

# Code
```cpp
#pragma once

#include <memory.h>
#include <cassert>

#define DEFAULT_CAPACITY 128

#ifndef min
	#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

// 직렬화버퍼
// DEFAULT_CAPACITY = 128
// 
// ※주의:
// 1. <<, >> 오버로딩 사용 시 버퍼에 복사한 바이트 수를 알 방법이 없습니다.
//    데이터를 다 넣지 못해도 알 방법이 없습니다. 널널한 크기를 선택하세요.
// 2. push(), pop() -> 성능을 위해 nullptr 체크하지 않습니다.
// 3. 원형 큐가 아닙니다.
class MsgBuf
{
private:
	char* buffer;
	int _size;
	int _capacity;

	int front;
	int rear;

public:
	// DEFAULT_CAPACITY = 128
	MsgBuf(): _size(0), _capacity(DEFAULT_CAPACITY), front(0), rear(0)
	{
		buffer = new char[_capacity];
	}

	// DEFAULT_CAPACITY = 128
	MsgBuf(int capacity) : _size(0), _capacity(capacity <= 0 ? DEFAULT_CAPACITY : capacity), front(0), rear(0)
	{
		buffer = new char[_capacity];
	}

	MsgBuf(const MsgBuf& src) : _size(src._size), _capacity(src._capacity), front(src.front), rear(src.rear)
	{
		buffer = new char[_capacity];
		memcpy(&buffer[front], &src.buffer[front], _size);
	}

	~MsgBuf()
	{
		delete[] buffer;
	}

	int capacity() const
	{
		return _capacity;
	}

	int size() const
	{
		return _size;
	}

	void clear()
	{
		front = 0;
		rear = 0;
		_size = 0;
	}

	int push(const char* src, int len)
	{
		int pushLen = min((_capacity - rear), len);

		int i = 0;
		for (; i < pushLen; i++)
			buffer[rear++] = *src++;

		_size += i;
		return i;
	}

	int pop(char* dest, int len)
	{
		int popLen = min(_size, len);

		int i = 0;
		for (; i < popLen; i++)
			*dest++ = buffer[front++];

		_size -= i;
		return i;
	}

	char* GetBuffer()
	{
		return buffer;
	}

	// push
	int MoveRear(int size)
	{
		if (size <= 0)
			return 0;

		int pushLen = min(_capacity - rear, size);
		rear += pushLen;
		_size += pushLen;

		return pushLen;
	}

	// pop
	int MoveFront(int size)
	{
		if (size <= 0)
			return 0;

		int popLen = min(_size, size);
		front += popLen;
		_size -= popLen;

		return popLen;
	}

	MsgBuf& operator=(const MsgBuf& src)
	{
		if (this == &src)
			return *this;

		if (_capacity != src._capacity)
		{
			delete[] buffer;
			buffer = new char[src._capacity];
			_capacity = src._capacity;
		}

		memcpy(&buffer[src.front], &src.buffer[src.front], src._size);

		_size = src._size;
		front = src.front;
		rear = src.rear;

		return *this;
	}

	//
	// <<
	//
	MsgBuf& operator<<(bool value)
	{
		push((char*)&value, sizeof(bool));
		return *this;
	}

	MsgBuf& operator<<(char value)
	{
		push((char*)&value, sizeof(char));
		return *this;
	}

	MsgBuf& operator<<(unsigned char value)
	{
		push((char*)&value, sizeof(unsigned char));
		return *this;
	}

	MsgBuf& operator<<(short value)
	{
		push((char*)&value, sizeof(short));
		return *this;
	}

	MsgBuf& operator<<(unsigned short value)
	{
		push((char*)&value, sizeof(unsigned short));
		return *this;
	}

	MsgBuf& operator<<(int value)
	{
		push((char*)&value, sizeof(int));
		return *this;
	}

	MsgBuf& operator<<(unsigned int value)
	{
		push((char*)&value, sizeof(unsigned int));
		return *this;
	}

	MsgBuf& operator<<(long value)
	{
		push((char*)&value, sizeof(long));
		return *this;
	}

	MsgBuf& operator<<(unsigned long value)
	{
		push((char*)&value, sizeof(unsigned long));
		return *this;
	}

	MsgBuf& operator<<(float value)
	{
		push((char*)&value, sizeof(float));
		return *this;
	}

	MsgBuf& operator<<(double value)
	{
		push((char*)&value, sizeof(double));
		return *this;
	}

	MsgBuf& operator<<(__int64 value)
	{
		push((char*)&value, sizeof(__int64));
		return *this;
	}

	MsgBuf& operator<<(unsigned __int64 value)
	{
		push((char*)&value, sizeof(unsigned __int64));
		return *this;
	}


	//
	// >>
	//
	MsgBuf& operator>>(bool& dest)
	{
		pop((char*)&dest, sizeof(bool));
		return *this;
	}

	MsgBuf& operator>>(char& dest)
	{
		pop((char*)&dest, sizeof(char));
		return *this;
	}

	MsgBuf& operator>>(unsigned char& dest)
	{
		pop((char*)&dest, sizeof(unsigned char));
		return *this;
	}

	MsgBuf& operator>>(short& dest)
	{
		pop((char*)&dest, sizeof(short));
		return *this;
	}

	MsgBuf& operator>>(unsigned short& dest)
	{
		pop((char*)&dest, sizeof(unsigned short));
		return *this;
	}

	MsgBuf& operator>>(int& dest)
	{
		pop((char*)&dest, sizeof(int));
		return *this;
	}

	MsgBuf& operator>>(unsigned int& dest)
	{
		pop((char*)&dest, sizeof(unsigned int));
		return *this;
	}

	MsgBuf& operator>>(long& dest)
	{
		pop((char*)&dest, sizeof(long));
		return *this;
	}

	MsgBuf& operator>>(unsigned long& dest)
	{
		pop((char*)&dest, sizeof(unsigned long));
		return *this;
	}

	MsgBuf& operator>>(float& dest)
	{
		pop((char*)&dest, sizeof(float));
		return *this;
	}

	MsgBuf& operator>>(double& dest)
	{
		pop((char*)&dest, sizeof(double));
		return *this;
	}

	MsgBuf& operator>>(__int64& dest)
	{
		pop((char*)&dest, sizeof(__int64));
		return *this;
	}

	MsgBuf& operator>>(unsigned __int64& dest)
	{
		pop((char*)&dest, sizeof(unsigned __int64));
		return *this;
	}
};
```
