#include <string>
using namespace std;

static const string to_convert = "image-to-convert/";
static const string converted = "converted/";

// Spacing of the tiles in the V1M file. Have to do it manually unfortunately....
static const int col_space = 11;
static const int row_space = 26;

// Diameter of the circle. Used for the header file
static const int diameter = 6;

// Magic number of the header. Used for the header file
static string magic = "V4";

// Divide parameter for making the convolve tile * filter value smaller
static const int divide_val = 30000;
