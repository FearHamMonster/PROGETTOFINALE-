#ifndef TOKEN_H
#define TOKEN_H

#include <string>
struct Token {

	//per garantire il corretto funzionamento del tokenizer è necessario collocare 
	//tutte le keyword id prima degli altri token id
	enum tokenIds {
	 IF, ELSE, DO, WHILE, BREAK, INT_CONST, BOOL_CONST, TRUE, FALSE, PRINT,
	 NUM, ID, PAR_L, PAR_R, SQUARE_L, SQUARE_R, CURLY_L, CURLY_R, ADD, MIN, MUL, DIV,
	 OR, AND, EQ, NOT_EQ, LESS, LESS_EQ, MORE, MORE_EQ, NOT, ASSIGN, SEMICOLON, 
	};

	static constexpr const char* id2word[] = {"if", "else", "do",
	"while", "break", "int", "boolean", "true", "false","print","NUM", "ID","(", ")","[","]","{","}","+", "-", "*", "/", "||", "&&", "==", "!=", "<", "<=", ">", ">=", "!", "=", ";", };

	//il numero di keywords
	static const int keywordsCount = 10;
	
	Token(int t, const char* w) : tag{ t }, word{ w } { }
	Token(int t, std::string w) : tag{ t }, word{ w } { }
	~Token() = default;
	Token(Token const&) = default;
	Token& operator=(Token const&) = default;

	// La coppia (ID, parola) che costituisce il Token
	int tag;
	std::string word;
};




std::ostream& operator<<(std::ostream& os, const Token& t);

#endif
