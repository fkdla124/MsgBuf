#pragma once

#include <memory.h>
#include <cassert>
#include <stdexcept>
#include <string>

#include "ObjectFreeList.h"

// test
#define DEFAULT_CAPACITY 128
class Buffer
{
	char buf[DEFAULT_CAPACITY];
};

class MsgBuf;
ObjectFreeList<Buffer, GuardCheck::ON, IdCheck::ON, PlacementNew::OFF> msgBufPool;

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
// 4. 메시지 처리부에서 항상 일회용으로 쓰고 버리거나, 초기화하면서 사용하는 환경을 상상하여 만들었습니다.
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
		// buffer = new char[_capacity];
		buffer = (char*)msgBufPool.alloc();
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
		clear();
		msgBufPool.free((Buffer*)buffer);
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
		int pushed = push((char*)&value, sizeof(bool)) != sizeof(bool);
		if (pushed != sizeof(bool))
		{
			std::string errorStr("MsgBuf& operator>>(bool value) -> Error. push return " + std::to_string(pushed)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear)
				+ "\nvalue: " + std::to_string(value));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator<<(char value)
	{
		int pushed = push((char*)&value, sizeof(char));
		if (pushed != sizeof(char))
		{
			std::string errorStr("MsgBuf& operator>>(char value) -> Error. push return " + std::to_string(pushed)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear)
				+ "\nvalue: " + std::to_string(value));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator<<(unsigned char value)
	{
		int pushed = push((char*)&value, sizeof(unsigned char));
		if (pushed != sizeof(unsigned char))
		{
			std::string errorStr("MsgBuf& operator>>(unsigned char value) -> Error. push return " + std::to_string(pushed)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear)
				+ "\nvalue: " + std::to_string(value));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator<<(short value)
	{
		int pushed = push((char*)&value, sizeof(short));
		if (pushed != sizeof(short))
		{
			std::string errorStr("MsgBuf& operator>>(short value) -> Error. push return " + std::to_string(pushed)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear)
				+ "\nvalue: " + std::to_string(value));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator<<(unsigned short value)
	{
		int pushed = push((char*)&value, sizeof(unsigned short));
		if (pushed != sizeof(unsigned short))
		{
			std::string errorStr("MsgBuf& operator>>(unsigned short value) -> Error. push return " + std::to_string(pushed)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear)
				+ "\nvalue: " + std::to_string(value));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator<<(int value)
	{
		int pushed = push((char*)&value, sizeof(int));
		if (pushed != sizeof(int))
		{
			std::string errorStr("MsgBuf& operator>>(int value) -> Error. push return " + std::to_string(pushed)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear)
				+ "\nvalue: " + std::to_string(value));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator<<(unsigned int value)
	{
		int pushed = push((char*)&value, sizeof(unsigned int));
		if (pushed != sizeof(unsigned int))
		{
			std::string errorStr("MsgBuf& operator>>(unsigned int value) -> Error. push return " + std::to_string(pushed)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear)
				+ "\nvalue: " + std::to_string(value));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator<<(long value)
	{
		int pushed = push((char*)&value, sizeof(long));
		if (pushed != sizeof(long))
		{
			std::string errorStr("MsgBuf& operator>>(long value) -> Error. push return " + std::to_string(pushed)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear)
				+ "\nvalue: " + std::to_string(value));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator<<(unsigned long value)
	{
		int pushed = push((char*)&value, sizeof(unsigned long));
		if (pushed != sizeof(unsigned long))
		{
			std::string errorStr("MsgBuf& operator>>(unsigned long value) -> Error. push return " + std::to_string(pushed)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear)
				+ "\nvalue: " + std::to_string(value));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator<<(float value)
	{
		int pushed = push((char*)&value, sizeof(float));
		if (pushed != sizeof(float))
		{
			std::string errorStr("MsgBuf& operator>>(float value) -> Error. push return " + std::to_string(pushed)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear)
				+ "\nvalue: " + std::to_string(value));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator<<(double value)
	{
		int pushed = push((char*)&value, sizeof(double));
		if (pushed != sizeof(double))
		{
			std::string errorStr("MsgBuf& operator>>(double value) -> Error. push return " + std::to_string(pushed)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear)
				+ "\nvalue: " + std::to_string(value));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator<<(__int64 value)
	{
		int pushed = push((char*)&value, sizeof(__int64));
		if (pushed != sizeof(__int64))
		{
			std::string errorStr("MsgBuf& operator>>(__int64 value) -> Error. push return " + std::to_string(pushed)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear)
				+ "\nvalue: " + std::to_string(value));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator<<(unsigned __int64 value)
	{
		int pushed = push((char*)&value, sizeof(unsigned __int64));
		if (pushed != sizeof(unsigned __int64))
		{
			std::string errorStr("MsgBuf& operator>>(unsigned __int64 value) -> Error. push return " + std::to_string(pushed)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear)
				+ "\nvalue: " + std::to_string(value));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}


	//
	// >>
	//
	MsgBuf& operator>>(bool& dest)
	{
		int poped = pop((char*)&dest, sizeof(bool));
		if (poped != sizeof(bool))
		{
			std::string errorStr("MsgBuf& operator>>(bool value) -> Error. pop return " + std::to_string(poped)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator>>(char& dest)
	{
		int poped = pop((char*)&dest, sizeof(char));
		if (poped != sizeof(char))
		{
			std::string errorStr("MsgBuf& operator>>(char value) -> Error. pop return " + std::to_string(poped)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator>>(unsigned char& dest)
	{
		int poped = pop((char*)&dest, sizeof(unsigned char));
		if (poped != sizeof(unsigned char))
		{
			std::string errorStr("MsgBuf& operator>>(unsigned char value) -> Error. pop return " + std::to_string(poped)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator>>(short& dest)
	{
		int poped = pop((char*)&dest, sizeof(short));
		if (poped != sizeof(short))
		{
			std::string errorStr("MsgBuf& operator>>(short value) -> Error. pop return " + std::to_string(poped)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator>>(unsigned short& dest)
	{
		int poped = pop((char*)&dest, sizeof(unsigned short));
		if (poped != sizeof(unsigned short))
		{
			std::string errorStr("MsgBuf& operator>>(unsigned short value) -> Error. pop return " + std::to_string(poped)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator>>(int& dest)
	{
		int poped = pop((char*)&dest, sizeof(int));
		if (poped != sizeof(int))
		{
			std::string errorStr("MsgBuf& operator>>(int value) -> Error. pop return " + std::to_string(poped)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator>>(unsigned int& dest)
	{
		int poped = pop((char*)&dest, sizeof(unsigned int));
		if (poped != sizeof(unsigned int))
		{
			std::string errorStr("MsgBuf& operator>>(unsigned int value) -> Error. pop return " + std::to_string(poped)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator>>(long& dest)
	{
		int poped = pop((char*)&dest, sizeof(long));
		if (poped != sizeof(long))
		{
			std::string errorStr("MsgBuf& operator>>(long value) -> Error. pop return " + std::to_string(poped)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator>>(unsigned long& dest)
	{
		int poped = pop((char*)&dest, sizeof(unsigned long));
		if (poped != sizeof(unsigned long))
		{
			std::string errorStr("MsgBuf& operator>>(unsigned long value) -> Error. pop return " + std::to_string(poped)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator>>(float& dest)
	{
		int poped = pop((char*)&dest, sizeof(float));
		if (poped != sizeof(float))
		{
			std::string errorStr("MsgBuf& operator>>(float value) -> Error. pop return " + std::to_string(poped)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator>>(double& dest)
	{
		int poped = pop((char*)&dest, sizeof(double));
		if (poped != sizeof(double))
		{
			std::string errorStr("MsgBuf& operator>>(double value) -> Error. pop return " + std::to_string(poped)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator>>(__int64& dest)
	{
		int poped = pop((char*)&dest, sizeof(__int64));
		if (poped != sizeof(__int64))
		{
			std::string errorStr("MsgBuf& operator>>(__int64 value) -> Error. pop return " + std::to_string(poped)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear));

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}

	MsgBuf& operator>>(unsigned __int64& dest)
	{
		int poped = pop((char*)&dest, sizeof(unsigned __int64));
		if (poped != sizeof(unsigned __int64))
		{
			std::string errorStr("MsgBuf& operator>>(unsigned __int64 value) -> Error. pop return " + std::to_string(poped)
				+ "\ncapacity: " + std::to_string(_capacity)
				+ "\nsize: " + std::to_string(_size)
				+ "\nfront: " + std::to_string(front)
				+ "\nrear: " + std::to_string(rear) + "\n\n");

			errorStr += "\n\nMsgBuf.buffer: \n";
			for (int i = 1; i <= _capacity; i++)
			{
				errorStr += std::to_string(buffer[i]) + " ";

				if (i % 4 == 0 && i != 0)
				{
					errorStr += "\n";
				}
			}

			throw std::runtime_error(errorStr);
		}
		return *this;
	}
};