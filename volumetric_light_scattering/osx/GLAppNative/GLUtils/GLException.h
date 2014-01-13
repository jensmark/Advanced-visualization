#ifndef _GLEXCEPTION_H__
#define _GLEXCEPTION_H__

#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>

class GameException : public std::runtime_error {
public:
	GameException(const char* file, unsigned int line, const char* msg) : std::runtime_error(msg) {
		std::cerr << file << ":" << line << ": " << msg << std::endl;
	}

	GameException(const char* file, unsigned int line, const std::string msg) : std::runtime_error(msg) {
		std::cerr << file << ":" << line << ": " << msg << std::endl;
	}
};


#define THROW_EXCEPTION(msg) throw GameException(__FILE__, __LINE__, msg)

#endif