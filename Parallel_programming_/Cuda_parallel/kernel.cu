#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "lab1_func.h"

#include <chrono>
#include <iostream>
#include <vector>

#include <unordered_set>

using namespace std;
using namespace chrono;

const int group_number = 514;
const int my_number = 17;

const int X = group_number + my_number;
const int A = X % 4;						// A = 3
const int B = 7 + X % 7;					// B = 13

const int block_size = 512;					// Число нитей в блоке

#pragma region CUDA_processing 

__global__ void CUDA_processing(int* array, int* indices, int* borders)
{
	int start_index = 0;
	int end_index = borders[0];
	for (int i = 0; i < blockIdx.x; i++)
	{
		start_index += borders[i];
		end_index += borders[i];
	}

	__syncthreads();

	for (int i = start_index + threadIdx.x; i < end_index; i += block_size)
	{
		array[indices[i]] = (indices[i] * array[indices[i]]) & 255;
	}
}

#pragma endregion

int main()
{
	try
	{
		cout << "A = " << A << "; B = " << B << endl;

		int* array = open_jpg_file_as_binary_array();
		int* array_copy = new int[binary_array_size];
		for (int j = 0; j < binary_array_size; j++)
		{
			array_copy[j] = array[j];
		}


		vector<vector<int>> ind_matrix;
		for (int i = 0; i < B; i++)
		{
			vector<int> block_indeces;
			for (int j = binary_array_size - 1 - i; j >= 0; j -= B)
			{
				block_indeces.push_back(j);
			}
			ind_matrix.push_back(block_indeces);
		}


		int ind_arr_size = 0;
		int* indeces_borders = new int[B];
		for (int i = 0; i < B; i++)
		{
			ind_arr_size += ind_matrix[i].size();
			indeces_borders[i] = ind_matrix[i].size();
		}
		
		int* ind_arr = new int[ind_arr_size];
		int prev_sum = 0;
		for (int i = 0; i < B; i++)
		{
			for (int j = prev_sum; j < prev_sum + indeces_borders[i]; j++)
			{
				ind_arr[j] = ind_matrix[i][j - prev_sum];
			}
			prev_sum += indeces_borders[i];
		}


		int* gpu_data;
		int* gpu_indices;
		int* gpu_indices_borders;

		cudaMalloc(&gpu_data, binary_array_size * sizeof(int));
		cudaMalloc(&gpu_indices, ind_arr_size * sizeof(int));
		cudaMalloc(&gpu_indices_borders, B * sizeof(int));

		cudaMemcpy(gpu_data, array_copy, binary_array_size * sizeof(int), cudaMemcpyHostToDevice);
		cudaMemcpy(gpu_indices, ind_arr, ind_arr_size * sizeof(int), cudaMemcpyHostToDevice);
		cudaMemcpy(gpu_indices_borders, indeces_borders, B * sizeof(int), cudaMemcpyHostToDevice);

		auto start = high_resolution_clock::now();

		CUDA_processing<<<B, block_size>>>(gpu_data, gpu_indices, gpu_indices_borders);
		cudaDeviceSynchronize();

		auto finish = high_resolution_clock::now();
		duration<float> proc_time = finish - start;

		cudaMemcpy(array_copy, gpu_data, binary_array_size * sizeof(int), cudaMemcpyDeviceToHost);
		write_data_in_new_file(array_copy, "processing_by_CUDA.txt");
		cout << "CUDA method time: " << proc_time.count() << endl;

		cudaFree(gpu_data);
		cudaFree(gpu_indices);
		cudaFree(gpu_indices_borders);
	}
	catch (const exception&)
	{

	}
}
