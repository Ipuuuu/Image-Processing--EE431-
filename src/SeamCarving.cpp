#include "Image.h"
#include <vector>
#include <float.h>

int width, height;
int targetWidth;
int newTargetWidth, newTargetHeight;
bool shrinkWidth = false;

ColorImage i;
int iteration = 0;

std::vector<float> energy, M, gray;
std::vector<int> seam;
std::vector<std::vector<int>> seamList;

float computeEnergy(int x, int y, int w);
ColorImage transpose(const ColorImage &src);
void removeSeams();
void insertSeams();

int main()
{
    i.Load("input.png");

    width = i.GetWidth();
    height = i.GetHeight();
    seam.resize(height);

    std::cout << "Image size: " << width << " x " << height << std::endl;

    std::cout << "Enter target width: ";
    std::cin >> newTargetWidth;
    std::cout << "Enter target height: ";
    std::cin >> newTargetHeight;

    shrinkWidth = newTargetWidth < width;
    targetWidth = newTargetWidth;

    if (!shrinkWidth && width != targetWidth)
    {
        while (width < targetWidth)
            insertSeams();

        if (height != newTargetHeight)
        {
            i = transpose(i);
            std::swap(width, height);
            seam.resize(height);

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
            std::swap(width, height);
            seam.resize(height);
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
    }

    if (height != newTargetHeight)
    {
        i = transpose(i);
        std::swap(width, height);
        seam.resize(height);

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
        std::swap(width, height);
        seam.resize(height);
    }

    std::cout << "Saving output, width=" << width << " height=" << height << std::endl;
    i.Save("output.png");
    return 0;
}

float computeEnergy(int x, int y, int w)
{
    auto clamp = [](int v, int lo, int hi)
    { return std::max(lo, std::min(hi, v)); };
    float Gx = gray[clamp(y - 1, 0, height - 1) * w + clamp(x - 1, 0, w - 1)] * 1 + gray[clamp(y - 1, 0, height - 1) * w + clamp(x + 1, 0, w - 1)] * -1 + gray[clamp(y, 0, height - 1) * w + clamp(x - 1, 0, w - 1)] * 2 + gray[clamp(y, 0, height - 1) * w + clamp(x + 1, 0, w - 1)] * -2 + gray[clamp(y + 1, 0, height - 1) * w + clamp(x - 1, 0, w - 1)] * 1 + gray[clamp(y + 1, 0, height - 1) * w + clamp(x + 1, 0, w - 1)] * -1;
    float Gy = gray[clamp(y - 1, 0, height - 1) * w + clamp(x - 1, 0, w - 1)] * 1 + gray[clamp(y - 1, 0, height - 1) * w + clamp(x, 0, w - 1)] * 2 + gray[clamp(y - 1, 0, height - 1) * w + clamp(x + 1, 0, w - 1)] * 1 + gray[clamp(y + 1, 0, height - 1) * w + clamp(x - 1, 0, w - 1)] * -1 + gray[clamp(y + 1, 0, height - 1) * w + clamp(x, 0, w - 1)] * -2 + gray[clamp(y + 1, 0, height - 1) * w + clamp(x + 1, 0, w - 1)] * -1;
    return std::abs(Gx) + std::abs(Gy);
}

ColorImage transpose(const ColorImage &src)
{
    int w = src.GetWidth(), h = src.GetHeight();
    ColorImage dst(h, w);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            dst(y, x) = src(x, y);
    return dst;
}

void insertSeams()
{
    int k = std::min(targetWidth - width, width / 2);
    seamList.clear();

    GrayscaleImage g(i);
    energy.resize(width * height);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            float Gx = (1) * g.Get(car(x - 1, width - 1), car(y - 1, height - 1)) + (-1) * g.Get(car(x + 1, width - 1), car(y - 1, height - 1)) + (2) * g.Get(car(x - 1, width - 1), car(y, height - 1)) + (-2) * g.Get(car(x + 1, width - 1), car(y, height - 1)) + (1) * g.Get(car(x - 1, width - 1), car(y + 1, height - 1)) + (-1) * g.Get(car(x + 1, width - 1), car(y + 1, height - 1));
            float Gy = (1) * g.Get(car(x - 1, width - 1), car(y - 1, height - 1)) + (2) * g.Get(car(x, width - 1), car(y - 1, height - 1)) + (1) * g.Get(car(x + 1, width - 1), car(y - 1, height - 1)) + (-1) * g.Get(car(x - 1, width - 1), car(y + 1, height - 1)) + (-2) * g.Get(car(x, width - 1), car(y + 1, height - 1)) + (-1) * g.Get(car(x + 1, width - 1), car(y + 1, height - 1));
            energy[y * width + x] = abs(Gx) + abs(Gy);
        }

    for (int y = 0; y < height; y++)
    {
        energy[y * width + 0] += 1e6f;
        energy[y * width + width - 1] += 1e6f;
    }

    std::vector<float> tempEnergy = energy;
    std::vector<float> tempM(width * height);

    for (int s = 0; s < k; s++)
    {
        for (int x = 0; x < width; x++)
            tempM[x] = tempEnergy[x];
        for (int y = 1; y < height; y++)
            for (int x = 0; x < width; x++)
            {
                float left = (x > 0) ? tempM[(y - 1) * width + (x - 1)] : FLT_MAX;
                float up = tempM[(y - 1) * width + x];
                float right = (x < width - 1) ? tempM[(y - 1) * width + (x + 1)] : FLT_MAX;
                float minP = std::min({left, up, right});
                tempM[y * width + x] = (minP >= FLT_MAX / 2) ? FLT_MAX : tempEnergy[y * width + x] + minP;
            }

        int minCol = 0;
        float minVal = tempM[(height - 1) * width];
        for (int x = 1; x < width; x++)
            if (tempM[(height - 1) * width + x] < minVal)
            {
                minVal = tempM[(height - 1) * width + x];
                minCol = x;
            }

        std::vector<int> currentSeam(height);
        currentSeam[height - 1] = minCol;
        for (int y = height - 2; y >= 0; y--)
        {
            int p = currentSeam[y + 1], best = p;
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

        bool collision = false;
        for (int y = 0; y < height; y++)
            if (tempEnergy[y * width + currentSeam[y]] >= FLT_MAX / 2)
            {
                collision = true;
                break;
            }
        if (collision)
        {
            k = s;
            break;
        }

        seamList.push_back(currentSeam);
        for (int y = 0; y < height; y++)
            tempEnergy[y * width + currentSeam[y]] = FLT_MAX;
    }

    k = seamList.size();
    ColorImage result(width + k, height);

    for (int y = 0; y < height; y++)
    {
        std::vector<std::pair<int, int>> seamsAtY(k);
        for (int s = 0; s < k; s++)
            seamsAtY[s] = {seamList[s][y], s};
        std::sort(seamsAtY.begin(), seamsAtY.end());

        int srcX = 0, dstX = 0, si = 0;
        while (srcX < width)
        {
            while (si < k && seamsAtY[si].first == srcX)
            {
                RGBA left = (dstX > 0) ? result(dstX - 1, y) : i(srcX, y);
                RGBA right = i(srcX, y);
                result(dstX, y) = RGBA((left.r + right.r) / 2, (left.g + right.g) / 2, (left.b + right.b) / 2, 255);
                dstX++;
                si++;
            }
            result(dstX, y) = i(srcX, y);
            dstX++;
            srcX++;
        }
        while (si < k)
        {
            RGBA left = (dstX > 0) ? result(dstX - 1, y) : i(width - 1, y);
            result(dstX, y) = left;
            dstX++;
            si++;
        }
    }
    i = result;
    width += k;
}

void removeSeams()
{
    energy.resize(width * height);
    M.resize(width * height);

    for (int x = 0; x < width; x++)
        M[x] = energy[x];

    for (int y = 1; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            float left = (x > 0) ? M[(y - 1) * width + (x - 1)] : FLT_MAX;
            float up = M[(y - 1) * width + x];
            float right = (x < width - 1) ? M[(y - 1) * width + (x + 1)] : FLT_MAX;
            M[y * width + x] = energy[y * width + x] + std::min({left, up, right});
        }

    int minCol = 0;
    float minVal = M[(height - 1) * width];
    for (int x = 1; x < width; x++)
        if (M[(height - 1) * width + x] < minVal)
        {
            minVal = M[(height - 1) * width + x];
            minCol = x;
        }

    seam[height - 1] = minCol;
    for (int y = height - 2; y >= 0; y--)
    {
        int p = seam[y + 1], best = p;
        float bv = M[y * width + p];
        if (p > 0 && M[y * width + (p - 1)] < bv)
        {
            bv = M[y * width + (p - 1)];
            best = p - 1;
        }
        if (p < width - 1 && M[y * width + (p + 1)] < bv)
        {
            bv = M[y * width + (p + 1)];
            best = p + 1;
        }
        seam[y] = best;
    }

    if (shrinkWidth)
    {
        int newWidth = width - 1;
        ColorImage result(newWidth, height);
        std::vector<float> newGray(newWidth * height);
        std::vector<float> newEnergy(newWidth * height);

        for (int y = 0; y < height; y++)
        {
            int sx = seam[y];
            for (int x = 0; x < sx; x++)
                result(x, y) = i(x, y);
            for (int x = sx + 1; x < width; x++)
                result(x - 1, y) = i(x, y);
            for (int x = 0; x < sx; x++)
                newGray[y * newWidth + x] = gray[y * width + x];
            for (int x = sx + 1; x < width; x++)
                newGray[y * newWidth + (x - 1)] = gray[y * width + x];
            for (int x = 0; x < sx; x++)
                newEnergy[y * newWidth + x] = energy[y * width + x];
            for (int x = sx + 1; x < width; x++)
                newEnergy[y * newWidth + (x - 1)] = energy[y * width + x];
        }

        i = result;
        gray = std::move(newGray);
        energy = std::move(newEnergy);
        width = newWidth;

        for (int y = 0; y < height; y++)
        {
            int sx = seam[y];
            if (sx < width)
                energy[y * width + sx] = computeEnergy(sx, y, width);
            if (sx - 1 >= 0)
                energy[y * width + sx - 1] = computeEnergy(sx - 1, y, width);
        }
    }

    iteration++;
}