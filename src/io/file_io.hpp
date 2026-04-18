
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;


namespace ruya::io
{
    /*
	* Read the contents of the file at the given path.
	* @throws std::ifstream::failure if something went wrong while trying to read the file
	*/
	std::string file_contents(const fs::path& file_path)
	{
		// init file object
		std::ifstream file_stream;
		file_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit); // when to throw exceptions
		file_stream.open(file_path);

		// copy file contents to string
		std::string content(
			(std::istreambuf_iterator<char>(file_stream)),
			(std::istreambuf_iterator<char>())
		);

		file_stream.close();
		return content;
	}
}