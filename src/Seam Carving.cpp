#include "Image.h"
#include <vector>
#include <float.h>

int main()
{

    ColorImage i;
    i.Load("images/towerColor.png");

    int width = i.GetWidth();
    int height = i.GetHeight();

    char debugChoice;
    std::cout << "Show seams during processing? (y/n): ";
    std::cin >> debugChoice;
    bool showSeams = (debugChoice == 'y');

    std::cout << "Image size: " << width << " x " << height << std::endl;

    int newTargetWidth, newTargetHeight;
    std::cout << "Enter target width: ";
    std::cin >> newTargetWidth;
    std::cout << "Enter target height: ";
    std::cin >> newTargetHeight;

    // determine mode from target vs current size
    bool shrinkWidth = newTargetWidth < width;
    int targetWidth = newTargetWidth;

    ColorImage seamAccum = i; // copy of original, will accumulate all seam lines
    int iteration = 0;

    std::vector<float> energy(width * height);
    std::vector<std::vector<int>> seamList;

    if (!shrinkWidth && width != targetWidth)
    {
        int k = abs(targetWidth - width);

        // compute initial energy once
        GrayscaleImage g(i);
        energy.resize(width * height);
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                float Gx = (1) * g.Get(car(x - 1, width - 1), car(y - 1, height - 1)) + (-1) * g.Get(car(x + 1, width - 1), car(y - 1, height - 1)) + (2) * g.Get(car(x - 1, width - 1), car(y, height - 1)) + (-2) * g.Get(car(x + 1, width - 1), car(y, height - 1)) + (1) * g.Get(car(x - 1, width - 1), car(y + 1, height - 1)) + (-1) * g.Get(car(x + 1, width - 1), car(y + 1, height - 1));
                float Gy = (1) * g.Get(car(x - 1, width - 1), car(y - 1, height - 1)) + (2) * g.Get(car(x, width - 1), car(y - 1, height - 1)) + (1) * g.Get(car(x + 1, width - 1), car(y - 1, height - 1)) + (-1) * g.Get(car(x - 1, width - 1), car(y + 1, height - 1)) + (-2) * g.Get(car(x, width - 1), car(y + 1, height - 1)) + (-1) * g.Get(car(x + 1, width - 1), car(y + 1, height - 1));
                energy[y * width + x] = abs(Gx) + abs(Gy);
            }
        }

        // find k seams on original image
        std::vector<float> tempEnergy = energy;
        std::vector<float> tempM(width * height);

        for (int s = 0; s < k; s++)
        {
            // DP
            for (int x = 0; x < width; x++)
                tempM[x] = tempEnergy[x];
            for (int y = 1; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    float left = (x > 0) ? tempM[(y - 1) * width + (x - 1)] : FLT_MAX;
                    float up = tempM[(y - 1) * width + x];
                    float right = (x < width - 1) ? tempM[(y - 1) * width + (x + 1)] : FLT_MAX;
                    float minParent = std::min({left, up, right});
                    tempM[y * width + x] = (minParent >= FLT_MAX / 2) ? FLT_MAX : tempEnergy[y * width + x] + minParent;
                }
            }

            // find min in last row
            int minCol = 0;
            float minVal = tempM[(height - 1) * width];
            for (int x = 1; x < width; x++)
                if (tempM[(height - 1) * width + x] < minVal)
                {
                    minVal = tempM[(height - 1) * width + x];
                    minCol = x;
                }

            // backtrack
            std::vector<int> currentSeam(height);
            currentSeam[height - 1] = minCol;
            for (int y = height - 2; y >= 0; y--)
            {
                int prevCol = currentSeam[y + 1];
                int bestCol = prevCol;
                float bestVal = tempM[y * width + prevCol];
                if (prevCol > 0 && tempM[y * width + (prevCol - 1)] < bestVal)
                {
                    bestVal = tempM[y * width + (prevCol - 1)];
                    bestCol = prevCol - 1;
                }
                if (prevCol < width - 1 && tempM[y * width + (prevCol + 1)] < bestVal)
                {
                    bestVal = tempM[y * width + (prevCol + 1)];
                    bestCol = prevCol + 1;
                }
                currentSeam[y] = bestCol;
            }

            seamList.push_back(currentSeam);

            // block this seam from being picked again
            for (int y = 0; y < height; y++)
                tempEnergy[y * width + currentSeam[y]] = FLT_MAX;
        }

        // draw all seams on seamAccum for debug
        if (showSeams)
        {
            for (int s = 0; s < (int)seamList.size(); s += std::max(1, k / 20))
            {
                for (int y = 0; y < height; y++)
                    seamAccum(seamList[s][y], y) = RGBA(0, 255, 0, 255); // green for insertion seams
            }
            seamAccum.Save("images/seams_insertion_debug.png");
        }

        // simultaneous insertion
        ColorImage result(width + k, height);
        for (int y = 0; y < height; y++)
        {
            std::vector<int> cols;
            for (auto &s : seamList)
                cols.push_back(s[y]);
            std::sort(cols.begin(), cols.end());

            int dstX = 0;
            int seamIdx = 0;
            for (int srcX = 0; srcX < width; srcX++)
            {
                result(dstX++, y) = i(srcX, y);
                // insert all seams that fall at this srcX
                while (seamIdx < (int)cols.size() && cols[seamIdx] == srcX)
                {
                    RGBA left = i(srcX, y);
                    RGBA right = i.Get(srcX + 1, y);
                    result(dstX++, y) = RGBA(
                        (left.r + right.r) / 2,
                        (left.g + right.g) / 2,
                        (left.b + right.b) / 2);
                    seamIdx++;
                }
            }
        }

        i = result;
        width = width + k;
    }

    std::vector<float> M(width * height);
    std::vector<int> seam(height);

    while (width != targetWidth)
    {
        energy.resize(width * height);
        M.resize(width * height);

        GrayscaleImage g(i);
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                float Gx = (1) * g.Get(car(x - 1, width - 1), car(y - 1, height - 1)) + (-1) * g.Get(car(x + 1, width - 1), car(y - 1, height - 1)) + (2) * g.Get(car(x - 1, width - 1), car(y, height - 1)) + (-2) * g.Get(car(x + 1, width - 1), car(y, height - 1)) + (1) * g.Get(car(x - 1, width - 1), car(y + 1, height - 1)) + (-1) * g.Get(car(x + 1, width - 1), car(y + 1, height - 1));
                float Gy = (1) * g.Get(car(x - 1, width - 1), car(y - 1, height - 1)) + (2) * g.Get(car(x, width - 1), car(y - 1, height - 1)) + (1) * g.Get(car(x + 1, width - 1), car(y - 1, height - 1)) + (-1) * g.Get(car(x - 1, width - 1), car(y + 1, height - 1)) + (-2) * g.Get(car(x, width - 1), car(y + 1, height - 1)) + (-1) * g.Get(car(x + 1, width - 1), car(y + 1, height - 1));
                energy[y * width + x] = abs(Gx) + abs(Gy);
            }
        }

        // First row, copy energy directly
        for (int x = 0; x < width; x++)
        {
            M[0 * width + x] = energy[0 * width + x];
        }

        // Fill downward
        for (int y = 1; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                float left = (x > 0) ? M[(y - 1) * width + (x - 1)] : FLT_MAX;
                float up = M[(y - 1) * width + x];
                float right = (x < width - 1) ? M[(y - 1) * width + (x + 1)] : FLT_MAX;

                M[y * width + x] = energy[y * width + x] + std::min({left, up, right});
            }
        }

        // find the column with minimum cost in the last row
        int minCol = 0;
        float minVal = M[(height - 1) * width + 0];

        for (int x = 1; x < width; x++)
        {
            if (M[(height - 1) * width + x] < minVal)
            {
                minVal = M[(height - 1) * width + x];
                minCol = x;
            }
        }

        // that's where the seam exits
        seam[height - 1] = minCol;

        // backtrack upward
        for (int y = height - 2; y >= 0; y--)
        {
            int prevCol = seam[y + 1]; // column we came from in the row below

            // check the three parents above prevCol
            int bestCol = prevCol; // assume straight up first
            float bestVal = M[y * width + prevCol];

            if (prevCol > 0 && M[y * width + (prevCol - 1)] < bestVal)
            {
                bestVal = M[y * width + (prevCol - 1)];
                bestCol = prevCol - 1;
            }
            if (prevCol < width - 1 && M[y * width + (prevCol + 1)] < bestVal)
            {
                bestVal = M[y * width + (prevCol + 1)];
                bestCol = prevCol + 1;
            }

            seam[y] = bestCol;
        }

        if (showSeams && iteration % 20 == 0)
        {
            for (int y = 0; y < height; y++)
                seamAccum(seam[y], y) = RGBA(255, 0, 0, 255);
        }

        if (shrinkWidth)
        {
            // removal block
            int newWidth = width - 1;
            ColorImage result(newWidth, height);
            for (int y = 0; y < height; y++)
            {
                int seamX = seam[y];
                for (int x = 0; x < seamX; x++)
                    result(x, y) = i(x, y);
                for (int x = seamX + 1; x < width; x++)
                    result(x - 1, y) = i(x, y);
            }
            i = result;
            width = newWidth;
        }
        else
        {
            int newWidth = width + 1;
            ColorImage result(newWidth, height);
            for (int y = 0; y < height; y++)
            {
                int seamX = seam[y];
                for (int x = 0; x < seamX; x++)
                    result(x, y) = i(x, y);
                RGBA left = i(seamX, y);
                RGBA right = i.Get(seamX + 1, y);
                result(seamX, y) = RGBA((left.r + right.r) / 2, (left.g + right.g) / 2, (left.b + right.b) / 2);
                for (int x = seamX; x < width; x++)
                    result(x + 1, y) = i(x, y);
            }
            i = result;
            width = newWidth;
        }
        iteration++;
    }
    if (showSeams)
        seamAccum.Save("images/seams_accumulated2.png");

    i.Save("images/tower_seam4.png");
    return 0;
}