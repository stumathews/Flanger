#include "pch.h"
#include "../OpenGLTemplate/CircularBuffer.h"
#include "../OpenGLTemplate/ConvolutionHelper.h"

using namespace std;


template<typename T>
void printArray(T arr[], int size)
{
	cout << "[";
	for (int i = 0; i < size ; i++)
	{
		cout << arr[i];
		if (i + 1 < size)
			cout << ",";

	}
	cout << "]" << endl;
}

class CircularBufferTests : public ::testing::Test
{
private:
	
protected:
    void SetUp() override { }

    void TearDown() override {}    
};

TEST_F(CircularBufferTests, GetWriteIndex)
{
	int m_WriteIndex = 0;
	cbuf<float> m_Buffer(4);

	m_WriteIndex = m_Buffer.GetWriteIndex();
	EXPECT_EQ(m_WriteIndex, 0);
	m_Buffer.Put(1.0);
	
	m_WriteIndex = m_Buffer.GetWriteIndex();
	EXPECT_EQ(m_WriteIndex, 1);
	m_Buffer.Put(1.0);
	
	m_WriteIndex = m_Buffer.GetWriteIndex();
	EXPECT_EQ(m_WriteIndex, 2);
	
	m_Buffer.Put(1.0);
	m_WriteIndex = m_Buffer.GetWriteIndex();
	EXPECT_EQ(m_WriteIndex, 3);
	
	m_Buffer.Put(1.0);
	m_WriteIndex = m_Buffer.GetWriteIndex();
	EXPECT_EQ(m_WriteIndex, 0);
	
	m_Buffer.Put(1.0);
	m_WriteIndex = m_Buffer.GetWriteIndex();
	EXPECT_EQ(m_WriteIndex, 1);

	m_Buffer.Put(1.0);
	m_WriteIndex = m_Buffer.GetWriteIndex();
	EXPECT_EQ(m_WriteIndex, 2);
	
}

TEST_F(CircularBufferTests, InitialSizeTest) 
{
	cbuf<float> m_Buffer(4);
	m_Buffer.Put(1.0);
	m_Buffer.Put(2.0);
	m_Buffer.Put(2.0);
	m_Buffer.Put(4.0);
	ASSERT_EQ(m_Buffer.GetSize(), 4) << "Buffer expected to be 4 items long but was " << m_Buffer.GetSize() << " in length";
}

TEST_F(CircularBufferTests, SmallArrayTest)
{
	const int cols = 4;
	const int rows = 4;
	float x[rows][cols] = 
	{ 
		{1, 2, 3, 4}, 
		{5, 6, 7, 8},
		{9, 10, 11, 12},
		{13, 14, 15, 16},
	};

	float y[rows][cols];

	cbuf<float> cbuf(4);

	for (int r = 0; r < rows; r++)
	{	
		// Simulates an new invocation with access 		
		for (int c = 0; c < cols; c++)
		{
			// Access new samples
			auto val = x[r][c];
			cbuf.Put(val);
			auto headval = cbuf.ReadNewestHead();
			ASSERT_EQ(headval, val);
		}		
	}

	EXPECT_EQ(cbuf.ReadAtIndex(0), 13);
	EXPECT_EQ(cbuf.ReadAtIndex(1), 14);
	EXPECT_EQ(cbuf.ReadAtIndex(2), 15);
	EXPECT_EQ(cbuf.ReadAtIndex(3), 16);
}

TEST_F(CircularBufferTests, ReadFromBack)
{
	cbuf<float> m_Buffer(4);
	m_Buffer.Put(1.0);
	m_Buffer.Put(2.0);
	m_Buffer.Put(3.0);
	m_Buffer.Put(4.0);

	ASSERT_EQ(m_Buffer.ReadFromBack(0), m_Buffer.ReadNewestHead()); // n
	ASSERT_EQ(m_Buffer.ReadFromBack(), m_Buffer.ReadNewestHead()); // n
	ASSERT_EQ(m_Buffer.ReadFromBack(1), 3.0); // n-1
	ASSERT_EQ(m_Buffer.ReadFromBack(2), 2.0); // n-2
	ASSERT_EQ(m_Buffer.ReadFromBack(3), 1.0); // n-3

	m_Buffer.Put(5.0); //everwrite oldest entry
	ASSERT_EQ(m_Buffer.ReadFromBack(3), 5.0); // n-3
}



TEST_F(CircularBufferTests, ReadOldestEntry)
{
	cbuf<float> m_Buffer(4); //[0,0,0,0]
	EXPECT_EQ(m_Buffer.ReadOldest(), 0);
	m_Buffer.Put(1.0); //[1,0,0,0]
	EXPECT_EQ(m_Buffer.ReadOldest(), 0);
	m_Buffer.Put(2.0); //[1,2,0,0]
	EXPECT_EQ(m_Buffer.ReadOldest(), 0);
	m_Buffer.Put(3.0); //[1,2,3,0]
	EXPECT_EQ(m_Buffer.ReadOldest(), 0);
	m_Buffer.Put(4.0); //[1,2,3,4]
	EXPECT_EQ(m_Buffer.ReadOldest(), 1);
	m_Buffer.Put(5.0); //[1,2,3,4]
	EXPECT_EQ(m_Buffer.ReadOldest(), 2);
}


