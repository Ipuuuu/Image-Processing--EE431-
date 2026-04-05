#include "Image.h"
#include <string.h>
#include <algorithm>

/*Write a C++ function or pseudo-code named IsolateBackground that takes
 an 8-bit gray scale image as input. Assume the most frequent intensity
  in the image reps the background. Your func should:

a) Calculate histogram to find the most freq intensity value.
b) Create & return a new binary image where all pixel matching that exact
background intensity are set to 255 (true), & all other pixels are set to 0 (false)*/

void IsolateBackground(std::string filepath){

    GrayscaleImage I;
    I.Load(filepath);

    GrayscaleImage O(I.GetWidth(), I.GetHeight());
    int maxval = 0;

    int hist[256] = {};

    for(int y = 0; y < I.GetHeight(); y++){
        for(int x = 0; x < I.GetWidth(); x ++){
            hist[I(x,y)] += 1;
        }
    }

    int *maxptr = std::max_element(hist, hist + 256);
    maxval = maxptr - hist;

    for(int y = 0; y < I.GetHeight(); y++){
        for(int x = 0; x < I.GetWidth(); x ++){
             if(I(x,y) == maxval){
                O(x,y) = 255;
             }
             else{
                O(x,y) = 0;
             }
        }
    }

    O.Save("images/LenaQuiz.png");

}

int main(){

    IsolateBackground("images/lena.png");

    return 0;
}