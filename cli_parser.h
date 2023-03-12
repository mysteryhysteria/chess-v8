#pragma once

#include <functional>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <utility>
#include <tuple>
#include <optional>
#include <algorithm>
#include "assert.h"

enum class States { COMMAND, ARG_NAME, ARG, EXEC };

struct required_argument_t {};
constexpr required_argument_t required_argument = required_argument_t();

struct optional_flag_argument_t {};
constexpr optional_flag_argument_t optional_flag_argument = optional_flag_argument_t();

struct optional_value_argument_t {};
constexpr optional_value_argument_t optional_value_argument = optional_value_argument_t();

class AbsArgument {
public:
	virtual inline bool is_arg_required() = 0;
	virtual inline bool is_arg_optional() = 0;
	virtual inline bool is_arg_value_only() = 0;
	virtual inline bool is_arg_flag_only() = 0;
	virtual inline bool is_arg_named_value() = 0;
	virtual inline bool is_arg_named() = 0;
	virtual inline bool is_arg_valued() = 0;
	virtual inline bool has_value() = 0;
	virtual inline void reset() = 0;

	// defined these so that the << and >> operator overloads do not have to touch the private
	// data of the concrete implementations
	virtual inline std::ostream& print(std::ostream& out) = 0;
	virtual inline std::istream& parse(std::istream& in) = 0;

	friend std::ostream& operator<<(std::ostream& out, AbsArgument& arg) { return arg.print(out); };
	friend std::istream& operator>>(std::istream& in, AbsArgument& arg) { return arg.parse(in); };
};

template<class T>
class Argument : public AbsArgument {
private:
	bool required;
	void gen_short_name() {
		this->short_name.clear();
		bool next = true;
		for (auto it = this->arg_name.begin(); it != this->arg_name.end(); ++it) {
			if (*it == '-') {
				next = true;
			}
			else {
				if (next) {
					this->short_name.push_back(*it);
					next = false;
				}
			}
		}
	};

public:
	std::string arg_name;
	std::string short_name;
	std::string val_str;
	std::optional<T> arg_value;

	// Required Argument Constructor
	// Required arguments are never referenced by name, but they do have a value name used for printing the argument syntax only.
	Argument(required_argument_t, std::string val_str) :
		required(true),
		arg_name(std::string{ "" }),
		short_name(std::string{ "" }),
		val_str(val_str),
		arg_value(std::optional<T>(std::nullopt))
	{
		assert(!val_str.empty());
	};

	// Optional Flag Argument Constructor 
	Argument(optional_flag_argument_t, std::string arg_name, std::optional<bool> arg_value = std::make_optional<bool>(false), std::string short_name = std::string{ "" }) :
		required(false),
		arg_name(arg_name),
		short_name(std::string()),
		val_str(std::string()),
		arg_value(arg_value)
	{
		assert(!arg_name.empty());

		if (short_name.empty()) {
			gen_short_name();
		}
		else {
			this->short_name = short_name;
		}
		return;
	};

	// Optional Value Argument Constructor
	Argument(optional_value_argument_t, std::string arg_name, std::string val_str, std::optional<T> arg_value = std::optional<T>(std::nullopt), std::string short_name = std::string{ "" }) :
		required(false),
		arg_name(arg_name),
		short_name(std::string()),
		val_str(val_str),
		arg_value(arg_value)
	{
		assert(!arg_name.empty());
		assert(!val_str.empty());

		if (short_name.empty()) {
			gen_short_name();
		}
		else {
			this->short_name = short_name;
		}
		return;
	};

	// Methods
	inline bool is_arg_required() { return required; };
	inline bool is_arg_optional() { return !required; };
	inline bool is_arg_value_only() { return required; };
	inline bool is_arg_flag_only() { return val_str.empty(); };
	inline bool is_arg_named_value() { return !required && !val_str.empty(); };
	inline bool is_arg_named() { return !required; };
	inline bool is_arg_valued() { return !val_str.empty(); };
	inline bool has_value() { return arg_value.has_value(); };
	inline operator bool() { return arg_value.has_value(); };
	void reset() { this->arg_value.reset(); return; }

