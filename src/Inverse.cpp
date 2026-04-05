#include "Image.h"
#include <iostream>

int main()
{
    GrayscaleImage I;
    I.Load("images/lena.png");

    int bit = 0;

    std::cout << "Enter bit image value: ";
    std::cin >> bit;

    int max_intensity = ( (1<<bit) - 1);

    GrayscaleImage O(I.GetWidth(), I.GetHeight());

    for (int y = 0; y < I.GetHeight(); y++)
    {
        for (int x = 0; x < I.GetWidth(); x++)
        {
            // Ox,y = (2^B - 1) - Ix,y
            O(x,y) = max_intensity - I(x,y);

        }
    }

    O.Save("images/lenaInverse.png");

    return 0;
}