TEST_F(CircularBufferTests, ReadOldest)
{
	cbuf<float> m_Buffer(4); //[0,0,0,0]
	EXPECT_EQ(m_Buffer.ReadOldest(), 0);

	m_Buffer.Put(1.0); //[1,0,0,0]
	EXPECT_EQ(m_Buffer.ReadOldest(), 0);
	m_Buffer.Put(2.0); //[1,2,0,0]
	EXPECT_EQ(m_Buffer.ReadOldest(), 0);
	m_Buffer.Put(3.0); //[1,2,3,0]
	EXPECT_EQ(m_Buffer.ReadOldest(), 0);
	m_Buffer.Put(4.0); //[1,2,3,4]
	EXPECT_EQ(m_Buffer.ReadOldest(), 1);
	m_Buffer.Put(5.0); //[1,2,3,4]
	EXPECT_EQ(m_Buffer.ReadOldest(), 2);
}

TEST_F(CircularBufferTests, ReadNewestHead)
{
	cbuf<float> m_Buffer(4); //[0,0,0,0]
	EXPECT_EQ(m_Buffer.ReadNewestHead(), 0);

	m_Buffer.Put(1.0); //[1,0,0,0]
	EXPECT_EQ(m_Buffer.ReadNewestHead(), 1);
	m_Buffer.Put(2.0); //[1,2,0,0]
	EXPECT_EQ(m_Buffer.ReadNewestHead(), 2);
	m_Buffer.Put(3.0); //[1,2,3,0]
	EXPECT_EQ(m_Buffer.ReadNewestHead(), 3);
	m_Buffer.Put(4.0); //[1,2,3,4]
	EXPECT_EQ(m_Buffer.ReadNewestHead(), 4);
	m_Buffer.Put(5.0); //[1,2,3,4]
	EXPECT_EQ(m_Buffer.ReadNewestHead(), 5);
}

TEST_F(CircularBufferTests, GetNewestIndex)
{
	cbuf<float> m_Buffer(4); //[0,0,0,0]
	EXPECT_EQ(m_Buffer.GetNewestIndex(), 0);
	
	m_Buffer.Put(1.0); //[1,0,0,0]
	EXPECT_EQ(m_Buffer.GetNewestIndex(), 0);
	m_Buffer.Put(2.0); //[1,2,0,0]
	EXPECT_EQ(m_Buffer.GetNewestIndex(), 1);
	m_Buffer.Put(3.0); //[1,2,3,0]
	EXPECT_EQ(m_Buffer.GetNewestIndex(), 2);
	m_Buffer.Put(4.0); //[1,2,3,4]
	EXPECT_EQ(m_Buffer.GetNewestIndex(), 3);
	m_Buffer.Put(5.0); //[5,2,3,4]
	EXPECT_EQ(m_Buffer.GetNewestIndex(), 0);
	m_Buffer.Put(6.0); //[5,6,3,4]
	EXPECT_EQ(m_Buffer.GetNewestIndex(), 1);

	EXPECT_EQ(m_Buffer.ReadAtIndex(0), 5);
	EXPECT_EQ(m_Buffer.ReadAtIndex(1), 6);
	EXPECT_EQ(m_Buffer.ReadAtIndex(2), 3);
	EXPECT_EQ(m_Buffer.ReadAtIndex(3), 4);
	EXPECT_EQ(m_Buffer.ReadFromBack(0), 4);

}

TEST_F(CircularBufferTests, PutReturnedIndex)
{
	cbuf<float> m_Buffer(4); //[0,0,0,0]
	int i1 = m_Buffer.Put(1.0); //[1,0,0,0]
	int i2 = m_Buffer.Put(2.0); //[1,2,0,0]
	int i3 = m_Buffer.Put(3.0); //[1,2,3,0]
	int i4 = m_Buffer.Put(4.0); //[1,2,3,4]
	int i5 = m_Buffer.Put(5.0); //[5,2,3,4]
	int i6 = m_Buffer.Put(6.0); //[5,6,3,4]

	EXPECT_EQ(i1, 0);
	EXPECT_EQ(i2, 1);
	EXPECT_EQ(i3, 2);
	EXPECT_EQ(i4, 3);
	EXPECT_EQ(i5, 0);
	EXPECT_EQ(i6, 1);
}