	inline std::ostream& print(std::ostream& out) {
		if (this->is_arg_required()) { // required args only need the argument name
			out << "<" << this->val_str << ">";
		}
		else if (this->is_arg_flag_only()) {
			out << "[{" << "--" << this->arg_name << " | " << "-" << this->short_name << "}]";
		}
		else if (this->is_arg_named_value()) {
			out << "[{" << "--" << this->arg_name << " | " << "-" << this->short_name << "} <" << this->val_str << ">]";
		}
		return out;
	};
	inline std::istream& parse(std::istream& in) {
		if (in.eof()) { return in; }
		T temp_value{};
		std::string input_token, name_token;
		if (this->is_arg_named()) { // if the argument is named
			if (!(in >> name_token)) { return in; };
			if (name_token != std::string("--").append(this->arg_name  ) &&
				name_token != std::string("-" ).append(this->short_name))
			{
				std::for_each(input_token.rbegin(), input_token.rend(), [&in](char c) {in.putback(c); }); // put all the characters back into the stream
				in.setstate(std::ios_base::failbit); // and indicate the parse failed
				return in;
			};
		}
		if (in.eof()) { // cannot have end of stream in the middle of an argument.
			in.clear();
			in.setstate(std::ios_base::badbit);
			return in;
		}
		if (!(in >> temp_value)) { // if the value cannot be parsed
			in.clear(); // clear the state flags
			in.setstate(std::ios_base::badbit); // and manually set the badbit to indicate the parsing cannot continue
			return in;
		}
		this->arg_value = temp_value;
		return in;
	};

	friend std::ostream& operator<<(std::ostream& out, Argument<T> arg) { return arg.print(out); };
	friend std::istream& operator>>(std::istream& in, Argument<T>& arg) { return arg.parse(in); };
};

class AbsCommand {
public:
	virtual std::ostream& print(std::ostream& out) = 0;
	virtual std::istream& parse(std::istream& in) = 0;

	friend std::ostream& operator<<(std::ostream& out, AbsCommand& cmd) { return cmd.print(out); };
	friend std::istream& operator>>(std::istream& in, AbsCommand& cmd) { return cmd.parse(in); };
};

template<typename Retval, typename... Args>
class Command : public AbsCommand {
private:
	std::string cmd_name, outputs;
	bool parsed;
	std::vector<AbsArgument*> args;
	std::function<Retval(Args...)> fn;

public:
	// Constructors
	Command(std::string name, std::vector<AbsArgument*> args, std::function<Retval(Args...)> fn, std::string outputs) {
		this->cmd_name = name;
		this->parsed = false;
		this->args = args;
		this->fn = fn;
		this->outputs = outputs;
	};

	Command(std::string name, std::function<Retval(Args...)> fn, std::string outputs) : Command(name, {}, fn, outputs) {};

	inline bool has_args() { return this->args.size() > 0; };
	inline bool has_no_args() { return this->args.size() == 0; };
	inline bool is_parsed() { return this->parsed; };
	inline bool is_unparsed() { return !this->parsed; };
	inline bool has_output() { return this->outputs.size() > 0; };
	inline bool has_no_output() { return this->outputs.size() == 0; };
	
	Retval execute(Args... args) {
		return fn(args...);
	};
	
	std::istream& parse(std::istream& in) {
		std::string token;
		if (!(in >> token)) { return in; };
		if (token != this->cmd_name) {
			std::for_each(token.rbegin(), token.rend(), [&in](char c) {in.putback(c); });
			in.setstate(std::ios_base::failbit);
			return in;
		};
		for (auto arg : this->args) {
			if (in.eof()) { break; };
			if (!(in >> *arg)) {
				if (in.bad()) { return in; };
				if (arg->is_arg_required()) { return in; };
				in.clear();
			}
		}
		return in;
	};
	std::ostream& print(std::ostream& out) {
		out << this->cmd_name;
		if (this->has_output()) {
			out << " -> ";
			out << this->outputs;
			out << std::endl;
		}
		for (auto arg : this->args) {
			out << "\t" << *arg << std::endl;
		}
		return out;
	};

	friend std::ostream& operator<<(std::ostream& out, Command cmd) { return cmd.print(out); };
	friend std::istream& operator>>(std::istream& in, Command& cmd) { return cmd.parse(in); };
};

