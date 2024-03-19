// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <utility> 
#include <vector>
#include <string>
#include <filesystem>

typedef struct ImageData {
	char* path;
	int label;
	int generated_label;
};

typedef struct IMAGES {
	struct ImageData* data;
	int size;
	int max_size;
};



//////////////////////////////////////////Functionality testing////////////////////////////////////////////
/*
* Functie pentru a verifica care path-uri au fost testate
*/
void test_opened_images(IMAGES* images) {
	for (int i = 0; i < images->size; i++) {
		std::cout << images->data[i].path << " | Label: " << images->data[i].label << std::endl;
	}
}

bool label_good(ImageData data) {
	return (data.label == data.generated_label);
}

void print_accuracy(int correct_labels, int total_labels) {
	float accuracy = (float)correct_labels / total_labels;
	std::cout << "Image labeling accuracy: " << accuracy << std::endl;
}

void test_accuracy(IMAGES* images) {
	int correct_labels = 0;
	for (int i = 0; i < images->size; i++) {
		if (label_good(images->data[i])) {
			correct_labels++;
		}
	}
	print_accuracy(correct_labels, images->size);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////Memory management////////////////////////////////////////////////
/*
* Verifica daca mai este destula memorie alocata pentru a adauga un nou path
*/
bool index_out_of_bounds(IMAGES images) {
	return images.max_size <= images.size;
}

/*
* Se ocupa de realocarea dinamica de memorie pentru array-ul de pathuri
*/
void resize_image_array(IMAGES* images) {
	images->max_size += 100;
	images->data = (ImageData*)realloc(images->data, images->max_size * sizeof(ImageData));
}

/*
* Initializeaza variabila IMAGES, care contine in campul data toate datele necesare pentru
* prelucrarea imaginilor (momentan contine doar path-ul)
*/
IMAGES* initialize_image_array() {
	IMAGES* images = (IMAGES*)malloc(sizeof(IMAGES));
	if (images == nullptr) {
		return nullptr;
	}
	images->max_size = 100;
	images->size = 0;
	images->data = (ImageData*)malloc(images->max_size * sizeof(ImageData));
	return images;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////Image management////////////////////////////////////////////////
/*
* Creaza path-urile intr-un mod dinamic.
* number - numar dintre paranteze a fisierului
* bv_type - cola/pepsi
* dname - train_images/test_images
*/
char* create_img_path(int number, char bv_type[], char dname[]) {
	char* path = (char*)malloc(sizeof(char) * (MAX_PATH + 1));
	sprintf(path, ".\\..\\Images\\%s\\new_%s (%d).jpg.jpg", dname, bv_type, number);
	return path;
}

/*
* Creaza label-ul imaginilor, utilizand numele fisierului
* 0 = cola
* 1 = pepsi
*/
int create_label(char label_name[]) {
	return (label_name == "cola") ? 0 : 1;
}

/*
* Verifica daca imaginea de la path-ul dat chiar exista
*/
bool image_exists(std::string path) {
	return (imread(path, IMREAD_COLOR).data);
}

/*
* Verifica existenta tuturor imaginilor cu number de la starting_number
* pana la un path care nu exista
*/
void open_images(IMAGES* images, int starting_number, char bv_type[], char dname[]) {
	while (1) {
		images->size++;
		if (index_out_of_bounds(*images)) {
			resize_image_array(images);
		}
		int index = images->size - 1;
		images->data[index].path = create_img_path(starting_number++, bv_type, dname);
		images->data[index].label = create_label(bv_type);
		if (!image_exists(images->data[index].path)) {
			images->size--;
			break;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////



/*
* Se ocupa de deschiderea imaginilor din subdirectorul train
*/
void open_train_batch()
{
	IMAGES* images = initialize_image_array();
	open_images(images, 1, "cola", "train_images");
	open_images(images, 1, "pepsi", "train_images");
	test_opened_images(images);
}

int main()
{
	open_train_batch();
	return 0;
}