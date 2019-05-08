#pragma once

#include <experimental/filesystem>
#include <iomanip>
#include <set>
#include <string>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <string>
#include <iterator>
namespace fs = std::experimental::filesystem;

class Terminal
{
private:
	struct CommandLineArgument {
		
		void get_line() {
			line_split.clear();
			std::getline(std::cin, line);
			std::istringstream iss(line);
			std::copy( std::istream_iterator< std::string >( iss ),
					   std::istream_iterator< std::string >(),
					   std::back_inserter( line_split ) );
		}
		
		std::string correct_arguments() const {
			// max 2 arguments
			if ( line_split.size() >= 3 ) return "";
			if ( line_split.size() == 1 ) {
				if ( line_split[ 0 ] == "ls" ) {
					return "ls";
				}
				if ( line_split[ 0 ] == "pwd" ) {
					return "pwd";
				}
				if ( line_split[ 0 ] == "cd" ) {
					return "";
				}
			}
			if ( line_split.size() == 2 ) {
				if ( line_split[ 0 ] == "cd" ) {
					return "cd"; 
				}
			}
			return "";
		}

		template < typename T >
		bool contains( const std::vector< T >& vec, const T& item ) const {
			return std::find( vec.begin(), vec.end(), item ) != vec.end();
		}

		std::vector<std::string> line_split;
		const std::unordered_map<std::string, std::vector<std::string>> arguments { {"ls",{"all"}} };
		std::string line;
		friend class Terminal;
	};
public:
	Terminal(const fs::path& path)
		: _path(path)
	{}
	~Terminal() = default;


	void run() {
		for (;;) {
			std::cout << _path.string() << " ~ ";
			_cla.get_line();
			auto c = _cla.correct_arguments();
			if ( c == std::string("ls") ) { ls(); }
			if ( c == std::string("ls all") ) { ls_sub(); }
			if ( c == std::string("pwd") ) { pwd(); }
			if ( c == std::string("cd") ) { cd( _cla.line_split[ 1 ] ); }
		}
	}

	void pwd() const {
		std::cout << _path.string() << "\n";
	}
	void cd( std::string& folder ) {
		if ( folder == ".." ) {
			_path = _path.parent_path();
			return;
		}
		if ( folder == "." ) {
			return;
		}
		fs::path tmp = _path;
		tmp /= folder;
		if ( fs::exists(tmp) ) { 
			_path /= folder; 
		}
	}

	void ls() {
		std::vector<std::tuple<std::string, std::string, int>> files;
		if (fs::is_directory(_path)) {
			for (const auto& file : fs::directory_iterator(_path)) {
				if (fs::is_directory(file.status())) {
					files.emplace_back( std::string("DIR"), file.path().filename().string(), 0 );
				}
				else if (fs::is_regular_file(file.status()))  {
					files.emplace_back( std::string("FILE"), file.path().filename().string(), fs::file_size(file) );
				} 
			}
			auto max = 0;
			for ( int i = 0; i < files.size(); ++i ){
				if (std::get<1>(files[i]).size() > max) {
					max = std::get<1>( files[ i ] ).size();
				}
			}
			max += 1;
			for ( int i = 0; i < files.size(); ++i ) {
				if (std::get<0>(files[i]) == "DIR") {
					std::cout << std::left << std::setw(5) << "DIR " << std::left << std::setw(max) << std::get<1>(files[i]) << std::get<2>(files[i]) << " B\n";
				}
				else {
					std::cout << "FILE " << std::left << std::setw( max ) << std::get<1>( files[ i ] ) << std::get<2>( files[ i ] ) << " B\n";
				}
			}
		}
	}

	void ls_sub() {
		ls_sub_util(_path, 0);
	}

	void ls_sub_util( const fs::path& path, int level ) {
		if (fs::is_directory(path) && fs::exists(path) ) {
			std::string f = std::string( level , ' ');
			for (const auto& file : fs::directory_iterator(path)) {
				if ( fs::is_directory( file.status() ) ) {
					std::cout << f << "DIR" << "[+]" << file.path().filename() << '\n';
					ls_sub_util(file, level + 3);
				}
				else if ( fs::is_regular_file( file.status() ) ) {
					std::cout << f << "FILE " << file.path().filename() << std::setw(50 - level) << fs::file_size( file.path() )/1024 << " kB\n";
				}
			}
		}
	}

	const fs::path& path() const { return _path; }
	fs::path& path() { return _path; }

private:
	CommandLineArgument _cla;
	fs::path _path;
};

