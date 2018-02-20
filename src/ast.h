#pragma once

#include <vector>
#include <string>
#include <memory>
#include <initializer_list>
#include <assert.h>


class term {
    std::string _name;
    enum kind {
        variable,
        atom,
        structure
    } _kind;
    std::vector<term> _args;

    constexpr static struct variable_t {} variable_marker{};
    constexpr static struct atom_t {} atom_marker{};

    term(variable_t, std::string name): _name(std::move(name)), _kind(kind::variable) {}
    term(atom_t, std::string name): _name(std::move(name)), _kind(kind::atom) {}
    term(std::string name, std::vector<term> args):
            _name(std::move(name)), _kind(kind::structure), _args(std::move(args)) {}

public:
    term(const term &) = delete;
    term(term &&) = default;
    term &operator=(const term &) = delete;
    term &operator=(term &&) = default;

    const std::string &name() const & noexcept {
        return _name;
    }

    std::string name() && noexcept {
        return std::move(_name);
    }

    const std::vector<term> & args() const & noexcept {
        return _args;
    }

    std::vector<term> args() && noexcept {
        return std::move(_args);
    }

    friend term make_variable(std::string name);
    friend term make_atom(std::string name);
    friend term make_structure(std::string name, std::vector<term> args);
    friend std::ostream &operator<<(std::ostream &, const term &t);
};

term make_variable(std::string name);
term make_atom(std::string name);
term make_structure(std::string name, std::vector<term> args);

/*
namespace detail {

    class default_movable {
    public:
        default_movable() = default;
        default_movable(default_movable &&) noexcept = default;
        default_movable &operator=(default_movable &&) noexcept = default;
    };

    class noncopyable {
    public:
        noncopyable() = default;
        noncopyable(const noncopyable &) = delete;
        noncopyable &operator=(const noncopyable &) = delete;
    };

    class has_name : private default_movable, private noncopyable {
        std::string _name;
    public:
        has_name(std::string name): _name(std::move(name)) {}

        const std::string &name() const & noexcept {
            return _name;
        }

        std::string name() && noexcept {
            return std::move(_name);
        }
    };

}

class term;

class variable : private detail::has_name {
public:
    variable(std::string name): has_name(std::move(name)) {}
};


class atom : public detail::has_name {
public:
    atom(std::string name): has_name(std::move(name)) {}
};

class functor : private detail::noncopyable, detail::default_movable {
    atom _fname;
    size_t _arity;

public:
    functor(atom fname, size_t arity): _fname(std::move(fname)), _arity(arity) {}
    size_t arity() { return _arity; }
    const std::string &name() const & noexcept {
        return _fname.name();
    }

    std::string name() && noexcept {
        return std::move(_fname).name();
    }
};


class structure : private detail::noncopyable, private detail::default_movable {
    functor _functor;
    std::vector<term> _args;
public:
    structure(functor functor, std::initializer_list<term> ilist): _functor(std::move(functor)), _args(ilist) {
        assert(functor.arity() == ilist.size());
    }

    structure(functor functor, std::vector<term> args): _functor(std::move(functor)), _args(std::move(args)) {
        assert(functor.arity() == args.size());
    }

};

class term {};





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
 */
