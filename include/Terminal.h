#pragma once
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <experimental/filesystem>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

#ifdef _WIN32
	#include <Windows.h>
	constexpr auto WIN = 1;
#endif

#include "utils.h"

namespace fs = std::experimental::filesystem;


class Terminal {

private:
	/*
	Command represents command on the input
	"ls", "pwd", "ls -all", "ls dir/dir2" etc..
	First - first argument
	Second - second argument/path
	Path - path
	*/
	
	struct Command {
		Command() = default;
		void setArgs( const std::string& f, const std::string& s,
					  const std::string& p ) {
			first = f;
			second = s;
			path = p;
		}
		bool zero_args() const { return first.empty(); }
		bool one_arg() const { return second.empty(); }
		bool two_args() const { return path.empty(); }
		bool three_args() const { return !path.empty(); }
		int num_of_args() const {
			if ( one_arg() ) return 1;
			if ( two_args() ) return 2;
			if ( three_args() ) return 3;
			return 0;
		}
		std::string first;
		std::string second;
		std::string path;
		friend class Terminal;
	};
	/* 
	CommandLineArgumentHandler check for correctness of
	given input and stuff about
	*/
	struct InputHandler {

		void get_line() {
			line_split.clear();
			std::string line;
			std::getline( std::cin, line );
			std::istringstream iss( line );
			std::copy( std::istream_iterator< std::string >( iss ),
					   std::istream_iterator< std::string >(),
					   std::back_inserter( line_split ) );
		}

		Command get_command() {
			get_line();
			Command command;
			switch ( line_split.size() ) {
				case 0:
					command.setArgs( "", "", {} );
					break;
				case 1:
					command.setArgs( line_split[ 0 ], "", {} );
					break;
				case 2:
					command.setArgs( line_split[ 0 ], line_split[ 1 ], {} );
					break;
				case 3:
					command.setArgs( line_split[ 0 ], line_split[ 1 ], line_split[ 2 ]);
					break;
				default:
					command.setArgs( "", "", {} );
					break;
			}
			return command;
		}

		std::vector<std::string> line_split;
		friend class Terminal;
	};
public:
	explicit Terminal( const fs::path& path )
		: _path( path ) {
		if ( WIN )
			_console = GetStdHandle( STD_OUTPUT_HANDLE );
	}
	~Terminal() = default;


	fs::path cd_util( const std::string& pat );
	void cd( const std::string& pat );
	void clear() const;
	bool check_arguments( const std::string& cmd, const std::string& flag, const std::string& path ) const;
	uintmax_t directory_size( const fs::path& path ) const;
	void ls( const std::string& flag, const std::string& path );
	void ls_all( const std::string& path );
	void mkdir(  const std::string& path ) const;
	void print_size_name( const fs::directory_entry& file );
	void print_time( const fs::directory_entry& file ) const;
	void print_permissions( fs::perms p ) const;
	void pwd() { std::cout << _path.string() << "\n"; }
	void rmdir( const std::string& path ) const;
	void run();


	const fs::path& path() const { return _path; }
	fs::path& path() { return _path; }

private:
	std::unordered_map< std::string, std::function<void( void )> > callback_no_arg {
		{"pwd",[this]() { return pwd(); }},
		{"clear",[this]() { return clear(); }}
	};
	std::unordered_map< std::string, std::function<void( std::string )> > callback_one_arg {
		{"mkdir",[this]( const std::string& path) { return mkdir( path ); }},
		{"rmdir",[this]( const std::string& path ) { return rmdir( path ); }},
		//{"ls",[this](const std::string& path) { return ls( path ); }},
		{"cd",[this]( const std::string& path) { return cd( path ); }}
	};
	std::unordered_map< std::string, std::function<void( std::string, std::string )> > callback_two_arg {
		{"ls",[this]( const std::string& flag, const std::string& path ) { return ls( flag, path ); }}
	};
	InputHandler		_input_handler;
	fs::path			_path;
	HANDLE				_console;
};

