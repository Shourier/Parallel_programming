#include "lab1_func.h"
#include <iostream>

#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <chrono>

using namespace std;
using namespace chrono;

const int group_number = 514;
const int my_number = 17;

const int X = group_number + my_number;
const int A = X % 4;						// A = 3
const int B = 7 + X % 7;					// B = 13


#pragma region fork

void processing_by_fork(int thread_number, int* array, int array_size)
{
	for (int i = 0; i < thread_number; i++)
	{
		pid_t child_process = fork();

		if (child_process == 0)
		{
			for (int j = array_size - i - 1; j >= 0; j -= thread_number)
			{
				array[j] = (j * array[j]) & 255;
			}
		}
		else
		{
			perror("error while calling fork");
			exit(1);
		}

	}

	for (int i = 0; i < thread_number; i++)
	{
		wait(NULL);
	}
}

#pragma endregion

void apply_method(void method(int, int*, int), int thread_number, int* array, int array_size, string method_name, const char* txt_name)
{
	auto start = high_resolution_clock::now();
	method(thread_number, array, array_size);
	write_data_in_new_file(array, txt_name);
	auto finish = high_resolution_clock::now();
	duration<float> proc_time = finish - start;
	cout << method_name << " method time: " << proc_time.count() << endl;
}

int main()
{
	try
	{
		cout << "A = " << A << "; B = " << B << endl;

		int* array = open_jpg_file_as_binary_array();
		vector<int*> array_copies;
		for (int i = 0; i < 8; i++)
		{
			int* array_copy = new int[binary_array_size];
			for (int j = 0; j < binary_array_size; j++)
			{
				array_copy[j] = array[j];
			}
			array_copies.push_back(array_copy);
		}

		auto start = high_resolution_clock::now();
		int* array_1 = transforming_function(array);
		write_data_in_new_file(array_1, "data.txt");
		auto finish = high_resolution_clock::now();
		duration<float> proc_time = finish - start;
		cout << "Without parallel programming methods time: " << proc_time.count() << endl;

		
		apply_method(processing_by_fork, B, array_copies[4], binary_array_size, "processing_by_CreateProcess", "processing_by_CreateProcess.txt");
	}
	catch (const exception& e)
	{
		cout << e.what();
	}
}