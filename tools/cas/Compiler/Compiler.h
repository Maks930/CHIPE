#ifndef COMPILER_H
#define COMPILER_H


#include <vector>
#include <unordered_map>

#include <Lexer.h>
#include <Types.h>


class Compiler {
public:
	explicit Compiler();
	
	void first_cycle(const std::vector<Lexer::Token>& tokens);
	
	std::vector<u8> compile(const std::vector<Lexer::Token>& tokens);

	std::unordered_map<std::string, u16> getMarks() {
		return marks;
	}

private:
	std::unordered_map<std::string, u16> marks;
};

#endif // !COMPILER_H
