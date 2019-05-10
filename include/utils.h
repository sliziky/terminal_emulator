#pragma once
#include <vector>
#include <string>

inline std::vector<std::string> split_path( std::string s, const std::string& delimiter ) {
	std::vector<std::string> splitted;
	size_t pos = 0;
	std::string token;
	s += "/";
	while ( (pos = s.find( delimiter )) != std::string::npos ) {
		token = s.substr( 0, pos );
		splitted.emplace_back( token );
		s.erase( 0, pos + delimiter.length() );
	}
	return splitted;
}

inline bool starts_with( const std::string& s, const std::string& prefix ) {
	return  s.rfind( prefix, 0 ) == 0;
}


constexpr uintmax_t operator"" _kB( uintmax_t kB ) {
	return kB * 1024;
}