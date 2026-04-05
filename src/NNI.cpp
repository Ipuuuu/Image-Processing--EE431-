#include "Image.h"

int main(){

    GrayscaleImage input;
    input.Load("images/lena.png");
    GrayscaleImage output(input.GetWidth(), input.GetHeight());

    float sx = input.GetWidth()/output.GetWidth();
    float sy = input.GetHeight()/output.GetHeight();

    for(int y=0; y < output.GetHeight(); y++){
        for(int x=0; x < output.GetWidth(); x++){
            output(x,y) = input(static_cast<int> (x * sx), static_cast<int> (y * sy));
        }
    }

    output.Save("images/lenaNNI.png");

    return 0;
}