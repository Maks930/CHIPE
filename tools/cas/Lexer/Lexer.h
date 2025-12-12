#ifndef LEXER_H
#define LEXER_H

#include <variant>
#include <Types.h>

/*
* Words		OpCode		[Mn			OPERANDS			]			DESC
* cls		00E0		cls											clear display
* ret		00EE		ret											return 
* sys		0nnn		sys			addr							jump to machine code routine at nnn				
* jp		1nnn		jp			addr						    jump to location nnn
* call		2nnn		call		addr							Call subtoutine at nnn				
* se		3xkk		se			Vx,   byte						Skip next instruction if Vx == kk					
* sne		4xkk		sne			Vx,   byte						Skin next instruction if Vx != kk					
* se		5xy0		se			Vx,   Vy						Skip next instruction if Vx == Vy					
* ld/mov	6xkk		ld/mov		Vx,   byte						Set Vx = kk					
* add		7xkk		add			Vx,   byte						Set Vx = Vx + kk					
* ld/mov	8xy0		ld/mov		Vx,   Vy						Set Vx = Vy					
* or		8xy1		or			Vx,   Vy						Set Vx = Vx | Vy					
* and		8xy2		and			Vx,   Vy						Set Vx = Vx	& Vy				
* xor		8xy3		xor			Vx,   Vy						Set Vx = Vx	^ Vy				
* add		8xy4		add			Vx,   Vy						Set Vx = Vx	+ Vy, set VF = carry				
* sub		8xy5		sub			Vx,   Vy						Set Vx = Vx - Vy, set VF = NOT borrow. If Vx > Vy, then VF is set to 1, otherwise 0.					
* shr		8xy6		shr			Vx,   Vy						Set Vx = Vx	>> 1, If the least-significant bits of Vx is 1, then VF os set to 1, otherwise 0. EQ Vx = Vx/2			
* subn		8xy7		subn		Vx,   Vy						Set Vx = Vx - Vy, set VF = NOT borrow. If Vy > Vx, then VF is set to 1, otherwise 0.					
* shl		8xyE		shl			Vx,   Vy						Set Vx = Vx	<< 1, If the most-significant bit of Vx is 1, then VF is set to 1, otherwise 0.	  EQ Vx = Vx*2			
* sne		9xy0		sne			Vx,   Vy						Skip next instruction if Vx != Vy				
* ld/mov	Annn		ld/mov		 I,   addr						Set I = nnnn					
* jp		Bnnn		jp			V0,   addr						Jump to location nnn + V0					
* rnd		Cxkk		rnd			Vx,   byte						Set Vx = random byte & kk					
* drw		Dxyn		drw			Vx,   Vy,	nibble				Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision							
* skp		Ex9E		skp			Vx								Skip next instruction if key with the value of Vx is pressed				
* skpn		ExA1		skpn		Vx								Skip next instruction if key with the value of Vx is not pressed								
* ld/mov	Fx07		ld/mov		Vx,   DT						Set Vx = delay timer value. The value of DT placed into Vx					
* ld/mov	Fx0A		ld/mov		Vx,   K							Wait for a key press, store the value of the key Vx				
* ld/mov	Fx15		ld/mov		DT,   Vx						Set delay timer = Vx. DT is set equal to the value of Vx					
* ld/mov	Fx18		ld/mov		ST,	  Vx						Set sound timer = Vx. ST is set equal to the value of Vx					
* add		Fx1E		add			 I,   Vx						Set I = I + Vx					
* ld/mov	Fx29		ld/mov		 F,   Vx						Set I = location of sprite for digit Vx					
* ld/mov	Fx33		ld/mov		 B,   Vx						Store BCD representation of Vx in memory locations I, I+1, and I+2					
* ld/mov	Fx55		ld/mov		[I],  Vx						Store registers V0 through Vx in memory starting at location I					
* ld/mov	Fx65		ld/mov		 Vx,  [I]						Read registers V0 through Vx from memory starting at location I					
* 
*/

/* Memory Map
* +---------------+= 0xFFF (4095) End Of Chip-8 RAM
* |				  |
* |				  |
* |				  |
* |				  |
* |				  |
* | 0x200 to 0xFFF|
* |     Chip-8    |
* | Program / Data|
* |     Space     |
* |               |
* |               |
* |               |
* +- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
* |               |
* |               |
* |               |
* +---------------+= 0x200 (512) Start of most Chip-8 programs
* | 0x000 to 0x1FF|
* | Reserved for  |
* |  interpreter  |
* +---------------+= 0x000 (0) Start of Chip--8 RAM
*/

/* Phys Keyboard
* +-------+-------+-------+-------+
* |       |       |       |       |
* |   1   |   2   |   3   |   C   |
* |       |       |       |       |
* +-------+-------+-------+-------+
* |       |       |       |       |
* |   4   |   5   |   6   |   D   |
* |       |       |       |       |
* +-------+-------+-------+-------+
* |       |       |       |       |
* |   7   |   9   |   9   |   E   |
* |       |       |       |       |
* +-------+-------+-------+-------+
* |       |       |       |       |
* |   A   |   0   |   B   |   F   |
* |       |       |       |       |
* +-------+-------+-------+-------+
*/

