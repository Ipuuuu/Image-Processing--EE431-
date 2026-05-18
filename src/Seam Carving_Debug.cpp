#include "Image.h"
#include <vector>
#include <float.h>
#include <functional>

int width, height;
int targetWidth;
int newTargetWidth, newTargetHeight;
bool showSeams = false;
bool shrinkWidth = false;

ColorImage i, seamAccum;
int iteration = 0;

// Persistent grayscale and energy — only recompute what changes
std::vector<float> energy(width *height), M(width *height), gray(width *height);
std::vector<int> seam(height);
std::vector<std::vector<int>> seamList;

float computeEnergy(int x, int y, int w);
ColorImage transpose(const ColorImage &src);
void removeSeams();
void insertSeams();

int main()
{

    i.Load("images/towerColor.png");

    width = i.GetWidth();
    height = i.GetHeight();
    seam.resize(height);

    char debugChoice;
    std::cout << "Show seams during processing? (y/n): ";
    std::cin >> debugChoice;
    showSeams = (debugChoice == 'y');

    std::cout << "Image size: " << width << " x " << height << std::endl;

    std::cout << "Enter target width: ";
    std::cin >> newTargetWidth;
    std::cout << "Enter target height: ";
    std::cin >> newTargetHeight;

    // determine mode from target vs current size
    shrinkWidth = newTargetWidth < width;
    targetWidth = newTargetWidth;

    seamAccum = i; // copy of original, will accumulate all seam lines

    if (!shrinkWidth && width != targetWidth)
    {
        while (width < targetWidth)
        {
            insertSeams();
        }

        if (showSeams)
            seamAccum.Save("images/seams_width_debug.png");

        // height phase
        if (height != newTargetHeight)
        {
            seamAccum = i;
            i = transpose(i);
            seamAccum = transpose(seamAccum);

            std::swap(width, height); // width is old height, height is old width
            seam.resize(height);

            // reinitialize gray and energy for the new orientation
            {
                GrayscaleImage g(i);
                gray.assign(width * height, 0);
                for (int y = 0; y < height; y++)
                    for (int x = 0; x < width; x++)
                        gray[y * width + x] = g.Get(x, y);
                energy.resize(width * height);
                for (int y = 0; y < height; y++)
                    for (int x = 0; x < width; x++)
                        energy[y * width + x] = computeEnergy(x, y, width);
            }

            targetWidth = newTargetHeight;
            shrinkWidth = (targetWidth < width);
            if (shrinkWidth)
            {
                while (width != targetWidth)
                    removeSeams();
            }
            else
            {
                while (width < targetWidth)
                    insertSeams();
            }

            i = transpose(i);
            seamAccum = transpose(seamAccum);
            std::swap(width, height); // restore orientation
            seam.resize(height);

            if (showSeams)
                seamAccum.Save("images/seams_height_debug.png");
        }
    }

    if (shrinkWidth)
    {
        GrayscaleImage g(i);
        gray.resize(width * height);
        for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++)
                gray[y * width + x] = g.Get(x, y);

        energy.resize(width * height);
        for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++)
                energy[y * width + x] = computeEnergy(x, y, width);

        while (width != targetWidth)
            removeSeams();

        if (showSeams)
            seamAccum.Save("images/seams_width_debug.png");
    }

    // height phase
    if (height != newTargetHeight)
    {
        seamAccum = i;
        i = transpose(i);
        seamAccum = transpose(seamAccum);
        std::swap(width, height); // width is old height, height is old width
        seam.resize(height);

        // reinitialize gray and energy for the new orientation
        {
            GrayscaleImage g(i);
            gray.assign(width * height, 0);
            for (int y = 0; y < height; y++)
                for (int x = 0; x < width; x++)
                    gray[y * width + x] = g.Get(x, y);
            energy.resize(width * height);
            for (int y = 0; y < height; y++)
                for (int x = 0; x < width; x++)
                    energy[y * width + x] = computeEnergy(x, y, width);
        }

        targetWidth = newTargetHeight;
        shrinkWidth = (targetWidth < width);
        if (shrinkWidth)
        {
            while (width != targetWidth)
                removeSeams();
        }
        else
        {
            while (width < targetWidth)
                insertSeams();
        }

        i = transpose(i);
        seamAccum = transpose(seamAccum);
        std::swap(width, height); // restore orientation
        seam.resize(height);

        if (showSeams)
            seamAccum.Save("images/seams_height_debug.png");
    }

    std::cout << "Saving output, width=" << width << " height=" << height << std::endl;

    i.Save("images/tower_seam3.png");
    return 0;
}

