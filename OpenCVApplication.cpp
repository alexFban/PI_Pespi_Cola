// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <utility> 
#include <vector>
#include <string>
#include <filesystem>

#define DECISION_PRAG 2

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
void print_accuracy_graph(float, float, const char*, const char*);
void generate_class_accuracy_graph(IMAGES*);
void test_labels(IMAGES*, int, bool);
void test_opened_images(IMAGES*);
bool label_good(ImageData);
void print_accuracy(IMAGES*);

bool index_out_of_bounds(IMAGES);
void resize_image_array(IMAGES*);
IMAGES* initialize_image_array();

int generate_label_colour(char*);
int generate_label_rand();
void assign_image_labels(IMAGES*);
void calculate_accuracy(IMAGES*);

cv::Mat toBinary_Red(char* img_path);
cv::Mat toBinary_Blue(char* img_path);
std::pair<float, float> average_color_intensity(cv::Mat);
char* create_img_path(int, char, char);
int assign_label(char);
bool image_exists(char*);
void open_images(IMAGES*, int, char, char);

void open_train_batch();
void open_test_batch();
///////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////Functionality testing////////////////////////////////////////////
void print_accuracy_graph(float class_one_accuracy, float class_two_accuracy, const char* class_one_name = "COLA", const char* class_two_name = "PEPSI") {
	std::cout << "\t" << class_one_name << "\t\t" << class_two_name << std::endl;
	std::cout << class_one_name << "\t" << class_one_accuracy << "\t\t" << 1 - class_one_accuracy << std::endl;
	std::cout << class_two_name << "\t" << 1 - class_two_accuracy << "\t\t" << class_two_accuracy << '\n' << std::endl;
}

void generate_class_accuracy_graph(IMAGES* images) {
	int correct_coca_cola = 0;
	int total_coca_cola = 0;
	int correct_pepsi = 0;
	int total_pepsi = 0;

	// folder pentru fisiere care nu sunt clasificate corect
	std::filesystem::create_directory("misclassified");

	for (int i = 0; i < images->size; i++) {
		if (images->data[i].label == 0) {
			total_coca_cola++;
			if (images->data[i].generated_label == images->data[i].label) {
				correct_coca_cola++;
			}
			else {
				// le mut si le renumesc
				std::filesystem::rename(images->data[i].path, "misclassified/new_cola (" + std::to_string(i+1) + ").jpg.jpg");
			}
		}
		else if (images->data[i].label == 1) {
			total_pepsi++;
			if (images->data[i].generated_label == images->data[i].label) {
				correct_pepsi++;
			}
			else {
				// le mut si le renumesc
				std::filesystem::rename(images->data[i].path, "misclassified/new_pepsi (" + std::to_string(i+1) + ").jpg.jpg");
			}
		}
	}

	float accuracy_coca_cola = (float)correct_coca_cola / total_coca_cola;
	float accuracy_pepsi = (float)correct_pepsi / total_pepsi;

	print_accuracy_graph(accuracy_coca_cola, accuracy_pepsi);
}

void test_labels(IMAGES* images, int nr_of_test = 1, bool class_graph = false) {
	for (int i = 0; i < nr_of_test; i++) {
		assign_image_labels(images);
		calculate_accuracy(images);
		print_accuracy(images);
		if(class_graph) generate_class_accuracy_graph(images);
		Sleep(1000);
	}
}

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

int generate_label_binary(char* path) {
	cv::Mat r_bin_img = toBinary_Red(path);
	cv::Mat b_bin_img = toBinary_Blue(path);

	int height = r_bin_img.rows;
	int width = r_bin_img.cols;

	int r_black = 0, b_black = 0;

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			switch (r_bin_img.at<uchar>(i, j))
			{
			case 0: {
				r_black++;
				break;
			}
			default:
				break;
			}

			switch (b_bin_img.at<uchar>(i, j))
			{
			case 0: {
				b_black++;
				break;
			}
			default:
				break;
			}
		}

	return r_black > b_black ? 0 : 1;
}

int generate_label_colour(char* path) {
	cv::Mat img = cv::imread(path);

	std::pair<float, float> color_intensity = average_color_intensity(img);

	float red_intensity = color_intensity.first;
	float blue_intensity = color_intensity.second;

	//intensitate rosu mare inseamna coca cola si intensitate albastra mare inseamna pepsi
	return red_intensity > blue_intensity ? 0 : 1;
}

int generate_label_rand() {
	return rand() % 2;
}

void assign_image_labels(IMAGES* images) {
	srand(time(NULL));
	for (int i = 0; i < images->size; i++) {
		//images->data[i].generated_label = generate_label_rand();
		images->data[i].generated_label = generate_label_binary(images->data[i].path);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////Image management////////////////////////////////////////////////

cv::Mat toBinary_Red(char* img_path) {
	cv::Mat src = cv::imread(img_path, IMREAD_COLOR);

	int height = src.rows;
	int width = src.cols;

	Mat dst = Mat(height, width, CV_8UC1);

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			Vec3b colors = src.at<Vec3b>(i, j);
			if (colors[2] > colors[0] && abs(colors[2] - colors[1]) > DECISION_PRAG && abs(colors[2] - colors[0]) > DECISION_PRAG && colors[1] < 80) {
				dst.at<uchar>(i, j) = 0;
			}
			else {
				dst.at<uchar>(i, j) = 255;
			}
		}
	return dst;
}

cv::Mat toBinary_Blue(char* img_path) {
	cv::Mat src = cv::imread(img_path, IMREAD_COLOR);

	int height = src.rows;
	int width = src.cols;

	Mat dst = Mat(height, width, CV_8UC1);

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			Vec3b colors = src.at<Vec3b>(i, j);
			if (colors[0] > colors[2] && abs(colors[0] - colors[1]) > DECISION_PRAG && abs(colors[0] - colors[2]) > DECISION_PRAG) {
				dst.at<uchar>(i, j) = 0;
			}
			else {
				dst.at<uchar>(i, j) = 255;
			}
		}
	return dst;
}

//calculeaza intensitatea culorii pentru rosu si albastru
std::pair<float, float> average_color_intensity(cv::Mat image) {
	cv::Scalar mean = cv::mean(image);
	return { mean.val[2], mean.val[0] }; // mean.val[2] este rosu, mean.val[0] este albastru
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_float_precision(int precision = 2) {
	std::cout << std::fixed << std::setprecision(precision);
}

/*
* Se ocupa de deschiderea imaginilor din subdirectorul train
*/
void open_train_batch()
{
	IMAGES* images_train = initialize_image_array();
	open_images(images_train, 1, "cola", "train_images");
	open_images(images_train, 1, "pepsi", "train_images");
	test_labels(images_train, 1, true);
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
	test_labels(images_test, 1, true);
}

int main()
{
	set_float_precision();
	open_train_batch();
	//open_test_batch();
	return 0;
}