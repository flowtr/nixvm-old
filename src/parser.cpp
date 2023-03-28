#include "parser.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "ast.hpp"
#include "parser.hpp"

namespace nixvm::parser {
Parser::Parser(const std::string& input) {
	this->input = input;
	this->pos = 0;
	this->line = 1;
	this->col = 1;

	this->errors = std::vector<ParserError>();
}

bool Parser::isIdentStart(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Parser::isIdentChar(char c) {
	return isIdentStart(c) || (c >= '0' && c <= '9') || c == '-' || c == '\'' || c == '.';
}

bool Parser::isBinOpChar(char c) {
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '<' || c == '>' || c == '=' || c == '&' || c == '|' || c == '!' || c == '?';
}

bool Parser::isWhiteSpace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

void Parser::skipWhiteSpace() {
	while (pos < input.size() && isWhiteSpace(input[pos])) {
		if (input[pos] == '\n') {
			line++;
			col = 1;
		} else {
			col++;
		}

		pos++;
	}
}

void Parser::skipComment() {
	if (pos + 1 < input.size() && input[pos] == '/' && input[pos + 1] == '/') {
		skipLineComment();
	} else if (pos + 1 < input.size() && input[pos] == '/' && input[pos + 1] == '*') {
		skipBlockComment();
	}
}

void Parser::skipLineComment() {
	while (pos < input.size() && input[pos] != '\n') {
		pos++;
	}

	if (pos < input.size()) {
		pos++;
		line++;
		col = 1;
	}

	skipWhiteSpace();
}

void Parser::skipBlockComment() {
	pos += 2;
	col += 2;

	while (pos < input.size() && !(input[pos] == '*' && pos + 1 < input.size() && input[pos + 1] == '/')) {
		if (input[pos] == '\n') {
			line++;
			col = 1;
		} else {
			col++;
		}

		pos++;
	}

	if (pos < input.size()) {
		pos += 2;
		col += 2;
	}

	skipWhiteSpace();
}

void Parser::expect(char c) {
	if (pos >= input.size()) {
		error("unexpected end of input");
	}

	if (input[pos] != c) {
		error("expected '" + std::string(1, c) + "'");
	}

	pos++;
	col++;
}

void Parser::expect(const std::string& s) {
	for (char c : s) {
		expect(c);
	}
}

void Parser::error(const std::string& msg) {
	errors.push_back(ParserError(msg, line, col));
}

std::vector<ParserError> Parser::getErrors() {
	return errors;
}

std::shared_ptr<Expr> Parser::parse() {
	std::shared_ptr<Expr> expr = parseExpr();
	if (pos < input.size()) {
		error("unexpected token");
	}
	return expr;
}

ParserError::ParserError(std::string msg, int line, int col) {
	this->msg = msg;
	this->line = line;
	this->col = col;
}

std::shared_ptr<Expr> Parser::parseExpr() {
	skipWhiteSpace();
	if (pos >= input.size()) {
		error("unexpected end of input");
	}
	switch (input[pos]) {
		case '0'...'9':
			return parseInt();
		case '"':
			return parseStr();
		case '[':
			return parseList();
		case '{':
			return parseSet();
		case '(':
			return parseBinOp();
		case '!':
			return parseUnaryOp();
		case 'i':
			return parseIf();
		case 'l':
			return parseLet();
		case 't':
		case 'f':
			return parseBool();
		// TODO: parse with, assert, etc.
		default:
			return parseIdent();
	}

	return nullptr;
}

std::shared_ptr<Bool> Parser::parseBool() {
	if (pos + 3 < input.size() && input.substr(pos, 4) == "true") {
		pos += 4;
		col += 4;
		return std::make_shared<Bool>(true);
	} else if (pos + 4 < input.size() && input.substr(pos, 5) == "false") {
		pos += 5;
		col += 5;
		return std::make_shared<Bool>(false);
	}

	return nullptr;
}

std::shared_ptr<Ident> Parser::parseIdent() {
	std::string name;
	while (pos < input.size() && isIdentChar(input[pos])) {
		name += input[pos];
		pos++;
		col++;
	}

	return std::make_shared<Ident>(name);
}

std::shared_ptr<Int> Parser::parseInt() {
	std::string num;
	while (pos < input.size() && input[pos] >= '0' && input[pos] <= '9') {
		num += input[pos];
		pos++;
		col++;
	}

	return std::make_shared<Int>(std::stoi(num));
}

std::shared_ptr<Str> Parser::parseStr() {
	std::string str;
	pos++;
	col++;
	while (pos < input.size() && input[pos] != '"') {
		str += input[pos];
		pos++;
		col++;
	}
	pos++;
	col++;

	return std::make_shared<Str>(str);
}

std::shared_ptr<List> Parser::parseList() {
	std::vector<std::shared_ptr<Expr>> elems;
	pos++;
	col++;
	skipWhiteSpace();
	while (pos < input.size() && input[pos] != ']') {
		elems.push_back(parseExpr());
		skipWhiteSpace();
		if (pos < input.size() && input[pos] == ',') {
			pos++;
			col++;
			skipWhiteSpace();
		}
	}
	if (pos >= input.size()) {
		error("unexpected end of input");
	}
	pos++;
	col++;

	return std::make_shared<List>(elems);
}

std::shared_ptr<Set> Parser::parseSet() {
	std::vector<std::shared_ptr<Attr>> attrs;
	pos++;
	col++;
	skipWhiteSpace();
	while (pos < input.size() && input[pos] != '}') {
		attrs.push_back(parseAttr());
		skipWhiteSpace();
		if (pos < input.size() && input[pos] == ',') {
			pos++;
			col++;
			skipWhiteSpace();
		}
	}
	if (pos >= input.size()) {
		error("unexpected end of input");
	}
	pos++;
	col++;
	return std::make_shared<Set>(attrs);
}

std::shared_ptr<BinOp> Parser::parseBinOp() {
	expect('(');
	skipWhiteSpace();
	std::shared_ptr<Expr> lhs = parseExpr();
	skipWhiteSpace();

	std::string op;
	while (pos < input.size() && isBinOpChar(input[pos])) {
		op += input[pos];
		pos++;
		col++;
	}

	skipWhiteSpace();
	std::shared_ptr<Expr> rhs = parseExpr();
	skipWhiteSpace();
	expect(')');
	return std::make_shared<BinOp>(lhs, op, rhs);
}

std::shared_ptr<UnaryOp> Parser::parseUnaryOp() {
	expect('!');
	skipWhiteSpace();

	std::string op;
	while (pos < input.size() && isBinOpChar(input[pos])) {
		op += input[pos];
		pos++;
		col++;
	}

	skipWhiteSpace();
	std::shared_ptr<Expr> expr = parseExpr();
	return std::make_shared<UnaryOp>(expr, op);
}

std::shared_ptr<If> Parser::parseIf() {
	expect("if");
	skipWhiteSpace();
	std::shared_ptr<Expr> cond = parseExpr();
	skipWhiteSpace();
	expect("then");
	skipWhiteSpace();
	std::shared_ptr<Expr> thenExpr = parseExpr();
	skipWhiteSpace();

	std::shared_ptr<Expr> elseExpr = nullptr;
	if (pos < input.size() && input[pos] == 'e') {
		expect("else");
		skipWhiteSpace();
		elseExpr = parseExpr();
		skipWhiteSpace();
	}

	return std::make_shared<If>(cond, thenExpr, elseExpr);
}

std::shared_ptr<Let> Parser::parseLet() {
	expect("let");
	skipWhiteSpace();

	// let x = 1; y = 1 in x
	
	std::vector<std::shared_ptr<Attr>> bindings;
 	while (pos < input.size() && input[pos] != 'i') {
		auto ident = parseIdent();
		skipWhiteSpace();
		expect('=');
		skipWhiteSpace();
		std::shared_ptr<Expr> expr = parseExpr();
		bindings.push_back(std::make_shared<Attr>(ident, expr));

		if (pos < input.size() && input[pos] == ';') {
			pos++;
			col++;
			skipWhiteSpace();
		}
	}

	expect("in");
	skipWhiteSpace();
	std::shared_ptr<Expr> expr = parseExpr();
	return std::make_shared<Let>(bindings, expr);
}

std::shared_ptr<Attr> Parser::parseAttr() {
	std::shared_ptr<Ident> ident = parseIdent();
	skipWhiteSpace();
	expect('=');
	skipWhiteSpace();
	std::shared_ptr<Expr> expr = parseExpr();
	return std::make_shared<Attr>(ident, expr);
}
}
