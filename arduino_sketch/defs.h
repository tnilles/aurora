typedef struct Point Point;
struct Point {
  int row;
  int col;
};

typedef struct Color Color;
struct Color {
  int hue; // Hue from 0 to 65535
  int sat; // Saturation from 0 to 255
  int val; // Brightness from 0 to 255
};