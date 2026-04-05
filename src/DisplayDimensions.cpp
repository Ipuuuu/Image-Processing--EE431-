#include "Image.h"
#include <iostream>

using namespace std;

int main(){
    GrayscaleImage input;
    input.Load("images/lena.png");

    cout << "Width: " << input.GetWidth() << " Height: " << input.GetHeight() << endl;

    return 0;

}