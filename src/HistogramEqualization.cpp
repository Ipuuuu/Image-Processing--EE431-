#include "Image.h"
#include <iostream>

int main()
{
    GrayscaleImage I;
    I.Load("images/lena.png");
    int w, h;
    h = I.GetHeight();
    w = I.GetWidth();

    int pixNum = h * w;

    int bit = 0;
    std::cout << "How many bit image is this? ";
    std::cin >> bit;  
    std::cout << std::endl;

    int max_intensity = (1 << bit) - 1;
    double minval = max_intensity, maxval=0;
    int hist[max_intensity + 1] = {};
    float PDF[max_intensity + 1] = {};
    float CDF[max_intensity + 1] = {};

    GrayscaleImage O(w, h);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            hist[I(x,y)] += 1;
        }
    }

    for(int i = 0; i < max_intensity+1; i++){
        PDF[i] = float( hist[i] )/ pixNum;
    }
    CDF[0] = PDF[0];
    for(int i = 1; i < max_intensity+1; i++){
        CDF[i] = CDF[i-1] + PDF[i];
    }
    for(int i = 0; i < max_intensity+1; i++){
        CDF[i] = CDF[i] * max_intensity;
    }


    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
           
            O(x,y) = int(std::round(CDF[I(x,y)]));
            
        }
    }

    O.Save("images/lenaHEq.png");

    return 0;
}