/* Display
*  +-----------------------+
*  | (0,0)          (63,0) |
*  |                       |
*  | (0,31)         (63,31)|
*  +-----------------------+
*/

/*  Fonts
*	Dot is Zero
* 
*   +------+----------+------+------+----------+------+------+----------+------+------+----------+------+
*   | NUM  | Binary   | Hex  | NUM  | Binary   | Hex  | NUM  | Binary   | Hex  | NUM  | Binary   | Hex  |
*	+------+----------+------+------+----------+------+------+----------+------+------+----------+------+
*   | **** | 1111.... | 0xF0 |   *  | ..1..... | 0x20 | **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 |
*   | *  * | 1..1.... | 0x90 |  **  | .11..... | 0x60 |    * | ...1.... | 0x10 |    * | ...1.... | 0x10 |
*   | *  * | 1..1.... | 0x90 |   *  | ..1..... | 0x20 | **** | 1111.... | 0xF0 | **** | 1111.... | 0x00 |
*   | *  * | 1..1.... | 0x90 |   *  | ..1..... | 0x20 | *    | 1....... | 0x80 |    * | ...1.... | 0x10 |
*   | **** | 1111.... | 0xF0 |  *** | .111.... | 0x70 | **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 |
*   +------+----------+------+------+----------+------+------+----------+------+------+----------+------+
* 	| *  * | 1..1.... | 0x90 | **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 |
* 	| *  * | 1..1.... | 0x90 | *    | 1....... | 0x80 | *    | 1....... | 0x80 |    * | ...1.... | 0x10 |
* 	| **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 |   *  | ..1..... | 0x20 |
* 	|    * | ...1.... | 0x10 |    * | ...1.... | 0x10 | *  * | 1..1.... | 0x90 |  *   | .1...... | 0x40 |
* 	|    * | ...1.... | 0x10 | **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 |  *   | .1...... | 0x40 |
* 	+------+----------+------+------+----------+------+------+----------+------+------+----------+------+
* 	| **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 | ***  | 111..... | 0xE0 |
* 	| *  * | 1..1.... | 0x90 | *  * | 1..1.... | 0x90 | *  * | 1..1.... | 0x90 | *  * | 1..1.... | 0x90 |
* 	| **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 | ***  | 111..... | 0xE0 |
* 	| *  * | 1..1.... | 0x90 |    * | ...1.... | 0x10 | *  * | 1..1.... | 0x90 | *  * | 1..1.... | 0x90 |
* 	| **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 | *  * | 1..1.... | 0x90 | ***  | 111..... | 0xE0 |
* 	+------+----------+------+------+----------+------+------+----------+------+------+----------+------+
* 	| **** | 1111.... | 0xF0 | ***  | 111..... | 0xE0 | **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 |
* 	| *    | 1....... | 0x80 | *  * | 1..1.... | 0x90 | *    | 1....... | 0x80 | *    | 1....... | 0x80 |
* 	| *    | 1....... | 0x80 | *  * | 1..1.... | 0x90 | **** | 1111.... | 0xF0 | **** | 1111.... | 0xF0 |
* 	| *    | 1....... | 0x80 | *  * | 1..1.... | 0x90 | *    | 1....... | 0x80 | *    | 1....... | 0x80 |
*   | **** | 1111.... | 0xF0 | ***  | 111..... | 0xE0 | **** | 1111.... | 0xF0 | *    | 1....... | 0x80 |
*   +------+----------+------+------+----------+------+------+----------+------+------+----------+------+
*/

#include <unordered_map>
#include <vector>
#include <string>

class Lexer {
public:

	struct Mark { 
		std::string idf;
		auto operator <=>(const Mark&) const = default;
	};
	struct Identifier { 
		std::string idf;
		auto operator<=>(const Identifier&) const = default;
	};
	struct Integer { 
		u32 value;
		auto operator<=>(const Integer&) const = default;
	};
	struct LSQPar {
		auto operator<=>(const LSQPar&) const = default;
	};
	struct RSQPar {
		auto operator<=>(const RSQPar&) const = default;
	};
	struct Plus {
		auto operator<=>(const Plus&) const = default;
	};
	struct Sub {
		auto operator<=>(const Sub&) const = default;
	};

	enum class Keyword : int {
		DB = 0,
		DW,

		CLS, RET, SYS, JP,
		CALL, SE, SNE, LD,
		ADD, OR, AND, XOR,
		SUB, SHR, SUBN, SHL,
		RND, DRW, HLT,
	};

	enum class Register : int {
		V1=0, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB, VC, VD, VE, VF
	};

	using Token = std::variant<
		Mark, Identifier, Integer,
		LSQPar, RSQPar, Plus, Sub,
		Keyword, Register
	>;
	static const std::unordered_map<std::string_view, Keyword> keywords;
private:
	std::unordered_map<std::string, u16> m_marks;
	std::vector<Lexer::Token> m_tokens;

	

	
public:

	Lexer() {};
	~Lexer() {}


	std::vector<Lexer::Token> processed(std::string_view& code);
	std::unordered_map<std::string, u16> getMarksAddres(const std::vector<Lexer::Token>& tokens);
};

#endif //LEXER_H
