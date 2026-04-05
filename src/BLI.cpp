#include "Image.h"
#include <math.h>

int main()
{

    GrayscaleImage input;
    input.Load("images/lena.png");

    GrayscaleImage output(400, 400);
    float sx = (float)input.GetWidth() / 400, sy = (float)input.GetHeight() / 400;

    for (int y = 0; y < output.GetHeight(); y++)
    {
        for (int x = 0; x < output.GetWidth(); x++)
        {

            float x_ = x * sx, y_ = y * sy;
            float fx = x_ - floor(x_), fy = y_ - floor(y_);

            // float val = ((1 - fx) * (1 - fy) * input.Get(car(x_, input.GetWidth()), car(y_, input.GetHeight()))) +
            //             (fx * (1 - fy) * input.Get(car(x_ + 1, input.GetWidth()), car(y_, input.GetHeight()))) +
            //             ((1 - fx) * fy * input.Get(car(x_, input.GetWidth()), car(y_ + 1, input.GetHeight()))) + 
            //             (fx * fy * input.Get(car(x_ + 1, input.GetWidth()), car(y_ + 1, input.GetHeight())));

             float val = ((1 - fx) * (1 - fy) * input.Get(x_, y_)) +
                        (fx * (1 - fy) * input.Get(x_ + 1, y_)) +
                        ((1 - fx) * fy * input.Get(x_, y_ + 1))+ 
                        (fx * fy * input.Get(x_ + 1, y_ + 1));

            output(x, y) = (val);
        }
    }

    output.Save("images/lenaBLI.png");

    return 0;
}