float computeEnergy(int x, int y, int w)
{
    auto clamp = [](int v, int lo, int hi)
    { return std::max(lo, std::min(hi, v)); };
    float Gx = gray[clamp(y - 1, 0, height - 1) * w + clamp(x - 1, 0, w - 1)] * 1 + gray[clamp(y - 1, 0, height - 1) * w + clamp(x + 1, 0, w - 1)] * -1 + gray[clamp(y, 0, height - 1) * w + clamp(x - 1, 0, w - 1)] * 2 + gray[clamp(y, 0, height - 1) * w + clamp(x + 1, 0, w - 1)] * -2 + gray[clamp(y + 1, 0, height - 1) * w + clamp(x - 1, 0, w - 1)] * 1 + gray[clamp(y + 1, 0, height - 1) * w + clamp(x + 1, 0, w - 1)] * -1;
    float Gy = gray[clamp(y - 1, 0, height - 1) * w + clamp(x - 1, 0, w - 1)] * 1 + gray[clamp(y - 1, 0, height - 1) * w + clamp(x, 0, w - 1)] * 2 + gray[clamp(y - 1, 0, height - 1) * w + clamp(x + 1, 0, w - 1)] * 1 + gray[clamp(y + 1, 0, height - 1) * w + clamp(x - 1, 0, w - 1)] * -1 + gray[clamp(y + 1, 0, height - 1) * w + clamp(x, 0, w - 1)] * -2 + gray[clamp(y + 1, 0, height - 1) * w + clamp(x + 1, 0, w - 1)] * -1;
    return std::abs(Gx) + std::abs(Gy);
};

ColorImage transpose(const ColorImage &src)
{
    int w = src.GetWidth(), h = src.GetHeight();
    ColorImage dst(h, w); // note: flipped dimensions
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            dst(y, x) = src(x, y);
    return dst;
}

