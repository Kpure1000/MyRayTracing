#ifndef STACK_H
#define STACK_H
#include <vector>
using std::vector;
namespace ry
{
	template <class T>
	class Stack
	{
	public:
		Stack() : m_top(0), m_size(0)
		{
		}

		Stack(int size) : m_size(size), m_top(0)
		{
			m_stack.resize(m_size);
		}

		inline void SetSize(int size)
		{
			m_size = size;
			m_stack.resize(size);
		}

		inline void clear() { m_top = 0; }

		inline void push(T data)
		{
			if (!isFull())
			{
				m_stack[m_top] = data;
				m_top++;
			}
		}

		inline T top()
		{
			return m_stack[m_top - 1];
		}

		inline void pop()
		{
			if (!isEmpty())
				m_top--;
		}

		inline bool isEmpty() { return m_top == 0; }

		inline bool isFull() { return m_top == m_size; }

		int size() { return m_size; }

	private:
		int m_size;

		vector<T> m_stack;

		int m_top;
	};
}

#endif // !STACK_H