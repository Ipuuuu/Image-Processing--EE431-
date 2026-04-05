# Digital Image Processing - EE431

A collection of fundamental image processing algorithms implemented in C++. This repository contains solutions to coursework from the Principles of Digital Image Processing course.

## Implemented Algorithms

- **Brightness Adjustment** - Modify image brightness
- **Contrast Enhancement** - Adjust image contrast
- **Contrast Stretching** - Stretch pixel value range for better contrast
- **Gamma Correction** - Non-linear brightness adjustment using gamma values
- **Histogram Equalization** - Enhance image contrast through histogram redistribution
- **Image Inversion** - Invert pixel values
- **Nearest Neighbor Interpolation (NNI)** - Image scaling using nearest neighbor method
- **Bilinear Interpolation (BLI)** - Image scaling using bilinear interpolation

## Project Structure

```
src/          - Source code for image processing algorithms
images/       - Test images
build/        - Compiled executables
Image.h       - Core image processing library (supports grayscale and color images)
Makefile      - Build configuration
```

## Build & Run

```bash
make          # Compile all programs
make clean    # Remove build files
./build/[algorithm_name]  # Run specific algorithm
```

## Requirements

- C++ compiler (g++)
- PNG library support

## Note

Each algorithm reads from `images/lena.png` and outputs a processed image. Modify the source code to process different images or adjust algorithm parameters.
