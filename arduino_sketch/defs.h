typedef struct Rgb Rgb;
struct Rgb {
  int r;
  int g;
  int b;
};

typedef struct ColorPair ColorPair;
struct ColorPair {
  Rgb start;
  Rgb end;
};