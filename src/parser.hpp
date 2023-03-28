#pragma once

#include <string>
#include <vector>

#include "ast.hpp"

namespace nixvm::parser {
// Nix expression language parser
using namespace nixvm::ast;

struct ParserError {
		std::string msg;
		int line;
		int col;

		ParserError(std::string msg, int line, int col);
};

class Parser {
public:
		Parser(const std::string& input);
		std::shared_ptr<Expr> parse();
		std::vector<ParserError> getErrors();
private:
		std::shared_ptr<Expr> parseExpr();
		std::shared_ptr<Ident> parseIdent();
		std::shared_ptr<Int> parseInt();
		std::shared_ptr<Str> parseStr();
		std::shared_ptr<Bool> parseBool();
		std::shared_ptr<List> parseList();
		std::shared_ptr<Set> parseSet();
		std::shared_ptr<Attr> parseAttr();
		std::shared_ptr<BinOp> parseBinOp();
		std::shared_ptr<UnaryOp> parseUnaryOp();
		std::shared_ptr<If> parseIf();
		std::shared_ptr<Let> parseLet();
		// TODO: parseWith, parseAssert, parseRecSet

		bool isIdentStart(char c);
		bool isIdentChar(char c);
		bool isBinOpChar(char c);
		bool isWhiteSpace(char c);

		void skipWhiteSpace();
		void skipComment();
		void skipLineComment();
		void skipBlockComment();

		void expect(char c);
		void expect(const std::string& s);

		void error(const std::string& msg);

		std::string input;
		size_t pos;
		int line;
		int col;

		std::vector<ParserError> errors;
};
}
