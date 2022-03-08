#include "quickcg.h"

int main(int argc, char *argv[])
{
  QuickCG::screen(800, 800, 0, "Y-Terminal");
  std::string someText;
  QuickCG::getInputString(someText, "Enter some text:", false, 0, 0);
  QuickCG::redraw();
  QuickCG::sleep();
}
