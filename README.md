# _Coca-Cola | Pepsi Label Generator_

### Main Idea:
- Create an "app" that could determine wheter an image contains a coca-cola/pepsi beverage.
- Experiment different ways to aproach the segregation process.

### Approach:
Create a datastructure that allows us to store all essential data about an image (path, correct label, generated label) and use a multitude of methods to interpret said images.

| Label Generation Idea | Observations |
| ------ | ------ |
|  Random | One of the easiest ways to generate an image label would be to just randomise it. In our case it could be an ok approach, since we don't have a lot of possible labels (only 2), but if the project needs scaling, this idea would soon fall apart, and become useless. |
| Color Maximum | The basic idea of this approach is to verify in each image if the mean colour of the image has more red or more blue in it. |
| Binary Images| Generating two binary images from is source image is a viable strategy, although it depends a lot on the surrounding elemnts in the image. The idea is to generate a "red" binary image where all red pixels become black and all other colours become white (then we do the same for a "blue" binary image). The final step is to verify which of the images has more black pixels, thus getting our label. |

## 1.1 Random
![random confusion matrix](https://raw.githubusercontent.com/alexFban/PI_Pespi_Cola/main/md_images/random_conf_mat.PNG)

We can observe that in the random label generation approach the results are as we expected: we get an accuracy approximately equal to 50%.

## 1.2 Color Maximum
![color maximum confusion matrix](https://raw.githubusercontent.com/alexFban/PI_Pespi_Cola/main/md_images/color_maximum_conf_mat.PNG)

In our second approach the numbers tilted in our favour, increasing the accuracy from aprox. 50% to 65% on the train images and 79% on the test images.

## 1.3 Binary Images
![binary images confusion matrix](https://raw.githubusercontent.com/alexFban/PI_Pespi_Cola/main/md_images/binary_conf_mat.PNG)

On our final approach is were things get interesting. First we tried to use only the binary images, and it worked great, but there were some images that were pretty obviously misclassified due to our strategy. Let's take the next image as an example: 

![example image 1](https://raw.githubusercontent.com/alexFban/PI_Pespi_Cola/main/md_images/new_cola_15.jpg)

If we applied our non-modified version of the binary code, we would get these binary images:

![example image 1 red binary](https://raw.githubusercontent.com/alexFban/PI_Pespi_Cola/main/md_images/new_cola_15_rb.PNG)

![example image 1 blue binary](https://raw.githubusercontent.com/alexFban/PI_Pespi_Cola/main/md_images/new_cola_15_bb.PNG)

We can observe that on the blue binary image there are some "artifacts". Those black squares are detected as being blue-ish even though they should be white. That is because of some subtle shadows that are present in the original image. An approach that we used for this is to "shrink" the visible image to a more managable size. Using this approach we eliminated the edges of the images, the images after this modifications being the next:

![example image 1 red binary shrunk](https://raw.githubusercontent.com/alexFban/PI_Pespi_Cola/main/md_images/new_cola_15_rb_shrunk.PNG)

![example image 1 blue binary shrunk](https://raw.githubusercontent.com/alexFban/PI_Pespi_Cola/main/md_images/new_cola_15_bb_shrunk.PNG)

Finally we got rid of those artifacts that made it seem like there was more blue than red. But we still had a problem. What if there was an image of a coca-cola can taken underwater? Well, there is, and the program still labeled it wrong. So the solution we used for this problem was to verify the size of the black "objects" in each binary image. If the "object" was bigger than a selected threshold then we reverse-labeled the image. Here is an example:

![example image 2](https://raw.githubusercontent.com/alexFban/PI_Pespi_Cola/main/md_images/new_cola_88.jpg)

![example image 2 blue binary shrunk](https://raw.githubusercontent.com/alexFban/PI_Pespi_Cola/main/md_images/new_cola_88_bb_shrunk.PNG)

In this blue binary image we can observe that there appears to be a black box, which is actually the ocean, so we run the image thru a method that tells us if the box is too big, and if the answer is yes, then we can ignore the black box.