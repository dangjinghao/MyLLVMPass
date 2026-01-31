// Simple test for local CSE pass
// Contains repeated computations that should be merged by the pass.

int compute(int x, int y) {
  int a = x + y;
  int b = x + y; // redundant: same as a
  int c = a * 2;
  int d = b * 2; // redundant: same as c
  return c + d;
}

int reassign(int x, int y) {
  int a = x + y;
  x = x + 1;     // modifies x, but does not affect previous computation
  int b = x + y; // not redundant: x has changed
  return a + b;
}

int main(void) {
  int r = compute(10, 20);
  r += reassign(10, 20);
  return r;
}
