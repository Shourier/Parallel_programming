#include <iostream>
#include <mpi.h>
#include <chrono>
#include <vector>
#include "lab1_func.h"

using namespace std;
using namespace chrono;

const int group_number = 514;
const int my_number = 17;

const int X = group_number + my_number;
const int A = X % 4;						// A = 3
const int B = 7 + X % 7;					// B = 13

int* array_copy = open_jpg_file_as_binary_array();

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	MPI_Bcast(array_copy, binary_array_size, MPI_INT, 0, MPI_COMM_WORLD);

	int proc_number, proc_index;
	MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_index);

	MPI_Barrier(MPI_COMM_WORLD);

	vector<int> local_arr;
	for (int i = binary_array_size - 1 - proc_index; i > 0; i -= proc_number)
	{
		local_arr.push_back((i * array_copy[i]) & 255);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	int* result = new int[binary_array_size];
	MPI_Gather(local_arr.data(), local_arr.size(), MPI_INT, result, local_arr.size(), MPI_INT, 0, MPI_COMM_WORLD);

	if (proc_index == 0)
	{
		write_data_in_new_file(result, "processing_by_MPI.txt");
	}
	MPI_Finalize();
}