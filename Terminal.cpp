#include "Terminal.h"


namespace fs = std::experimental::filesystem;





void Terminal::ls( const std::string& flag ) {
	auto split = split_path( flag, "/" );
	fs::path x = cd_util( flag );

	if ( fs::is_directory( x ) && fs::exists( x ) ) {
		for ( const auto& file : fs::directory_iterator( x ) ) {
			if ( fs::is_directory( file.status() ) ) {
				SetConsoleTextAttribute( _console, 10 );
			} else if ( fs::is_regular_file( file.status() ) ) {
				SetConsoleTextAttribute( _console, 12 );
			}
			std::cout << file.path().filename().string() << '\n';
			SetConsoleTextAttribute( _console, 7 );
		}
	}

	//if ( flag == "-all" ) {
	//	ls_all();
	//}
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

void Terminal::print_time( const fs::directory_entry& file ) const {
	auto ftime = fs::last_write_time( file );
	std::time_t cftime = fs::file_time_type::clock::to_time_t( ftime );
	std::cout << std::put_time( std::localtime( &cftime ), "%d-%m-%Y %H:%M:%S " );
}




void Terminal::rmdir( const std::string& path ) const {
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

void Terminal::print_size_name( const fs::directory_entry& file ) {

	uintmax_t size = 0;
	std::string type = " B  ";
	int font_color = 10;

	if ( fs::is_directory( file.status() ) ) {
		size = directory_size( file );
	} else if ( fs::is_regular_file( file.status() ) ) {
		size = fs::file_size( file );
		font_color = 12;
	}

	if ( size >= 1024 ) {
		size /= 1024;
		type = " kB ";
	}
	if ( WIN )
		SetConsoleTextAttribute( _console, font_color );
	std::cout << std::right << std::setw( 7 ) << size << std::left << std::setw( 2 ) << type
		<< std::setw( 10 ) << file.path().filename() << '\n';
	if ( WIN )
		SetConsoleTextAttribute( _console, 7 );
}
