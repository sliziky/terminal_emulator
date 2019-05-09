
#include <string>  
#include <iostream>  
#include <sstream>  
#include <vector>
#include <unordered_map>
#include <experimental/filesystem>  
#include "Terminal.h"

int main() {
	Terminal t(std::experimental::filesystem::current_path());
	t.run();
	return 0;
}
