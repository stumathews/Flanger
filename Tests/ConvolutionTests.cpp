#include "pch.h"


#include <array>
#include <gtest/gtest.h>

#include "../OpenGLTemplate/CircularBuffer.h"
#include "../OpenGLTemplate/ConvolutionHelper.h"


class ConvolutionTests : public ::testing::Test
{
private:

protected:
    void SetUp() override { }

    void TearDown() override {}
};



TEST_F(ConvolutionTests, ConvolveTest)
{
	const float s1[] = { 1.0f, 0.0f, 2.0f, 3.0f, 0.0f, 1.0f };
	const float s2[] = { 2.0f, 0.0f , 1.0f };
	const float exp[] = { 2.0f,0.0f,5.0f,6.0f,2.0f,5.0f,0.0f,1.0f };

	const auto result = static_cast<float*>(malloc(sizeof(float) * 8));
	ConvolutionHelper::ConvolveTo(s1, 6, s2, 3, result);

	auto same = true;
	for(int i = 0; i < 7 ;i++)
	{
		if (exp[i] != result[i])
		{
			same = false;
			break;
		}
		
	}

	ASSERT_TRUE(same);
	

	free(result);	
}

TEST_F(ConvolutionTests, ConvolveBuffers)
{
	const float s1[] = { 1.0f, 0.0f, 2.0f, 3.0f, 0.0f, 1.0f };
	const float s2[] = { 2.0f, 0.0f , 1.0f };
	
	const float expected[] = { 2.0f,0.0f,5.0f,6.0f,2.0f,5.0f,0.0f,1.0f };
	const auto expected_b = new cbuf<float>(const_cast<float*>(&expected[0]), 8);

	const auto s1_b = new cbuf<float>(const_cast<float*>(&s1[0]), 6);
	const auto s2_b = new cbuf<float>(const_cast<float*>(&s2[0]), 3);

	const auto result = ConvolutionHelper::ConvolveBuffers(s1_b, s2_b);

	ASSERT_TRUE(*result == *expected_b);

	delete result;
	delete expected_b;
}


TEST_F(ConvolutionTests, SimulateStream)
{
	const int nSamples = 6;
	const int numInvocations = 2;
	const int numCoefficients = 3;

	float bCoefficients[numCoefficients] = { 2.0 /*n*/, 0 /*n-1*/, 1/*n-2*/ }; // latest to earliest

	float y[numInvocations * (nSamples + numCoefficients -1)] = { 0 };
	float x[numInvocations][nSamples] = // arranged earliest to latest ie [0] = n-3 (where n = nSamples)
	{
		{ 1.0f, 0.0f, 2.0f, 3.0f, 0.0f, 1.0f}, /* as in: n-3, n-2, n-1, n */
		{ 1.0f, 0.0f, 2.0f, 3.0f, 0.0f, 1.0f}
	};
	
	const float expected[] = {
		2.0f,0.0f,5.0f,6.0f,2.0f,5.0f,0.0f,1.0f,
		2.0f,0.0f,5.0f,6.0f,2.0f,5.0f,0.0f,1.0f,
		2.0f,0.0f,5.0f,6.0f,2.0f,5.0f,0.0f,1.0f
	};

	cbuf<float> prevBuff(4);

	for (int call = 0; call < numInvocations; call++)
	{
		float* xn = &x[call][0];
		float* yn = &y[call];

		ConvolutionHelper::ConvolveTo(xn, nSamples, bCoefficients, 3, yn);
		for(int i = 0; i < (nSamples + numCoefficients - 1);i++)
		{
			ASSERT_EQ(yn[i], expected[call* (nSamples + numCoefficients - 1) + i]);
		}
	}
}
