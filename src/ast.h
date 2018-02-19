//
// Created by 冯泽灵 on 2017/1/29.
//
#ifndef PROLOG0_AST_H
#define PROLOG0_AST_H

#include <string>

class term {
public:
    struct variable {
    public:
        std::string name_;
        variable(const std::string &name): name_(name) { std::assert(std::islower(name_[0])); }
        variable(const variable &rhs): name_(rhs.name_) {}
        variable(variable &&rhs) { std::swap(name_, rhs.name_); }

        template <typename T>
        void operator=(T &&rhs) {
            variable tmp(rhs);
            swap(*this, rhs);
        }
        void operator=(const std::string &rhs): name_(rhs.name_) {}
        void operator=( &&rhs) { std::swap(name_, rhs.name_); }
    };
    struct atom {
        std::string name_;
        atom(const std::string &name): name_(name) { std::assert(std::isupper(name_[0])); }
        atom(const atom &rhs): name_(rhs) {}
        atom(atom &&rhs) { std::swap(name_, rhs.name_); }
        void operator=(const std::string &rhs): name_(rhs.name_) {}
        void operator=()
    };
    struct functor {
        atom fname_;
        int arity_;
    };
    struct structure {
        functor f_;
        std::list<term> args_;
    };

private:
    struct functor;
    enum TAG {
        ATM,
        VAR,
        STR,
    } tag;

    union {
        atom a;
        variable v;
        structure s;
    };

};


#endif //PROLOG0_AST_H
