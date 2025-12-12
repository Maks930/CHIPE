#include <Lexer/Lexer.h>
#include <fmt/format.h>
#include <stdexcept>
#include <cctype>
#include <ranges>
#include <charconv>


const std::unordered_map<std::string_view, Lexer::Keyword> Lexer::keywords = {
		{"db",		Keyword::DB},
		{"dw",		Keyword::DW},

		{"cls",		Keyword::CLS},
		{"ret",		Keyword::RET},
		{"sys",		Keyword::SYS},
		{"jp",		Keyword::JP},
		{"call",	Keyword::CALL},
		{"se",		Keyword::SE},
		{"sne",		Keyword::SNE},
		{"ld",		Keyword::LD},
		{"mov",		Keyword::LD},
		{"and",		Keyword::AND},
		{"xor",		Keyword::XOR},
		{"OR",		Keyword::OR},
		{"add",		Keyword::ADD},
		{"sub",		Keyword::SUB},
		{"subn",	Keyword::SUBN},
		{"shr",		Keyword::SHR},
		{"shl",		Keyword::SHL},
		{"rnd",		Keyword::RND},
		{"drw",		Keyword::DRW},
		{"hlt",		Keyword::HLT},
		{"nop",		Keyword::HLT},

		{"V1",		Register::V1},
		{"V2",		Register::V2},
		{"V3",		Register::V3},
		{"V4",		Register::V4},
		{"V5",		Register::V5},
		{"V6",		Register::V6},
		{"V7",		Register::V7},
		{"V8",		Register::V8},
		{"V9",		Register::V9},

		{"VA",		Register::VA},
		{"V10",		Register::VA},
		{"VB",		Register::VB},
		{"V11",		Register::VB},
		{"VC",		Register::VC},
		{"V12",		Register::VC},
		{"VD",		Register::VD},
		{"V13",		Register::VD},
		{"VE",		Register::VE},
		{"V14",		Register::VE},
		{"VF",		Register::VF},
		{"V15",		Register::VF},
};

std::vector<Lexer::Token> Lexer::processed(std::string_view& code)
{
	std::vector<Lexer::Token> result;


	


	u32 line = 0;
	u32 col = 0;
	for (auto it = code.begin(); it != code.end(); it++) {

		auto remainingString = std::ranges::subrange(it, code.end());

		auto findBlank = [&] {
			return std::ranges::find_if(remainingString, [](char c) -> bool {
				switch (c) {
				case ' ':
				case '\t':
				case '\n':
				case ':':
				case ',':
				//case '-':
				//case '+':
					return true;

				default:
					return false;
				}
				});
			};

		switch (*it) {
		case ' ':
		case ':':
		case '\t':
		case '\r':
		case ',':
			break;

		case '[':
			result.push_back(LSQPar{});
			break;
		case ']':
			result.push_back(RSQPar{});
			break;
		case '+':
			result.push_back(Plus{});
			break;
		case '-':
			result.push_back(Sub{});
			break;


		case '\n':
			line++;
			col = 0;
			break;



		default:
			if (std::isalpha(*it) || *it == '_') {
				auto blank = findBlank();
				std::string_view valueString(it, blank);

				if (auto keyword = keywords.find(valueString); keyword != keywords.end()) {
					result.push_back(keyword->second);
					it = std::prev(blank);
					break;
				}

				if (*blank == ':') {
					result.push_back(Mark{ .idf = std::string(valueString) });
					it = std::prev(blank);
					break;
				}
				result.push_back(Identifier{ .idf = std::string(valueString) });
				it = std::prev(blank);
				break;
			}

			if (*it == '0' && *(it + 1) == 'x') {
				auto end = std::ranges::find_if(std::ranges::subrange(it + 2, code.end()), [](char c) -> bool {
					return !std::isxdigit(c);
					});
				std::string_view digitString(it + 2, end);
				u32 integer = 0;
				std::from_chars(digitString.data(), digitString.data() + digitString.size(), integer, 16);
				result.push_back(Integer{ .value = integer });
				it = std::prev(end);
				break;
			}
			else if (std::isdigit(*it)) {
				auto end = std::ranges::find_if(remainingString, [](char c) -> bool {
					return !std::isdigit(c);
					});
				std::string_view digitString(it, end);

				u32 integer = 0;
				std::from_chars(digitString.data(), digitString.data() + digitString.size(), integer, 10);
				result.push_back(Integer{ .value = integer });
				it = std::prev(end);
				break;
			}



			throw std::runtime_error(fmt::format("Unexpected character: {}, at {}:{}", *it, line, col));
			break;

		}
		col++;
	}

	return result;
}

std::unordered_map<std::string, u16> Lexer::getMarksAddres(const std::vector<Lexer::Token>& tokens)
{
	int startAddr = 0x202; //Because 0x200 is jp _start
	return std::unordered_map<std::string, u16>();
}
