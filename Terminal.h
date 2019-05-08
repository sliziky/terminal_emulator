#pragma once
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
#pragma once

#include <algorithm>
#include <experimental/filesystem>
#include <iomanip>
#include <iostream>
#include <functional>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include "utils.h"
#ifdef _WIN32
#include <Windows.h>
#include <map>
constexpr auto WIN = 1;
#endif

namespace fs = std::experimental::filesystem;


class Terminal {
	using functionPtr = void( *)();
private:
	struct Command {
		Command() = default;
		Command( const std::string& f, const std::string& s,
				 const std::vector<std::string>& p )
			: first( f )
			, second( s )
			, path( p ) {}
		void setArgs( const std::string& f, const std::string& s,
					  const std::vector<std::string>& p ) {
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
		std::vector<std::string> path;
		friend class Terminal;
	};
	struct CommandLineArgumentHandler {
		using Arguments = std::unordered_map < std::string, std::vector<std::string >>;

		void get_line() {
			line_split.clear();
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
					command.setArgs( line_split[ 0 ], line_split[ 1 ], split_path( line_split[ 2 ], "/" ) );
					break;
				default:
					command.setArgs( "", "", {} );
					break;
			}
			return command;
		}

		bool correct_first_argument() const {
			return arguments.find( line_split[ 0 ] ) != arguments.end();
		}

		bool no_additional_args( const std::string& arg ) const {
			auto it = arguments.find( arg );
			return std::find( it->second.begin(), it->second.end(), "" ) != it->second.end();
		}

		bool correct_second_argument() const {
			auto f = arguments.find( line_split[ 0 ] );
			if ( f == arguments.end() ) return false;
			return std::find( f->second.begin(), f->second.end(), line_split[ 1 ] ) != f->second.end();

		}

		bool both_arguments_correct() const { return correct_first_argument() && correct_second_argument(); }

		std::string correct_arguments() const {

			switch ( line_split.size() ) {
				case 1:
					if ( correct_first_argument() ) {
						return line_split[ 0 ];
					}
				case 2:
					if ( line_split[ 0 ] == "mkdir" || line_split[ 0 ] == "cd"
						 || line_split[ 0 ] == "rmdir"
						 || line_split[ 0 ] == "ls" ) {
						return line_split[ 0 ];
					}
					if ( both_arguments_correct() ) {
						return line_split[ 0 ] + " " + line_split[ 1 ];
					}
				case 3:
					if ( line_split[ 0 ] == "rmdir" && line_split[ 1 ] == "-r" ) {
						return line_split[ 0 ] + " " + line_split[ 1 ];
					}
				default:
					return "";

			}
		}

		template < typename T >
		bool contains( const std::vector< T >& vec, const T& item ) const {
			return std::find( vec.begin(), vec.end(), item ) != vec.end();
		}

		std::vector<std::string> line_split;
		const Arguments arguments { {"ls",{"","-all"}},
									{"pwd",{""}},
									{"clear",{""}},
									{"cd",{""}},
									{"mkdir",{"-r"}},
									{"rmdir",{"","-r"}} };
		std::string line;
		Command command;
		friend class Terminal;
	};
public:
	explicit Terminal( const fs::path& path )
		: _path( path ) {
		if ( WIN )
			_console = GetStdHandle( STD_OUTPUT_HANDLE );
	}
	~Terminal() = default;


	fs::path cd_util( const std::string& pat ) {
		std::vector<std::string> path = split_path( pat, "/" );
		auto tmp = _path;
		for ( const auto& p : path ) {
			if ( p == "." ) { continue; } 
			else if ( p == ".." ) {
				tmp = tmp.parent_path();
			} 
			else {
				if ( !fs::exists( tmp/p ) ) {
					std::cout << "cd: " << p << " - no such file or directory\n";
					return tmp;
				}
				tmp /= p;
			}
		}
		return tmp;
	}

	void cd( const std::string& pat ) {
		_path = cd_util( pat );
	}

	void clear() { std::system( "cls" ); }
	void ls( const std::string& flag );
	void ls_all() {
		if ( fs::is_directory( _path ) && fs::exists( _path ) ) {
			for ( const auto& file : fs::directory_iterator( _path ) ) {
				// permissions
				print_permissions( file.status().permissions() );
				// last time
				print_time( file );
				// size of folder/file and name
				print_size_name( file );
			}
		}
	}
	void print_size_name( const fs::directory_entry& file );

	void mkdir(  const std::string& path ) {
		auto split = split_path( path, "/" );
		auto tmp_path = _path;
		for ( const auto& f : split ) {
			tmp_path /= f;
			if ( !fs::exists( tmp_path ) ) {
				fs::create_directory( tmp_path );
			}
		}
	}
	void pwd() { std::cout << _path.string() << "\n"; }
	void rmdir( const std::string& path ) const;
	void run() {
		for ( ;; ) {
			std::cout << _path.string() << " ~ ";
			Command command = _cla.get_command();
			auto without_arg = callbackMap.find( command.first );
			if ( without_arg != callbackMap.end() ) {
				without_arg->second();
			}

			auto one_arg = callbackMap3.find( command.first );
			if ( one_arg != callbackMap3.end() ) {
				one_arg->second(command.second);
			}
		}
	}


	uintmax_t directory_size( const fs::path& path ) const;
	void print_time( const fs::directory_entry& file ) const;
	void print_permissions( fs::perms p ) const {
		std::cout << ((p & fs::perms::owner_read) != fs::perms::none ? "r" : "-")
			<< ((p & fs::perms::owner_write) != fs::perms::none ? "w" : "-")
			<< ((p & fs::perms::owner_exec) != fs::perms::none ? "x" : "-")
			<< ((p & fs::perms::group_read) != fs::perms::none ? "r" : "-")
			<< ((p & fs::perms::group_write) != fs::perms::none ? "w" : "-")
			<< ((p & fs::perms::group_exec) != fs::perms::none ? "x" : "-")
			<< ((p & fs::perms::others_read) != fs::perms::none ? "r" : "-")
			<< ((p & fs::perms::others_write) != fs::perms::none ? "w" : "-")
			<< ((p & fs::perms::others_exec) != fs::perms::none ? "x " : "- ");
	}
	const fs::path& path() const { return _path; }
	fs::path& path() { return _path; }

private:
	std::unordered_map< std::string, std::function<void( void )> > callbackMap {
		{"pwd",[this]() { return pwd(); }},
		{"clear",[this]() { return clear(); }}
	};
	std::unordered_map< std::string, std::function<void( std::string )> > callbackMap3 {
		{"mkdir",[this]( const std::string& path) { return mkdir(path); }},
		{"rmdir",[this]( const std::string& path ) { return rmdir( path ); }},
		{"ls",[this](const std::string& flag) { return ls(flag); }},
		{"cd",[this]( const std::string& path) { return cd( path ); }}
	};
	CommandLineArgumentHandler _cla;
	fs::path			_path;
	HANDLE				_console;
};

