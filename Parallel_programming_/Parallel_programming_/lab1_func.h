#pragma once
#ifndef LAB1_FUNC
#define LAB1_FUNC

#define CHAR_SIZE sizeof(char)

#include <iostream>
#include <fstream>

int binary_array_size = 0;
int jpg_file_size = 0;

int* open_jpg_file_as_binary_array()
{
	FILE* jpg_file = NULL;
	if (fopen_s(&jpg_file, "1623781112_img0.jpg", "r") != 0)
	{
		return NULL;
		throw std::exception("Cannot open file or it doesn't exist!");
	}

	fseek(jpg_file, 0, SEEK_END);
	jpg_file_size = ftell(jpg_file);
	fseek(jpg_file, 0, SEEK_SET);

	binary_array_size = jpg_file_size / CHAR_SIZE;
	char* binary_raw_array = new char[binary_array_size];

	fread(binary_raw_array, CHAR_SIZE, binary_array_size, jpg_file);
	fclose(jpg_file);

	int* binary_array = new int[binary_array_size];
	for (int i = 0; i < binary_array_size; i++)
	{
		binary_array[i] = int(binary_raw_array[i]);
	}

	delete[] binary_raw_array;
	return (binary_array);
}


void write_data_in_new_file(int* array, const char* file_name)
{
	FILE* data_file = NULL;
	if (fopen_s(&data_file, file_name, "w+") != 0)
	{
		throw std::exception("Cannot open file or it doesn't exist!");
	}

	for (int i = 0; i < binary_array_size; i++)
	{
		fprintf(data_file, "%d ", array[i]);
	}
	fclose(data_file);
}


int* transforming_function(int* transformed_array)
{
	int* result = new int[binary_array_size];
	for (int i = 0; i < binary_array_size; i++)
	{
		result[i] = (i * transformed_array[i]) & 255;
	}
	return (result);
}

#endif 
