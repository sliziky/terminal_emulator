#include <string>  
#include <iostream>  
#include <sstream>  
#include <vector>
#include <unordered_map>
#include <experimental/filesystem>  
#include "Terminal.h"


std::vector<std::string> split( const std::string& s, char delimiter ) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream( s );
	while ( std::getline( tokenStream, token, delimiter ) ) {
		tokens.push_back( token );
	}
	return tokens;
}

int main() {
	Terminal t(std::experimental::filesystem::current_path());
	t.run();

	return 0;
}