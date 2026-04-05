#include "Image.h"
#include <iostream>

int main()
{
    GrayscaleImage I;
    I.Load("images/lena.png");
    int w, h;
    h = I.GetHeight();
    w = I.GetWidth();

    int bit = 0;
    std::cout << "How many bit image is this? ";
    std::cin >> bit;  
    std::cout << std::endl;

    int max_intensity = (1 << bit) - 1;
    double minval = max_intensity, maxval=0;
    int hist[max_intensity + 1] = {};

    GrayscaleImage O(w, h);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int pix = I(x,y);

            //to find minval & maxval
            if(pix < minval) minval = pix;
            if(pix > maxval) maxval = pix;

            /*
            // to find histogram not necessary for just contrast streching
            hist[I(x,y)] += 1;
            }*/
        }
    }

/*
    // how to find maxval and minval using hist
    for(int i = 0; i <= max_intensity; i++){
        if(hist[i] != 0) {
            maxval = i;
        }
    }
    
    
    for(int i = 0; i <= max_intensity; i++){
        if( hist[i] != 0) {
            minval = i;
            break;
        }
    }
  
*/

    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            O(x,y) = int(((I(x,y) - minval) / (maxval - minval)) * max_intensity);
        }
    }

    O.Save("images/lenaCS.png");

    return 0;
}
