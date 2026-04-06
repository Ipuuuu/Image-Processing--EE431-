#include "Image.h"
#include <iostream>

/*Write a function called Halve that will shrink a given image into half its size using a special interpolation
technique. In this technique, you should get the average of 4 pixels and write it as the output including (0, 0). Hint:
Details are limited as it is your job to figure out how to perform this operation. Coding part is quite simple.*/

void Halve(GrayscaleImage I){
    int x = I.GetWidth(); int y = I.GetHeight();
    // float sx = x/(x/2), sy = y/(y/2);

    GrayscaleImage O(x/2, y/2);

    std::cout << "Ix: " << I.GetWidth() << " Iy: " << I.GetHeight() << std::endl;
    std::cout << "Ox: " << O.GetWidth() << " Oy: " << O.GetHeight() << std::endl;

    for(int y = 0; y < O.GetHeight(); y++ ){
        for(int x = 0; x < O.GetWidth(); x++ ){

            float val = float(I(x,y) + I(x+1, y)+
                              I(x, y+1) + I(x+1, y+1)) / 4;

            O(x,y) = int(val);
        }
    }
   
    O.Save("images/lenaMidterms.png");


}

int main(){
    GrayscaleImage I;
    I.Load("images/lena.png");

    Halve(I);

    return 0;
}