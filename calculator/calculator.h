#pragma once

#include <cstdint>
#include <cstdio>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

class Tokenizer {
public:
    explicit Tokenizer(std::istream* in) : in_{in} {
        Consume();
    }

    enum TokenType { kNumber, kSymbol, kEnd };

    void Consume() {
        if (in_->peek() == EOF) {
            type_ = TokenType::kEnd;
            return;
        }

        while (in_->peek() == ' ') {
            in_->ignore();
        }

        if (std::isdigit(in_->peek())) {
            std::string cur_num;

            while (std::isdigit(in_->peek())) {
                cur_num += in_->peek();
                in_->ignore();
            }

            type_ = TokenType::kNumber;
            number_ = std::stoll(cur_num);

        } else {
            type_ = TokenType::kSymbol;
            symbol_ = in_->peek();

            in_->ignore();
        }
    }

    TokenType GetType() const {
        return type_;
    }

    int64_t GetNumber() const {
        return number_;
    }

    char GetSymbol() const {
        return symbol_;
    }

private:
    std::istream* in_;

    TokenType type_;
    int64_t number_;
    char symbol_;
};

class Expression {
public:
    virtual ~Expression() {}
    virtual int64_t Evaluate() const = 0;
};

using unique_ptr = std::unique_ptr<Expression>;

class BinaryExpression : public Expression {
public:
    BinaryExpression(): left(nullptr), right(nullptr) {
    }

    BinaryExpression(unique_ptr left, unique_ptr right):
        left(std::move(left)), right(std::move(right)) {
    }

protected:
    unique_ptr left;
    unique_ptr right;
};

class Primary : public Expression {
public:
    Primary(int64_t value): value(value) {
    }

    int64_t Evaluate() const override {
        return value;
    }

protected:
    int64_t value;
};

class Unary : public Expression {
public:
    Unary(int64_t minus, unique_ptr left): minus(minus), left(std::move(left)) {
    }

    int64_t Evaluate() const override {
        int64_t res = left->Evaluate();
        res *= minus;
        return res;
    }

protected:
    int64_t minus;
    unique_ptr left = nullptr;
};

class Add : public BinaryExpression {
public:
    Add(unique_ptr left, unique_ptr right): BinaryExpression(std::move(left), std::move(right)) {
    }

    int64_t Evaluate() const override {
        int64_t left_value = 0;
        if (left) {
            left_value = left->Evaluate();
        }
        int64_t right_value = 0;
        if (right) {
            right_value = right->Evaluate();
        }
        return left_value + right_value;
    }
};

class Sub : public BinaryExpression {
public:
    Sub(unique_ptr left, unique_ptr right): BinaryExpression(std::move(left), std::move(right)) {
    }

    int64_t Evaluate() const override {
        int64_t left_value = 0;
        if (left) {
            left_value = left->Evaluate();
        }
        int64_t right_value = 0;
        if (right) {
            right_value = right->Evaluate();
        }
        return left_value - right_value;
    }
};

class Mul : public BinaryExpression {
public:
    Mul(unique_ptr left, unique_ptr right): BinaryExpression(std::move(left), std::move(right)) {
    }

    int64_t Evaluate() const override {
        int64_t left_value = 0;
        if (left) {
            left_value = left->Evaluate();
        }
        int64_t right_value = 0;
        if (right) {
            right_value = right->Evaluate();
        }
        return left_value * right_value;
    }
};

class Div : public BinaryExpression {
public:
    Div(unique_ptr left, unique_ptr right): BinaryExpression(std::move(left), std::move(right)) {
    }

    int64_t Evaluate() const override {
        int64_t left_value = 0;
        if (left) {
            left_value = left->Evaluate();
        }
        int64_t right_value = 0;
        if (right) {
            right_value = right->Evaluate();
        }
        return left_value / right_value;
    }
};

unique_ptr ParseAddSub(Tokenizer*); 

unique_ptr ParsePrimary(Tokenizer* tk) {
    unique_ptr res = nullptr;
    if (tk->GetType() == Tokenizer::kSymbol && tk->GetSymbol() == '(') {
        tk->Consume();
        res = ParseAddSub(tk);
    } else {
        res = unique_ptr(new Primary(tk->GetNumber()));
    }
    tk->Consume();
    return res;
}

unique_ptr ParseUnary(Tokenizer* tk) {
    if (tk->GetType() == Tokenizer::kSymbol && tk->GetSymbol() == '-') {
        tk->Consume();
        return unique_ptr(new Unary(-1, ParseUnary(tk)));
    } else {
        return unique_ptr(ParsePrimary(tk));
    }
}

unique_ptr ParseMulDiv(Tokenizer* tk) {
    unique_ptr left = ParseUnary(tk); 
    while (tk->GetType() != Tokenizer::kEnd) {
        if (tk->GetType() == Tokenizer::kSymbol && tk->GetSymbol() == ')') {
            break;
        }

        char oper = tk->GetSymbol();
        if (oper != '*' && oper != '/') {
            break;
        }

        tk->Consume();
        unique_ptr right = ParseUnary(tk);

        if (oper == '*') {
            unique_ptr mul(new Mul(std::move(left), std::move(right)));
            left = std::move(mul);
        }

        if (oper == '/') {
            unique_ptr div(new Div(std::move(left), std::move(right)));
            left = std::move(div);
        }
    }
    return std::move(left);
}

unique_ptr ParseAddSub(Tokenizer* tk) {
    unique_ptr left = ParseMulDiv(tk); 
    while (tk->GetType() != Tokenizer::kEnd) {
        if (tk->GetType() == Tokenizer::kSymbol && tk->GetSymbol() == ')') {
            break;
        }

        char oper = tk->GetSymbol();
        if (oper != '+' && oper != '-') {
            break;
        }
        tk->Consume();
        unique_ptr right = ParseMulDiv(tk);

        if (oper == '+') {
            unique_ptr add(new Add(std::move(left), std::move(right)));
            left = std::move(add);
        }

        if (oper == '-') {
            unique_ptr sub(new Sub(std::move(left), std::move(right)));
            left = std::move(sub);
        }
    }
    return left;
}

unique_ptr ParseExpression(Tokenizer* tk) {
    unique_ptr res = ParseAddSub(tk);
    return res;
}
