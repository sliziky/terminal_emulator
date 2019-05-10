#include "../include/Terminal.h"
#include "../include/colors.h"
namespace fs = std::experimental::filesystem;


fs::path Terminal::cd_util( const std::string& path_str ) {
	std::vector<std::string> path_split = split_path( path_str, "/" );
	auto tmp = _path;

	for ( const auto& path : path_split ) {
		// self directory
		if ( path == "." ) { continue; }

		// parent directory
		else if ( path == ".." ) {
			tmp = tmp.parent_path();
		}
		// given path not found
		else if ( !fs::exists( tmp / path ) ) {
			std::cout << "cd: " << path << " - no such file or directory\n";
			return tmp;
		}
		else { 
			tmp /= path;
		}
	}
	return tmp;
}

void Terminal::cd( const std::string& pat ) {
	_path = cd_util( pat );
}

void Terminal::clear() const {
	std::system( "cls" );
}

bool Terminal::check_arguments( const std::string& cmd, const std::string& flag, const std::string& path ) const {
	if ( cmd == "ls" ) {
		if ( starts_with( flag, "-" ) && starts_with( path, "-" ) ) {
			std::cout << "ls -> Wrong arguments - both are flags\n";
			return false;
		}

		if ( !starts_with( flag, "-" ) && starts_with( path, "-" ) ) {
			std::cout << "ls -> Wrong arguments - first must be FLAG second PATH\n";
			return false;
		}
		if ( !path.empty() && 
			 !starts_with( flag, "-" ) && !starts_with( path, "-" ) ) {
			std::cout << "ls -> Wrong arguments - both are PATHS\n";
			return false;
		}
		return true;
	}
	return true;
}

uintmax_t Terminal::directory_size( const fs::path& path ) const {
	uintmax_t total_size = 0;
	for ( auto& file : fs::recursive_directory_iterator( path ) ) {
		if ( fs::is_regular_file( file ) ) {
			total_size += fs::file_size( file );
		}
	}
	return total_size;
}

void Terminal::ls( const std::string& flag, const std::string& path ) {
	if ( !check_arguments( "ls", flag, path ) ) { return; }
	fs::path new_path = _path;
	
	// ls PATH
	if ( !flag.empty() && !starts_with( flag, "-" ) && path.empty() ) {
		new_path = flag;
	}

	if ( flag == "-all" ) {
		// ls -all path
		if ( !path.empty() ) {
			ls_all( cd_util( path ).string() );
			return;
		}
		// ls -all
		else {
			ls_all( new_path.string() );
			return;
		}
	}

	if ( fs::is_directory( new_path ) && fs::exists( new_path ) ) {
		WORD font_color;
		for ( const auto& file : fs::directory_iterator( new_path ) ) {
			if ( fs::is_directory( file.status() ) ) {
				font_color = (WORD) Colors::green;
			}
			else if ( fs::is_regular_file( file.status() ) ) {
				font_color = (WORD) Colors::red;
			}
			else {
				font_color = (WORD) Colors::yellow;
			}
			SetConsoleTextAttribute( _console, font_color );
			std::cout << file.path().filename().string() << '\n';
			SetConsoleTextAttribute( _console, (WORD) Colors::white  );
		}
	}
	else {
		std::cout << "ls -> '" << new_path << "' not found\n";
	}
}

void Terminal::ls_all( const std::string& path ) {
	fs::path new_path = path;
	if ( fs::is_directory( new_path ) && fs::exists( new_path ) ) {
		for ( const auto& file : fs::directory_iterator( new_path ) ) {
			// permissions
			print_permissions( file.status().permissions() );
			// last time
			print_time( file );
			// size of folder/file and name
			print_size_name( file );
		}
	}
}

void Terminal::mkdir( const std::string& path ) const {
	auto split = split_path( path, "/" );
	auto tmp_path = _path;
	for ( const auto& f : split ) {
		tmp_path /= f;
		if ( !fs::exists( tmp_path ) ) {
			fs::create_directory( tmp_path );
		}
		else {
			std::cout << "-mkdir -> " << f << " exists\n";
		}
	}
}

void Terminal::print_size_name( const fs::directory_entry& file ) {

	uintmax_t size = 0;
	std::string type = " B  ";
	WORD font_color = (WORD)Colors::green;

	if ( fs::is_directory( file.status() ) ) {
		size = directory_size( file );
	}
	else if ( fs::is_regular_file( file.status() ) ) {
		size = fs::file_size( file );
		font_color = (WORD)Colors::red;
	}

	if ( size >= 1024 ) {
		size /= 1024;
		type = " kB ";
	}
	std::cout << std::right << std::setw( 7 ) << size << std::left << std::setw( 2 ) << type;
	if ( WIN )
		SetConsoleTextAttribute( _console, font_color );

	std::cout << std::setw( 10 ) << file.path().filename() << '\n';
	
	if ( WIN )
		SetConsoleTextAttribute( _console, (WORD)Colors::white );
}

void Terminal::print_time( const fs::directory_entry& file ) const {
	auto ftime = fs::last_write_time( file );
	std::time_t cftime = fs::file_time_type::clock::to_time_t( ftime );
	std::cout << std::put_time( std::localtime( &cftime ), "%d-%m-%Y %H:%M:%S " );
}

void Terminal::print_permissions( fs::perms p ) const {
	std::cout << ( ( p & fs::perms::owner_read ) != fs::perms::none ? "r" : "-" )
		<< ( ( p & fs::perms::owner_write ) != fs::perms::none ? "w" : "-" )
		<< ( ( p & fs::perms::owner_exec ) != fs::perms::none ? "x" : "-" )
		<< ( ( p & fs::perms::group_read ) != fs::perms::none ? "r" : "-" )
		<< ( ( p & fs::perms::group_write ) != fs::perms::none ? "w" : "-" )
		<< ( ( p & fs::perms::group_exec ) != fs::perms::none ? "x" : "-" )
		<< ( ( p & fs::perms::others_read ) != fs::perms::none ? "r" : "-" )
		<< ( ( p & fs::perms::others_write ) != fs::perms::none ? "w" : "-" )
		<< ( ( p & fs::perms::others_exec ) != fs::perms::none ? "x " : "- " );
}

void Terminal::rmdir( const std::string & path ) const {
	if ( path == "." ) {
		std::cout << "Cant remove directory you are IN, move up and try again \n";
		return;
	}
	if ( !fs::exists( _path / path ) ) {
		std::cout << "Folder NOT found \n";
		return;
	}
	if ( !fs::is_empty( _path / path ) ) {
		std::cout << "Cant remove NON-EMPTY directory - try rm -r\n";
		return;
	}
	fs::remove( _path / path );
}

void Terminal::run() {
	for ( ;; ) {
		std::cout << _path.string() << " ~ ";
		Command command = _input_handler.get_command();
		auto cmd = callback_no_arg.find( command.first );
		if ( cmd != callback_no_arg.end() ) {
			cmd->second();
		}

		auto one_arg = callback_one_arg.find( command.first );
		if ( one_arg != callback_one_arg.end() ) {
			one_arg->second( command.second );
		}
		auto two_arg = callback_two_arg.find( command.first );
		if ( two_arg != callback_two_arg.end() ) {
			two_arg->second( command.second, command.path );
		}
	}
}