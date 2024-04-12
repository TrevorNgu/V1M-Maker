# V1M-Maker
Using Quadrature Disambiguation Edge Detection to convert PGM/PPM files into V1M. Coded on a linux environment.

# Requirements
- Cmake
- OpenCV

# How to use
1. Use QDED to generate a text file from a PGM image file.
2. In the image-to-convert folder, put both text and PGM file in
3. Compile with make and execute ./V1M-Maker [file name]. Don't include the file extension.

Output will be a V1M file with the following header:
- V4 | V5 | V6
- Image width in pixel
- Image height in pixel
- Bits per filter value (For this code, it should be 8.)
- Vertical grid spacing in pixels
- Horizotnal grid spacing in pixels
- Horizotnal Position of center of first filter in pixel
- Vertical position of center of first filter in pixel

The rest of the file will contain the filter values as bytes. Each 5 filter after the header will be used to represent a tile, starting at (0, 0) in the top left.