TEST_F(CircularBufferTests, PreviousN)
{
	cbuf<float> m_Buffer(4); //[0,0,0,0]
	int i1 = m_Buffer.Put(1.0); //[1,0,0,0]
	int i2 = m_Buffer.Put(2.0); //[1,2,0,0]
	int i3 = m_Buffer.Put(3.0); //[1,2,3,0]
	int i4 = m_Buffer.Put(4.0); //[1,2,3,4]
	int i5 = m_Buffer.Put(5.0); //[5,2,3,4]
	int i6 = m_Buffer.Put(6.0); //[5,6,3,4]

	/* Internal function not accessible to the public */
	ASSERT_EQ(m_Buffer.Wrap(0, 4), 0);
	ASSERT_EQ(m_Buffer.Wrap(1, 4), 1);
	ASSERT_EQ(m_Buffer.Wrap(2, 4), 2);
	ASSERT_EQ(m_Buffer.Wrap(3, 4), 3);
	ASSERT_EQ(m_Buffer.Wrap(4, 4), 0);
	ASSERT_EQ(m_Buffer.Wrap(5, 4), 1);
	ASSERT_EQ(m_Buffer.Wrap(6, 4), 2);

	ASSERT_EQ(m_Buffer.GetNewestIndex(), m_Buffer.Wrap(m_Buffer.GetNewestIndex(), 4));

	/* GetPrevNIndex is not accessible to the public, but ReadN(+/-x) is */
	ASSERT_EQ(m_Buffer.GetPrevNIndex(), 1);
	ASSERT_EQ(m_Buffer.GetPrevNIndex(1), 0);
	ASSERT_EQ(m_Buffer.GetPrevNIndex(2), 3);
	ASSERT_EQ(m_Buffer.GetPrevNIndex(3), 2);
	ASSERT_EQ(m_Buffer.GetPrevNIndex(4), 1);	

	ASSERT_EQ(m_Buffer.ReadN(0), 6);
	ASSERT_EQ(m_Buffer.ReadN(), 6);
	ASSERT_EQ(m_Buffer.ReadN(-1), 5);
	ASSERT_EQ(m_Buffer.ReadN(-2), 4);
	ASSERT_EQ(m_Buffer.ReadN(-3), 3);
	ASSERT_EQ(m_Buffer.ReadN(-4), 6);
	ASSERT_EQ(m_Buffer.ReadN(-5), 5);
	ASSERT_EQ(m_Buffer.ReadN(1), 3);
}

TEST_F(CircularBufferTests, Equals)
{
	cbuf<float> m_Buffer(4); //[0,0,0,0]
	int i1 = m_Buffer.Put(1.0); //[1,0,0,0]
	int i2 = m_Buffer.Put(2.0); //[1,2,0,0]
	int i3 = m_Buffer.Put(3.0); //[1,2,3,0]
	int i4 = m_Buffer.Put(4.0); //[1,2,3,4]
	int i5 = m_Buffer.Put(5.0); //[5,2,3,4]
	int i6 = m_Buffer.Put(6.0); //[5,6,3,4]

	cbuf<float> m_Buffer1(4); //[0,0,0,0]
	int m1 = m_Buffer1.Put(1.0); //[1,0,0,0]
	int m2 = m_Buffer1.Put(2.0); //[1,2,0,0]
	int m3 = m_Buffer1.Put(3.0); //[1,2,3,0]
	int m4 = m_Buffer1.Put(4.0); //[1,2,3,4]
	int m5 = m_Buffer1.Put(5.0); //[5,2,3,4]
	int m6 = m_Buffer1.Put(6.0); //[5,6,3,4]

	ASSERT_TRUE(m_Buffer == m_Buffer1) << "Buffer should report as being the same";
}

TEST_F(CircularBufferTests, InitWithArrayTest)
{
	float myArray[] = { 1.0f, 2.0f, 3.0f, 4.0f };
	cbuf<float> myCBuff(&myArray[0], 4);

	cbuf<float> control(4);
	control.Put(1.0);
	control.Put(2.0);
	control.Put(3.0);
	control.Put(4.0);
	
	ASSERT_EQ(4, myCBuff.GetSize());
	ASSERT_TRUE(myCBuff == control);
	
}


TEST_F(CircularBufferTests, EqualsNegativeTest)
{
	cbuf<float> m_Buffer(4); //[0,0,0,0]
	int i1 = m_Buffer.Put(1.0); //[1,0,0,0]
	int i2 = m_Buffer.Put(2.0); //[1,2,0,0]
	int i3 = m_Buffer.Put(3.0); //[1,2,3,0]
	int i4 = m_Buffer.Put(4.0); //[1,2,3,4]
	int i5 = m_Buffer.Put(5.0); //[5,2,3,4]
	int i6 = m_Buffer.Put(6.0); //[5,6,3,4]

	cbuf<float> m_Buffer1(4); //[0,0,0,0]
	int m1 = m_Buffer1.Put(1.0); //[1,0,0,0]
	int m2 = m_Buffer1.Put(2.0); //[1,2,0,0]
	int m3 = m_Buffer1.Put(3.0); //[1,2,3,0]
	int m4 = m_Buffer1.Put(4.9); //[1,2,3,4]
	int m5 = m_Buffer1.Put(5.0); //[5,2,3,4]
	int m6 = m_Buffer1.Put(6.0); //[5,6,3,4]

	ASSERT_FALSE(m_Buffer == m_Buffer1) << "Buffer should report as being the same";
}