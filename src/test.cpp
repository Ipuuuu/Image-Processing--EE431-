#include "Image.h"

int main(){
    GrayscaleImage input;
    input.Load("images/lena.png");
    int w, h;
    h = input.GetHeight(); w = input.GetWidth();

    GrayscaleImage output(w, h);

    for(int y = 0; y < h; y++){
        for(int x =0; x < w; x++){

            output(x,y) = input.Get(x,y);

        }
    }

    output.Save("images/lenasame.png");

    return 0;
}