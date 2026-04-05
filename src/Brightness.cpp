#include "Image.h"
#include <iostream>

int main()
{
    GrayscaleImage I;
    I.Load("images/lena.png");

    int brightness = 0;

    std::cout << "Enter brightness value: ";
    std::cin >> brightness;

    GrayscaleImage O(I.GetWidth(), I.GetHeight());

    for (int y = 0; y < I.GetHeight(); y++)
    {
        for (int x = 0; x < I.GetWidth(); x++)
        {
            //Ox,y = Ix,y + b
            O(x, y) = I(x, y) + brightness;
        }
    }

    O.Save("images/lenaBright.png");

    return 0;
}