void insertSeams()
{

    // How many seams to find this pass (at most half current width)
    int k = std::min(targetWidth - width, width / 2);

    seamList.clear();

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

    // border penalty
    for (int y = 0; y < height; y++)
    {
        energy[y * width + 0] += 1e6f;
        energy[y * width + width - 1] += 1e6f;
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
            for (int x = 0; x < width; x++)
            {
                float left = (x > 0) ? tempM[(y - 1) * width + (x - 1)] : FLT_MAX;
                float up = tempM[(y - 1) * width + x];
                float right = (x < width - 1) ? tempM[(y - 1) * width + (x + 1)] : FLT_MAX;
                float minP = std::min({left, up, right});
                tempM[y * width + x] = (minP >= FLT_MAX / 2) ? FLT_MAX
                                                             : tempEnergy[y * width + x] + minP;
            }

        // find min last row
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
            int p = currentSeam[y + 1];
            int best = p;
            float bv = tempM[y * width + p];
            if (p > 0 && tempM[y * width + (p - 1)] < bv)
            {
                bv = tempM[y * width + (p - 1)];
                best = p - 1;
            }
            if (p < width - 1 && tempM[y * width + (p + 1)] < bv)
            {
                bv = tempM[y * width + (p + 1)];
                best = p + 1;
            }
            currentSeam[y] = best;
        }

        // collision check
        bool collision = false;
        for (int y = 0; y < height; y++)
            if (tempEnergy[y * width + currentSeam[y]] >= FLT_MAX / 2)
            {
                collision = true;
                break;
            }
        if (collision)
        {
            k = s; // only insert what we found
            break;
        }

        seamList.push_back(currentSeam);
        for (int y = 0; y < height; y++)
            tempEnergy[y * width + currentSeam[y]] = FLT_MAX;
    }
    std::cout << "Pass: inserting " << k << " seams into width=" << width << std::endl;

    // Single pass: insert all k seams at once
    k = seamList.size();
    ColorImage result(width + k, height);

    for (int y = 0; y < height; y++)
    {
        // get all seam columns at this row, sort them
        std::vector<std::pair<int, int>> seamsAtY(k); // (original col, seam index)
        for (int s = 0; s < k; s++)
            seamsAtY[s] = {seamList[s][y], s};
        std::sort(seamsAtY.begin(), seamsAtY.end());

        int srcX = 0; // current position in source image
        int dstX = 0; // current position in result image
        int si = 0;   // index into sorted seams

        while (srcX < width)
        {
            // insert any seams whose original column == srcX (before copying srcX)
            while (si < k && seamsAtY[si].first == srcX)
            {
                // blend: left neighbor is what we just wrote, right is srcX
                RGBA left = (dstX > 0) ? result(dstX - 1, y) : i(srcX, y);
                RGBA right = i(srcX, y);
                result(dstX, y) = RGBA(
                    (left.r + right.r) / 2,
                    (left.g + right.g) / 2,
                    (left.b + right.b) / 2, 255);
                dstX++;
                si++;
            }
            result(dstX, y) = i(srcX, y);
            dstX++;
            srcX++;
        }
        // flush any remaining seams at the right edge
        while (si < k)
        {
            RGBA left = (dstX > 0) ? result(dstX - 1, y) : i(width - 1, y);
            result(dstX, y) = left;
            dstX++;
            si++;
        }
    }
    if (showSeams)
    {
        seamAccum = i;
        for (int s = 0; s < k; s++)
            for (int y = 0; y < height; y++)
                seamAccum(seamList[s][y], y) = RGBA(0, 255, 0, 255); // green
    }
    i = result;
    width += k;
}

void removeSeams()
{
    energy.resize(width * height);
    M.resize(width * height);

    // DP
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

    // if (shrinkWidth)
    // {
    //
    if (shrinkWidth)
    {
        int newWidth = width - 1;
        ColorImage result(newWidth, height);

        std::vector<float> newGray(newWidth * height);
        std::vector<float> newEnergy(newWidth * height);

        for (int y = 0; y < height; y++)
        {
            int sx = seam[y];

            // copy pixels
            for (int x = 0; x < sx; x++)
                result(x, y) = i(x, y);
            for (int x = sx + 1; x < width; x++)
                result(x - 1, y) = i(x, y);

            // copy gray row, skipping seam column
            for (int x = 0; x < sx; x++)
                newGray[y * newWidth + x] = gray[y * width + x];
            for (int x = sx + 1; x < width; x++)
                newGray[y * newWidth + (x - 1)] = gray[y * width + x];

            // copy energy row, skipping seam column
            for (int x = 0; x < sx; x++)
                newEnergy[y * newWidth + x] = energy[y * width + x];
            for (int x = sx + 1; x < width; x++)
                newEnergy[y * newWidth + (x - 1)] = energy[y * width + x];
        }

        i = result;
        gray = std::move(newGray);
        energy = std::move(newEnergy);
        width = newWidth;

        // recompute only the 2 boundary columns
        for (int y = 0; y < height; y++)
        {
            int sx = seam[y];
            if (sx < width)
                energy[y * width + sx] = computeEnergy(sx, y, width);
            if (sx - 1 >= 0)
                energy[y * width + (sx - 1)] = computeEnergy(sx - 1, y, width);
        }
    }

    iteration++;
}


  