#pragma once
class ConvolutionHelper
{
public:
	
	// Swaps the signal and convolves the b coefficients using previous values stored in circular buffer, updates the buffer	
	static void convolution_sum(const float* chunk, 
						   const unsigned int num_samples_per_chunk,
						   const unsigned int n, //current or most recent sample 
		                   float* yn, // output
	                       const float* b_coefficients,
						   const int num_coefficients, 
						   cbuf<float>* prev_samples, // access to the previous samples
						   const float coefficient_scale = 1) // Uses the camera's X position to scale the coefficients
	{
		const auto xn = chunk[(num_samples_per_chunk - 1) - n];
		
		
		*yn = 0;// prev_samples->ReadN(-5);

		prev_samples->Put(chunk[n]);
	}

	/*
	 * Perform s1 * s2 ie a convolution of s1 and s2
	 * s1: input vector or signal or x[]
	 * N1: length of s1
	 * s2: filter b coefficients
	 * N2: length of s2
	 * Returns a pointer to cbuf<float>,which represents y[], which the caller is responsible for releasing after use
	 * Remarks: No need to flip any of the input vectors
	 */
	static void ConvolveTo(const float* s1, const unsigned int N1, const float* s2, const unsigned int N2, float* result)
	{
		for (auto i = 0; i < N1+N2-1; i++)
			result[i] = convolve_at_t1(i, s1, N1, s2, N2);
	}

	static cbuf<float>* ConvolveBuffers(cbuf<float>* s1, cbuf<float>* s2)
	{
		auto result = new cbuf<float>(s1->GetSize() + s2->GetSize() - 1);
		for (auto i = 0; i < result->GetSize(); i++)
			result->Put(convolve_at_t1(i, s1->ToArray(), s1->GetSize(), s2->ToArray(), s2->GetSize()));
		return result;
	}

private:

	// Performs convolution at position y[t1]	
	static float convolve_at_t1(const unsigned int t1, const float* s1, const unsigned int N1, const float* s2, const unsigned int N2)
	{
		float sum = 0;
		for (unsigned t = 0; t <= N2 - 1; t++)
			sum += t1 - t >= N1 ? 0 : s1[t1 - t] * s2[t];
		return sum;
	}

};

