#include "lab1_func.h" 
#include <iostream> 
#include <chrono> 

#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/wait.h> 
#include <cstring> 
#include <unistd.h> 

using namespace std;
using namespace chrono;

const int group_number = 514;
const int my_number = 17;

const int X = group_number + my_number;
const int A = X % 4;      // A = 3 
const int B = 7 + X % 7;     // B = 13 


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
            exit(0);
        }
        else if (child_process < 0)
        {
            perror("error while calling fork");
            exit(1);
        }
        else
        {
            int status;
            waitpid(child_process, &status, 0);
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

        auto start = high_resolution_clock::now();
        int* array_1 = transforming_function(array);
        write_data_in_new_file(array_1, "data.txt");
        auto finish = high_resolution_clock::now();
        duration<float> proc_time = finish - start;
        cout << "Without parallel programming methods time: " << proc_time.count() << endl;


        key_t key = ftok(".", 'S');
        int shmid = shmget(key, binary_array_size * sizeof(int), IPC_CREAT | 0666);

        int* array_copy_0 = (int*)shmat(shmid, NULL, 0);

        for (int j = 0; j < binary_array_size; j++)
        {
            array_copy_0[j] = array[j];
        }
        apply_method(processing_by_fork, B, array_copy_0, binary_array_size, "processing_by_fork", "processing_by_fork.txt");
        for (int i = 0; i < 100; i++)
        {
            cout << array_copy_0[i] << " ";
        }
    }
    catch (const exception& e)
    {
        cout << e.what();
    }
}