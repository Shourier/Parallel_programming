#include "lab1_func.h"
#include <windows.h>

// #include <pthread.h>
#include <thread>
#include <mutex>

#include <omp.h>

// #include <unistd.h>

#include <vector>
#include <chrono>

using namespace std;
using namespace chrono;

const int group_number = 514;
const int my_number = 17;

const int X = group_number + my_number;
const int A = X % 4;						// A = 3
const int B = 7 + X % 7;					// B = 13

HANDLE* semaph = new HANDLE;
mutex mtx;

#pragma region CreateThread_CreateSemaphore

void transform_by_thread(vector<int*>* args)
{
	int* array = (*args)[0];
	int* arr_size = (*args)[1];
	int* delta = (*args)[2];
	WaitForSingleObject(*semaph, INFINITE);
	for (int i = *arr_size - *delta - 1; i >= 0; i -= B)
	{
		// WaitForSingleObject(*semaph, INFINITE);
		array[i] = (i * array[i]) & 255;
		// ReleaseSemaphore(*semaph, 1, NULL);
	}
	ReleaseSemaphore(*semaph, 1, NULL);
}

void processing_by_CreateThread(int thread_number, int* array, int array_size)
{
	HANDLE* threads_array = new HANDLE[thread_number];
	*semaph = CreateSemaphore(NULL, thread_number, thread_number, NULL);
	for (int i = 0; i < thread_number; i++)
	{
		vector<int*>* arguments = new vector<int*>;
		int* delta = new int(i);
		*arguments = { array, &array_size, delta };
		threads_array[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)transform_by_thread, arguments, 0, NULL);
	}
	WaitForMultipleObjects(thread_number, threads_array, TRUE, INFINITE);

	CloseHandle(*semaph);
	for (int i = 0; i < thread_number; i++)
	{
		CloseHandle(threads_array[i]);
	}
}

#pragma endregion

#pragma region pthread

void* transform_by_pthread(void* args)
{
	vector<int*>* a = static_cast<vector<int*>*>(args);
	int* array = (*a)[0];
	int* arr_size = (*a)[1];
	int* delta = (*a)[2];
	for (int i = *arr_size - *delta - 1; i >= 0; i -= B)
	{
		array[i] = (i * array[i]) & 255;
	}
	return NULL;
}

void processing_by_pthread_create(int thread_number, int* array, int array_size)
{
	// pthread_t* threads_array = new pthread_t[thread_number];
	int* threads_id = new int[thread_number];

	for (int i = 0; i < thread_number; i++)
	{
		int* delta = new int(i);
		vector<int*>* arguments = new vector<int*>;
		*arguments = { array, &array_size, delta };
		void* a = static_cast<void*>(arguments);
		// pthread_create(&threads_array[i], NULL, transform_by_pthread, a);								// У меня траблы с линковкой, но код должен быть рабочим - IDE не может разобраться с именами из библиотеки, в VS2022 я на скорую руку не смог корректно поставить 
	}

	for (int i = 0; i < thread_number; i++)
	{
		//pthread_join(threads_array[i], nullptr);
	}
}

#pragma endregion

#pragma region std_thread

void transform_function_by_mtx(int* array, int array_size, int delta)
{
	for (int i = array_size - delta - 1; i >= 0; i -= B)
	{
		lock_guard<mutex> lock(mtx);
		array[i] = (i * array[i]) & 255;
	}
}

void transform_function(int* array, int array_size, int delta)    // faster
{
	for (int i = array_size - delta - 1; i >= 0; i -= B)
	{
		array[i] = (i * array[i]) & 255;
	}
}

void processing_by_std_thread(int thread_number, int* array, int array_size)
{
	thread* threads_array = new thread[thread_number];

	for (int i = 0; i < thread_number; i++)
	{
		threads_array[i] = thread(transform_function_by_mtx, array, array_size, i);
	}

	for (int i = 0; i < thread_number; i++)
	{
		threads_array[i].join();
	}
}

#pragma endregion

#pragma region OMP

void processing_by_OMP(int thread_number, int* array, int array_size)
{
	#pragma omp parallel num_threads(thread_number)
	{
		int thread_id = omp_get_thread_num();
		for (int i = array_size - thread_id - 1; i >= 0; i -= thread_number)
		{
			array[i] = (i * array[i]) & 255;  
		}
	}
}

#pragma endregion

#pragma region _CreateProcess_   
// with MapViewOfFile and CreateEvent (5c2 and 5d)

void processing_by_CreateProcess(int thread_number, int* array, int array_size)
{
	HANDLE* Processes_array = new HANDLE[thread_number];
	HANDLE Start_flag = CreateEvent(NULL, TRUE, FALSE, L"Global\\Start_processing");
	STARTUPINFO* startup_Infos = new STARTUPINFO[thread_number];
	PROCESS_INFORMATION* process_Infos = new PROCESS_INFORMATION[thread_number];

	HANDLE h_Map_File = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, array_size * sizeof(int), L"Array");
	int* shared_array = (int*)MapViewOfFile(h_Map_File, FILE_MAP_ALL_ACCESS, 0, 0, array_size * sizeof(int));

	for (int i = 0; i < array_size; i++)
	{
		shared_array[i] = array[i];
	}

	for (int i = 0; i < thread_number; i++)
	{
		ZeroMemory(&startup_Infos[i], sizeof(STARTUPINFO));
		startup_Infos[i].cb = sizeof(STARTUPINFO);
		ZeroMemory(&process_Infos[i], sizeof(PROCESS_INFORMATION));

		wstring com_line = L"C:\\Users\\user\\source\\repos\\Parallel_programming_\\x64\\Debug\\Process_function.exe " + to_wstring(array_size) + L" " + to_wstring(i);

		if (CreateProcessW(nullptr, const_cast<wchar_t*>(com_line.c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startup_Infos[i], &process_Infos[i]))
		{
			Processes_array[i] = process_Infos[i].hProcess;
		}
		else 
		{
			cerr << "Error" << GetLastError() << endl;
		}
	}

	SetEvent(Start_flag);
	WaitForMultipleObjects(thread_number, Processes_array, TRUE, INFINITE);

	for (int i = 0; i < thread_number; i++)
	{
		CloseHandle(Processes_array[i]);
	}

	for (int i = 0; i < array_size; i++)
	{
		array[i] = shared_array[i];
	}
	
	UnmapViewOfFile(shared_array);
	CloseHandle(h_Map_File);
	CloseHandle(Start_flag);
	/*delete[] shared_array;
	delete[] Processes_array;
	delete[] startup_Infos;
	delete[] process_Infos;*/
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

		apply_method(processing_by_CreateThread, B, array_copies[0], binary_array_size, "processing_by_CreateThread", "processing_by_CreateThread.txt");
		// apply_method(processing_by_pthread_create, B, array_copies[1], binary_array_size, "processing_by_pthread_create", "processing_by_pthread_create.txt");
		apply_method(processing_by_std_thread, B, array_copies[2], binary_array_size, "processing_by_std_thread", "processing_by_std_thread.txt");
		apply_method(processing_by_OMP, B, array_copies[3], binary_array_size, "processing_by_OMP", "processing_by_OMP.txt");
		apply_method(processing_by_CreateProcess, B, array_copies[4], binary_array_size, "processing_by_CreateProcess", "processing_by_CreateProcess.txt");
	}
	catch (const exception& e)
	{
		cout << e.what();
	}
}