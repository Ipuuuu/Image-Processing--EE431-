#include "Image.h"
#include <iostream>

int main()
{
    GrayscaleImage I;
    I.Load("images/lena.png");

    double gamma = 0;
    int bit = 0;

    std::cout << "Enter gamma value (0-1): ";
    std::cin >> gamma;

    std::cout << "Enter bit image value: ";
    std::cin >> bit;

    int max_intensity = ((1 << bit) - 1);

    GrayscaleImage O(I.GetWidth(), I.GetHeight());

    for (int y = 0; y < I.GetHeight(); y++)
    {
        for (int x = 0; x < I.GetWidth(); x++)
        {
            float normalized = ((float)I(x, y)) / max_intensity;
            // Ox,y = (2^B - 1) * (I(x, y) / (2^B - 1))^γ
            O(x, y) = std::pow((normalized / max_intensity), gamma) * max_intensity;
        }
    }

    O.Save("images/lenaGamma.png");

    return 0;
}