#include "Image.h"

int main()
{
    ColorImage i;
    i.Load("images/lena.png");
    GrayscaleImage gray;
    gray = i;

    int height = i.GetHeight();
    int width = i.GetWidth();
    float sat[width][height] = {};
    int max = 0;
    int min = 255;
    float t = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            RGBA pix = i(x, y);
            float avg = (pix.r + pix.b + pix.g) / 3.0f;

            max = pix.b, min = pix.b;
            max = max > pix.r ? (max > pix.g ? max : pix.g) : (pix.r > pix.g ? pix.r : pix.g);
            min = min < pix.r ? (min < pix.g ? min : pix.g) : (pix.r < pix.g ? pix.r : pix.g);

            if (avg == 0)
                sat[x][y] = 0;
            else
                sat[x][y] = ((max - min) / avg);

            t += sat[x][y];
        }
    }

    t = t / (height * width);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (sat[x][y] < t)
                gray(x, y) = 0;
            else
                gray(x, y) = 255;
        }
    }

    gray.Save("images/finals.png");

    return 0;
}
