#include "Image.h"
#include <iostream>

int main()
{
    GrayscaleImage I;
    I.Load("images/lena.png");

    int contrast = 0;
    int bit = 0;

    std::cout << "Enter contrast value: ";
    std::cin >> contrast;

    std::cout << "Enter bit image value: ";
    std::cin >> bit;

    // debug
    std::cout << "2^( B-1 ): " << (1 << (bit - 1)) << std::endl;
    std::cout << "2^( B-1 ): " << std::pow(2, bit - 1) << std::endl;

    GrayscaleImage O(I.GetWidth(), I.GetHeight());

    for (int y = 0; y < I.GetHeight(); y++)
    {
        for (int x = 0; x < I.GetWidth(); x++)
        {
            // Ox,y = C(Ix,y - 2^(B-1)) + 2^(B-1)
            O(x, y) = contrast * (I.Get(x, y) - (1 << (bit - 1))) + (1 << (bit - 1));
            // O(x, y) = contrast * (I(x, y) - (std::pow(2, bit - 1))) + std::pow(2, bit - 1);
        }
    }

    O.Save("images/lenaContrast.png");

    return 0;
}