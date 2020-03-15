#pragma once
#include <vector>
using namespace std;
#pragma optimize( "", off )

class CircularBufferTests_PreviousN_Test;

/*
A very simple circular buffer for my DSP assignment
*/

template <typename T>
class cbuf
{
	friend CircularBufferTests_PreviousN_Test; // For testing

private:
	int m_Size; // size of the buffer. It is fixed.
	vector<T> m_Buffer; // underlying buffer, buffer[0] is considered the first item
	int m_WriteIndex;
	int m_ReadIndex;

	// Internal functions for calculating indexes relative to n (last sample)
	int GetPrevNIndex(int n = 0) const { return Wrap(GetNewestIndex() - n, m_Size); }
	int Wrap(int n, int arrayLength) const { return ((n % arrayLength) + arrayLength) % arrayLength; }

public:
	cbuf(unsigned size) :m_Size(size), m_WriteIndex(0), m_ReadIndex(0), m_Buffer(vector<T>(size)) { }

	cbuf(T* values, unsigned length) : cbuf(length)
	{
		m_Buffer.clear();		
		m_Buffer.insert(m_Buffer.end(), &values[0], &values[length]);
	}
	
	~cbuf() { m_Buffer.clear(); m_Buffer = { 0 }; }

	// Expose the underlying vector for direct pointer maipultion.
	// https://stackoverflow.com/questions/2923272/how-to-convert-vector-to-array
	T* ToArray() { return  &m_Buffer[0];	}

	// Overwrite oldest entry, returns the index of the item saved
	int Put(T item)
	{
		m_Buffer[GetWriteIndex()] = item;
		m_ReadIndex = GetWriteIndex();
		m_WriteIndex = (m_WriteIndex + 1) % m_Size;
		return m_ReadIndex;
	}

	// Read the value most recently added
	T ReadNewestHead() const { return m_Buffer[m_ReadIndex]; }

	// Get the index of the last added item ie the newest
	int GetNewestIndex() const { return m_ReadIndex; }

	// Read at a specific index. No bounds checking.
	T ReadAtIndex(int i) const { return m_Buffer[i]; }

	// Read at value from the back of the buffer
	T ReadFromBack(int n = 0) const { return m_Buffer[(m_Size - 1) - n]; }

	// Use index notation to reference items relative to the most recent item. ie n
	// Supports notation like -3 as in n-3 as well as 0 which means n
	// Not tested when n < 0
	T ReadN(int n = 0) const { return m_Buffer[GetPrevNIndex(-n)]; }

	// Get the value that will be overwritten next
	T ReadOldest() const { return m_Buffer[m_WriteIndex]; }

	// Get the index of the value that is going to be overwritten by next Put
	int GetOldIndex() const { return m_WriteIndex; }

	// same as GetOldIndex()
	int GetWriteIndex() const { return GetOldIndex(); }

	// Size of the underlying buffer.
	int GetSize() const { return m_Buffer.size(); }

	bool operator==(cbuf<T> otherBuffer)
	{
		if (otherBuffer.GetSize() != GetSize())
			return false;
		
		for(auto i = 0; i < otherBuffer.GetSize();i++)
			if (otherBuffer.ReadAtIndex(i) != ReadAtIndex(i))
				return false;
		
		return true;
	}	

	void PrintContents()
	{
		cout << "[";
		for (int i = 0; i < m_Buffer.size(); i++)
		{
			std::cout << m_Buffer[i];
			if (i < m_Buffer.size())
				cout << ",";
		}
		cout << "]" << endl;;
	}

	T ReadNext()
	{
		auto val = m_Buffer[GetWriteIndex()];
		m_WriteIndex = (m_WriteIndex + 1) % m_Size;
		
		return val;
	}
	
};
#pragma optimize( "", on )