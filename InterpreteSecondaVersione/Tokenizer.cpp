#include <string>
#include <cstring>

#include "Tokenizer.h"
#include "Exceptions.h"

void Tokenizer::tokenizeInputFile(std::ifstream& inputFile,
	std::vector<Token>& inputTokens) {

	char ch;
	ch = inputFile.get();

	while (!inputFile.eof()) {
		if (std::isspace(ch)) {
			ch = inputFile.get();
			continue;
		}
		if (ch == '(') {
			inputTokens.push_back(Token{ Token::PAR_L, Token::id2word[Token::PAR_L] });
		}
		else if (ch == ')') {
			inputTokens.push_back(Token{ Token::PAR_R, Token::id2word[Token::PAR_R] });
		}
		else if (ch == '[') {
			inputTokens.push_back(Token{ Token::SQUARE_L, Token::id2word[Token::SQUARE_L] });
		}
		else if (ch == ']') {
			inputTokens.push_back(Token{ Token::SQUARE_R, Token::id2word[Token::SQUARE_R] });
		}
		else if (ch == '{') {
			inputTokens.push_back(Token{ Token::CURLY_L, Token::id2word[Token::CURLY_L] });
		}
		else if (ch == '}') {
			inputTokens.push_back(Token{ Token::CURLY_R, Token::id2word[Token::CURLY_R] });
		}
		else if (ch == '+') {
			inputTokens.push_back(Token{ Token::ADD, Token::id2word[Token::ADD] });
		}
		else if (ch == '-') {
			inputTokens.push_back(Token{ Token::MIN, Token::id2word[Token::MIN] });
		}
		else if (ch == '*') {
			inputTokens.push_back(Token{ Token::MUL, Token::id2word[Token::MUL] });
		}
		else if (ch == '/') {
			inputTokens.push_back(Token{ Token::DIV, Token::id2word[Token::DIV] });
		}

		else if (ch == '|') {
            ch = inputFile.get();
			if(ch == '|')
                inputTokens.push_back(Token{ Token::OR, Token::id2word[Token::OR] });
            else
                throw LexicalError("Errore lessicale sul simbolo: |");
		}

		else if (ch == '&') {
            ch = inputFile.get();
			if(ch == '&')
                inputTokens.push_back(Token{ Token::AND, Token::id2word[Token::AND] });
            else
                throw LexicalError("Errore lessicale sul simbolo: &");
		}

		else if (ch == '!') {
			ch = inputFile.get(); 
			if(ch == '=')
				inputTokens.push_back(Token{ Token::NOT_EQ, Token::id2word[Token::NOT_EQ] });
			else
            {
				inputTokens.push_back(Token{ Token::NOT, Token::id2word[Token::NOT] });
				//dato che il simbolo letto non è un "=", abbiamo letto qualcos'altro
				//è necessario usare continue per non ignorare in una chiamata successiva a inputFile.get() questo simbolo
				continue;
			}
		}

		else if (ch == '<') {
			ch = inputFile.get();
			if(ch == '=')
				inputTokens.push_back(Token{ Token::LESS_EQ, Token::id2word[Token::LESS_EQ] });
			else
            {
				inputTokens.push_back(Token{ Token::LESS, Token::id2word[Token::LESS] });
				continue;
			}
		}

		else if (ch == '>') {
			ch = inputFile.get();
			if(ch == '=')
				inputTokens.push_back(Token{ Token::MORE_EQ, Token::id2word[Token::MORE_EQ] });
            else
            {
				inputTokens.push_back(Token{ Token::MORE, Token::id2word[Token::MORE] });
				continue;
			}
		}

		else if (ch == '=') {
			ch = inputFile.get();
			if(ch == '=')
				inputTokens.push_back(Token{ Token::EQ, Token::id2word[Token::EQ] });

			else
            {
				inputTokens.push_back(Token{ Token::ASSIGN, Token::id2word[Token::ASSIGN] });
				continue;
			}
		}

		else if (ch == ';') {
			inputTokens.push_back(Token{ Token::SEMICOLON, Token::id2word[Token::SEMICOLON] });
		}

		//Se il carattere è alfanumerico si trova o un identificatore o una keyword (print, if,...) 
		else if (std::isalpha(ch)){
            std::string chars;

			//operazione di concatenazione
			chars+=ch;
			
			//dato che i prossimi caratteri possono anche essere numerici, controlliamo che siano
			//lettere alfabetiche oppure numeriche con alnum
			do
            {
                ch = inputFile.get();
                if(std::isalnum(ch))
                	chars+=ch;					
            }while(std::isalnum(ch));

			bool isKeyword = false; 

			for (int i = 0; i<Token::keywordsCount; i++)
			{
				//strcmp ritorna 0 se solo se le stringhe sono uguali
				if( std::strcmp(Token::id2word[i], chars.c_str()) == 0 )
				{
					inputTokens.push_back(Token{i, Token::id2word[i] });
					isKeyword = true;
					break;
				}
			}

			//se word non è una keyword allora è un identificatore	
			if(!isKeyword)
				inputTokens.push_back(Token{ Token::ID, chars});

			//continue è necessario per non effetuare un ulteriore inputFile.get() che skipperebbe
			//caratteri non ancora visitati
			continue;
        }

		//Si cercano valori numerici
		else if (std::isdigit(ch)) {
			std::string chars;
			chars += ch;
			do {
				ch = inputFile.get();
				if (std::isdigit(ch)) 
					chars+=ch;
			} while (std::isdigit(ch));
			inputTokens.push_back(Token{ Token::NUM, chars});
			continue;
		}
		else {
			throw LexicalError(std::string("Non si è riconosciuto il simbolo: ")+=ch);
		}
		ch = inputFile.get();
	}
}