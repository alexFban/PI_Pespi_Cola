// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <utility> 
#include <vector>
#include <string>
#include <filesystem>

typedef struct ImageData {
    std::string path;
    std::string label;
} ImageData;

typedef struct IMAGES {
    std::vector<ImageData> data;
    int size;
    int max_size;
} IMAGES;

bool index_out_of_bounds(IMAGES images) {
    return images.max_size <= images.size;
}

bool image_exists(std::string path) {
    cv::Mat image = cv::imread(path, cv::IMREAD_COLOR);
    if (!image.empty()) {
        std::cout << "Image opened successfully: " << path << std::endl;
        return true;
    }
    else {
        std::cout << "Failed to open image: " << path << std::endl;
        return false;
    }
}

void resize_image_array(IMAGES* images) {
    images->max_size += 100;
    images->data.resize(images->max_size);
}

std::vector<ImageData> open_images(int starting_number, std::string bv_type, std::string dname) {
    std::vector<ImageData> image_data;
    int number = starting_number;

   
    while (1) {
        ImageData img_data;
        img_data.path = ".\\Images\\" + dname + "\\new_" + bv_type + " (" + std::to_string(number++) + ").jpg.jpg";
        if (!image_exists(img_data.path)) {
            std::cout << "Failed to open image: " << img_data.path << std::endl;
            break;
        }
       

        // Extract label from the file name
        if (bv_type == "cola") {
            img_data.label = "cola";
           
        }
        else if (bv_type == "pepsi") {
            img_data.label = "pepsi";
          
        }
        image_data.push_back(img_data);
    }
    
    return image_data;
}

int main() {
    IMAGES images;
    images.max_size = 100;
    images.size = 0;
    resize_image_array(&images);

    std::vector<ImageData> cola_images = open_images(1, "cola", "train_images");
    std::vector<ImageData> pepsi_images = open_images(1, "pepsi", "train_images");

    images.data.insert(images.data.end(), cola_images.begin(), cola_images.end());
    images.data.insert(images.data.end(), pepsi_images.begin(), pepsi_images.end());

    // Print image paths and labels
    for (const auto& img_data : images.data) {
        std::cout << "Image Path: " << img_data.path << ", Label: " << img_data.label << std::endl;
    }

    std::string label_of_image_40;
    for (const auto& img_data : images.data) {
        // Check if image number is 40 and label is "cola"
        if (img_data.path.find("new_cola (40)") != std::string::npos && img_data.label == "cola") {
            label_of_image_40 = img_data.label;
            break; // Exit loop once found
        }
    }

    // Print label of image with number 40 from cola image training set
    std::cout << "Label of image with number 40 from cola image training set: " << label_of_image_40 << std::endl;


   

    return 0;
}