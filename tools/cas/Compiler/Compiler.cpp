#include <Compiler/Compiler.h>

Compiler::Compiler() {
	
}

void Compiler::first_cycle(const std::vector<Lexer::Token>& tokens)
{
	u32 addr = 0x200;
	for (const auto& token : tokens) {
		if (std::holds_alternative<Lexer::Keyword>(token)) {
			addr += 2;
		}
		if (auto mark = std::get_if<Lexer::Mark>(&token)) {
			if (marks.contains(mark->idf)) {
				continue;
			}
			marks[mark->idf] = addr;
		}
	}
}

std::vector<u8> Compiler::compile(const std::vector<Lexer::Token>& tokens)
{
	std::vector<u8> result;

	for (const auto& token : tokens) {

	}

	return result;
}
