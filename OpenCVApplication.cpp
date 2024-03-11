// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"

void read_images(int starting_number, char bv_type[], char dname[], bool debug = false) {
	char fname[MAX_PATH];
	int i = starting_number;
	while (1) {
		sprintf(fname, ".\\..\\Images\\%s\\new_%s (%d).jpg.jpg", dname, bv_type, i);
		Mat src = imread(fname, IMREAD_COLOR);
		if (!src.data) break;
		if (debug) std::cout << "image read" << fname << std::endl;
		i++;
	}
}

void open_batch()
{
	read_images(1, "cola", "train_images", true);
	read_images(1, "pepsi", "train_images", true);
}

int main()
{
	open_batch();
	return 0;
}