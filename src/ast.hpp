#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nixvm::ast {
// Nix expression language AST

struct Expr {
	virtual ~Expr() = default;

	virtual std::string toString() = 0;
};

struct Ident : Expr {
	std::string name;

	Ident(const std::string& name) : name(name) {}

	std::string toString() override {
		return name;
	}
};

struct Int : Expr {
	int value;

	Int(int value) : value(value) {}

	std::string toString() override {
		return std::to_string(value);
	}
};

struct Str : Expr {
	std::string value;

	Str(const std::string& value) : value(value) {}

	std::string toString() override {
		return '"' + value + '"';
	}
};

struct Bool : Expr {
	bool value;

	Bool(bool value) : value(value) {}

	std::string toString() override {
		return value ? "true" : "false";
	}
};

struct List : Expr {
	std::vector<std::shared_ptr<Expr>> values;

	List(const std::vector<std::shared_ptr<Expr>>& values) : values(values) {}

	std::string toString() override {
		std::string str = "[";
		for (auto& value : values) {
			str += value->toString() + " ";
		}
		str += "]";
		return str;
	}
};


struct Attr : Expr {
	std::shared_ptr<Expr> set;
	std::shared_ptr<Expr> key;

	Attr(std::shared_ptr<Expr> set, std::shared_ptr<Expr> key) : set(set), key(key) {}

	std::string toString() override {
		return set->toString() + "." + key->toString();
	}
};

struct Set : Expr {
	std::vector<std::shared_ptr<Attr>> values;

	Set(const std::vector<std::shared_ptr<Attr>>& values) : values(values) {}

	std::string toString() override {
		std::string str = "{";
		for (auto& value : values) {
			str += value->toString() + " ";
		}
		str += "}";
		return str;
	}
};

struct BinOp : Expr {
	std::shared_ptr<Expr> lhs;
	std::shared_ptr<Expr> rhs;
	std::string op;

	BinOp(std::shared_ptr<Expr> lhs, const std::string& op, std::shared_ptr<Expr> rhs) : lhs(lhs), rhs(rhs), op(op) {}

	std::string toString() override {
		return "(" + lhs->toString() + " " + op + " " + rhs->toString() + ")";
	}
};

struct UnaryOp : Expr {
	std::shared_ptr<Expr> expr;
	std::string op;

	UnaryOp(std::shared_ptr<Expr> expr, const std::string& op) : expr(expr), op(op) {}

	std::string toString() override {
		return "(" + op + " " + expr->toString() + ")";
	}
};

struct If : Expr {
	std::shared_ptr<Expr> cond;
	std::shared_ptr<Expr> then;
	std::shared_ptr<Expr> else_;

	If(std::shared_ptr<Expr> cond, std::shared_ptr<Expr> then, std::shared_ptr<Expr> else_) : cond(cond), then(then), else_(else_) {}

	std::string toString() override {
		return "(if " + cond->toString() + " then " + then->toString() + " else " + else_->toString() + ")";
	}
};

struct Let : Expr {
	std::vector<std::shared_ptr<Attr>> attrs;
	std::shared_ptr<Expr> body;

	Let(const std::vector<std::shared_ptr<Attr>>& attrs, std::shared_ptr<Expr> body) : attrs(attrs), body(body) {}

	std::string toString() override {
		std::string str = "(let ";
		for (auto& attr : attrs) {
			str += attr->toString() + " ";
		}
		str += "in " + body->toString() + ")";
		return str;
	}
};

struct Lambda : Expr {
	std::vector<std::shared_ptr<Expr>> params;
	std::shared_ptr<Expr> body;

	Lambda(const std::vector<std::shared_ptr<Expr>>& params, std::shared_ptr<Expr> body) : params(params), body(body) {}

	std::string toString() override {
		std::string str = "(lambda ";
		for (auto& param : params) {
			str += param->toString() + " ";
		}
		str += "in " + body->toString() + ")";
		return str;
	}
};
}

