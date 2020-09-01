#include <fstream>
#include <iostream>

int main(int argc, char const *argv[])
{
  if (argc == 1)
  {
  }
  else
  {
    if (strcmp(argv[1], "init") == 0)
    {
      std::ofstream config("config.lua");
      config << "settings = {\n";
      config << "    tick_period = 100\n";
      config << "}\n";
      config << "\n";
      config << "shape = {\n";
      config << "    path = \"f.lua\",\n";
      config << "    vname = \"shape\"\n";
      config << "}\n";
      config << "\n";
      config << "updater = {\n";
      config << "    path = \"f.lua\",\n";
      config << "    vname = \"upd\"\n";
      config << "}\n";
      config.close();

      std::ofstream f("f.lua");
      f << "shape = {}\n";
      f << "\n";
      f << "upd = function(obj)\n";
      f << "    print(\"HW\")\n";
      f << "end\n";
      f.close();
    }
  }
  return 0;
}
