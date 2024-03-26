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
	float accuracy;
};



/////////////////////////////////////////////////Headers///////////////////////////////////////////////////
void test_opened_images(IMAGES*);
bool label_good(ImageData);
void print_accuracy(IMAGES*);
bool index_out_of_bounds(IMAGES);
void resize_image_array(IMAGES*);
IMAGES* initialize_image_array();
int generate_label();
void process_images(IMAGES*);
void calculate_accuracy(IMAGES*);
char* create_img_path(int, char, char);
int assign_label(char);
bool image_exists(char*);
void open_images(IMAGES*, int, char, char);
void process_images(IMAGES*);
void open_train_batch();
void print_class_accuracy(IMAGES* images);
std::pair<float, float> average_color_intensity(cv::Mat image);
///////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////Functionality testing////////////////////////////////////////////
/*
* Functie pentru a verifica care path-uri au fost testate
*/
void test_opened_images(IMAGES* images) {
	for (int i = 0; i < images->size; i++) {
		std::cout << images->data[i].path << " | Label: " << images->data[i].label <<
			" | Generated Label: " << images->data[i].generated_label << std::endl;
	}
}

bool label_good(ImageData data) {
	return (data.label == data.generated_label);
}

void print_accuracy(IMAGES* images) {
	std::cout << "Image labeling accuracy: " << images->accuracy << std::endl;
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



//////////////////////////////////////////Label management////////////////////////////////////////////////
void calculate_accuracy(IMAGES* images) {
	int correct_labels = 0;
	for (int i = 0; i < images->size; i++) {
		if (label_good(images->data[i])) {
			correct_labels++;
		}
	}
	images->accuracy = (float)correct_labels / images->size;
}

int generate_label() {
	return rand() % 2;
}

void assign_image_labels(IMAGES* images) {
	srand(time(NULL));
	for (int i = 0; i < images->size; i++) {
		images->data[i].generated_label = generate_label();
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////Image management////////////////////////////////////////////////
void test_labels(IMAGES* images, int nr_of_test = 10) {
	int count1 = 0, count2 = 0;
	assign_image_labels(images);
	calculate_accuracy(images);

	// Display the first 10 Coca Cola images
	std::cout << "Coca Cola Images:" << std::endl;
	for (int j = 0; j < images->size && count1 < nr_of_test; j++) {
		if (images->data[j].label == 0) { // Coca Cola label
			cv::Mat img = cv::imread(images->data[j].path);
			if (!img.empty()) {
				cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);
				cv::imshow("Image", img);
				cv::waitKey(0);
				count1++;
			}
		}
	}
	std::cout << "The accuracy for Coca Cola is: ";
	print_accuracy(images);

	// Display the first 10 Pepsi images
	std::cout << "Pepsi Images:" << std::endl;
	for (int j = 0; j < images->size && count2 < nr_of_test; j++) {
		if (images->data[j].label == 1) { // Pepsi label
			cv::Mat img = cv::imread(images->data[j].path);
			if (!img.empty()) {
				cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);
				cv::imshow("Image", img);
				cv::waitKey(0);
				count2++;
			}
		}
	}
	std::cout << "The accuracy for Pepsi is: ";
	print_accuracy(images);
	print_class_accuracy(images);

	Sleep(1000);
}


/*
* Creaza path-urile intr-un mod dinamic.
* number - numar dintre paranteze a fisierului
* bv_type - cola/pepsi
* dname - train_images/test_images
*/
char* create_img_path(int number, char bv_type[], char dname[]) {
	char* path = (char*)malloc(sizeof(char) * (MAX_PATH + 1));
	sprintf(path, ".\\Images\\%s\\new_%s (%d).jpg.jpg", dname, bv_type, number);
	return path;
}

/*
* Creaza label-ul imaginilor, utilizand numele fisierului
* 0 = cola
* 1 = pepsi
*/
int assign_label(char label_name[]) {
	return (label_name == "cola") ? 0 : 1;
}

/*
* Verifica daca imaginea de la path-ul dat chiar exista
*/
bool image_exists(char* path) {
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
		images->data[index].label = assign_label(bv_type);
		images->data[index].generated_label = -1;
		if (!image_exists(images->data[index].path)) {
			images->size--;
			break;
		}
	}
}
///////////////////////////////////////////////////////////////////////CLASS ACCURACY

//calculeaza intensitatea culorii pentru rosu si albastru
std::pair<float, float> average_color_intensity(cv::Mat image) {
	cv::Scalar mean = cv::mean(image);
	return { mean.val[2], mean.val[0] }; // mean.val[2] este rosu, mean.val[0] este albastru
}

// functie pentru a calcula si printa acuratetea per clasa
void print_class_accuracy(IMAGES* images) {
	int correct_coca_cola = 0;
	int total_coca_cola = 0;
	int correct_pepsi = 0;
	int total_pepsi = 0;

	for (int i = 0; i < images->size; i++) {
		cv::Mat img = cv::imread(images->data[i].path);
		
		std::pair<float, float> color_intensity = average_color_intensity(img);

		
		float red_intensity = color_intensity.first;
		float blue_intensity = color_intensity.second;

		//intensitate rosu mare inseamna coca cola si intensitate albastra mare inseamna pepsi
		int predicted_label = red_intensity > blue_intensity ? 0 : 1;

		if (images->data[i].label == 0) {
			total_coca_cola++;
			if (predicted_label == images->data[i].label) {
				correct_coca_cola++;
			}
		}
		else if (images->data[i].label == 1) {
			total_pepsi++;
			if (predicted_label == images->data[i].label) {
				correct_pepsi++;
			}
		}
	}

	float accuracy_coca_cola =  (float)correct_coca_cola / total_coca_cola;
	float accuracy_pepsi = (float)correct_pepsi / total_pepsi;

	std::cout << "Coca Cola class accuracy: " << accuracy_coca_cola * 100 << "%" << std::endl;
	std::cout << "Pepsi class accuracy: " << accuracy_pepsi * 100 << "%" << std::endl;
}

//////////////////////////////////////////////////////////////// BATCH


/*
* Se ocupa de deschiderea imaginilor din subdirectorul train
*/
void open_train_batch()
{
	IMAGES* images_train = initialize_image_array();
	open_images(images_train, 1, "cola", "train_images");
	open_images(images_train, 1, "pepsi", "train_images");
	test_labels(images_train);
}

/*
* Se ocupa de deschiderea imaginilor din subdirectorul train
*/
void open_test_batch()
{
	IMAGES* images_test = initialize_image_array();
	open_images(images_test, 149, "cola", "test_images");
	open_images(images_test, 204, "cola", "test_images");
	open_images(images_test, 230, "cola", "test_images");
	open_images(images_test, 150, "pepsi", "test_images");
	open_images(images_test, 233, "pepsi", "test_images");
}

int main()
{
	open_train_batch();
	open_test_batch();
	return 0;
}