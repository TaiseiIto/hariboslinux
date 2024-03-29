// Backus Naur Form
//
// <formula>           ::= <term> | <formula> <comma> <formula>
// <term>              ::= <factor> | <plus> <factor> | <minus> <factor> | <term> <plus> <factor> | <term> <minus> <factor>
// <factor>            ::= <power> | <factor> <asterisk> <power> | <factor> <slash> <power> | <factor> <i>
// <power>             ::= <operand> | <power> <circumflex> <operand>
// <operand>           ::= <absolute> | <left_parenthesis> <formula> <right_parenthesis> | <e> | <i> | <pi> | <function> <left_parenthesis> <formula> <right_parenthesis>
// <function>          ::= <function_acos> | <function_asin> | <function_atan> | <function_cos> | <function_cosh> | <function_log> | <function_sin> | <function_sinh> | <function_sqrt> | <function_tan> | <function_tanh>
// <e>                 ::= <alphabets "e">
// <i>                 ::= <alphabets "i">
// <pi>                ::= <alphabets "pi">
// <function_acos>     ::= <alphabets "acos">
// <function_acosh>    ::= <alphabets "acosh">
// <function_asin>     ::= <alphabets "asin">
// <function_asinh>    ::= <alphabets "asinh">
// <function_atan>     ::= <alphabets "atan">
// <function_atanh>    ::= <alphabets "atanh">
// <function_cos>      ::= <alphabets "cos">
// <function_cosh>     ::= <alphabets "cosh">
// <function_log>      ::= <alphabets "log">
// <function_sin>      ::= <alphabets "sin">
// <function_sinh>     ::= <alphabets "sinh">
// <function_sqrt>     ::= <alphabets "sqrt">
// <function_tan>      ::= <alphabets "tan">
// <function_tanh>     ::= <alphabets "tanh">
// <absolute>          ::= <numbers> | <numbers> <dot> <numbers>
// <alphabets>         ::= <alphabet> | <alphabets> <alphabet>
// <numbers>           ::= <number> | <numbers> <number>
// <alphabet>          ::= 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z'
// <comma>             ::= ','
// <number>            ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
// <dot>               ::= '.'
// <plus>              ::= '+'
// <minus>             ::= '-'
// <asterisk>          ::= '*'
// <slash>             ::= '/'
// <circumflex>        ::= '^'
// <left_parenthesis>  ::= '('
// <right_parenthesis> ::= ')'

#include "chain_string.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct _Symbol;

typedef enum _SymbolType
{
	absolute,
	alphabet,
	alphabets,
	asterisk,
	circumflex,
	comma,
	dot,
	e,
	factor,
	formula,
	function,
	function_acos,
	function_acosh,
	function_asin,
	function_asinh,
	function_atan,
	function_atanh,
	function_cos,
	function_cosh,
	function_log,
	function_sin,
	function_sinh,
	function_sqrt,
	function_tan,
	function_tanh,
	i,
	left_parenthesis,
	minus,
	number,
	numbers,
	operand,
	pi,
	plus,
	power,
	right_parenthesis,
	slash,
	term,
	invalid_symbol_type
} SymbolType;

typedef struct _Substring
{
	char const *initial;
	size_t length;
} Substring;

typedef struct _Absolute
{
	struct _Symbol *integer;
	struct _Symbol *dot;
	struct _Symbol *decimal;
} Absolute;

typedef struct _Alphabets
{
	struct _Symbol *alphabets;
	struct _Symbol *alphabet;
} Alphabets;

typedef struct _E
{
	struct _Symbol *alphabets;
} E;

typedef struct _Factor
{
	struct _Symbol *factor;
	struct _Symbol *operator;
	struct _Symbol *power;
	struct _Symbol *i;
} Factor;

typedef struct _Formula
{
	struct _Symbol *term;
	struct _Symbol *comma;
	struct _Symbol *left_formula;
	struct _Symbol *right_formula;
} Formula;

typedef struct _Function
{
	struct _Symbol *function;
} Function;

typedef struct _FunctionAcos
{
	struct _Symbol *alphabets;
} FunctionAcos;

typedef struct _FunctionAcosh
{
	struct _Symbol *alphabets;
} FunctionAcosh;

typedef struct _FunctionAsin
{
	struct _Symbol *alphabets;
} FunctionAsin;

typedef struct _FunctionAsinh
{
	struct _Symbol *alphabets;
} FunctionAsinh;

typedef struct _FunctionAtan
{
	struct _Symbol *alphabets;
} FunctionAtan;

typedef struct _FunctionAtanh
{
	struct _Symbol *alphabets;
} FunctionAtanh;

typedef struct _FunctionCos
{
	struct _Symbol *alphabets;
} FunctionCos;

typedef struct _FunctionCosh
{
	struct _Symbol *alphabets;
} FunctionCosh;

typedef struct _FunctionLog
{
	struct _Symbol *alphabets;
} FunctionLog;

typedef struct _FunctionSin
{
	struct _Symbol *alphabets;
} FunctionSin;

typedef struct _FunctionSinh
{
	struct _Symbol *alphabets;
} FunctionSinh;

typedef struct _FunctionSqrt
{
	struct _Symbol *alphabets;
} FunctionSqrt;

typedef struct _FunctionTan
{
	struct _Symbol *alphabets;
} FunctionTan;

typedef struct _FunctionTanh
{
	struct _Symbol *alphabets;
} FunctionTanh;

typedef struct _I
{
	struct _Symbol *alphabets;
} I;

typedef struct _Numbers
{
	struct _Symbol *numbers;
	struct _Symbol *number;
	unsigned int level;
} Numbers;

typedef struct _Operand
{
	struct _Symbol *function;
	struct _Symbol *left_parenthesis;
	struct _Symbol *right_parenthesis;
	struct _Symbol *value;
} Operand;

typedef struct _Pi
{
	struct _Symbol *alphabets;
} Pi;

typedef struct _Power
{
	struct _Symbol *power;
	struct _Symbol *circumflex;
	struct _Symbol *operand;
} Power;

typedef struct _Term
{
	struct _Symbol *term;
	struct _Symbol *operator;
	struct _Symbol *factor;
} Term;

typedef union _Component
{
	Absolute absolute;
	Alphabets alphabets;
	E e;
	Factor factor;
	Formula formula;
	Function function;
	FunctionAcos function_acos;
	FunctionAcosh function_acosh;
	FunctionAsin function_asin;
	FunctionAsinh function_asinh;
	FunctionAtan function_atan;
	FunctionAtanh function_atanh;
	FunctionCos function_cos;
	FunctionCosh function_cosh;
	FunctionLog function_log;
	FunctionSin function_sin;
	FunctionSinh function_sinh;
	FunctionSqrt function_sqrt;
	FunctionTan function_tan;
	FunctionTanh function_tanh;
	I i;
	Numbers numbers;
	Operand operand;
	Pi pi;
	Power power;
	Term term;
} Component;

typedef struct
{
	double real;
	double imag;
} Complex;

typedef struct _Symbol
{
	Component component;
	Substring string;
	SymbolType type;
	Complex value;
	struct _Symbol *previous;
	struct _Symbol *next;
} Symbol;

typedef struct _Symbols
{
	Symbol *first_symbol;
	Symbol *last_symbol;
} Symbols;

char *combine_argv(int argc, char const * const * const argv);
Complex complex_abs(Complex a);
Complex complex_addition(Complex a, Complex b);
Complex complex_acos(Complex x);
Complex complex_acosh(Complex x);
Complex complex_asin(Complex x);
Complex complex_asinh(Complex x);
Complex complex_atan(Complex x);
Complex complex_atanh(Complex x);
Complex complex_cos(Complex theta);
Complex complex_cosh(Complex x);
Complex complex_division(Complex a, Complex b);
Complex complex_exp(Complex exponent);
Complex complex_log(Complex power);
Complex complex_multiplication(Complex a, Complex b);
Complex complex_pow(Complex base, Complex exponent);
Complex complex_sin(Complex theta);
Complex complex_sinh(Complex x);
Complex complex_subtraction(Complex a, Complex b);
Complex complex_sqrt(Complex x);
Complex complex_tan(Complex theta);
Complex complex_tanh(Complex x);
void delete_symbol(Symbol *symbol);
void delete_symbols(Symbols symbols);
Symbols lexical_analysis(char const * const input_string);
void print_complex(Complex value);
void print_symbols(Symbols const symbols);
SymbolType substring2symbol_type(Substring substring);
char *symbol_to_string(Symbol const *symbol);
char const *symbol_type_name(SymbolType symbol);
void semantic_analysis(Symbol* symbol);
ChainString *symbol_to_chain_string(Symbol const *symbol);
Symbols syntactic_analysis(Symbols symbols);

int main(int argc, char const * const * const argv)
{
	char *input_string = combine_argv(argc - 1, argv + 1);
	Symbols symbols = lexical_analysis(input_string);
	symbols = syntactic_analysis(symbols);
	if(!symbols.first_symbol)
	{
		ERROR();
		printf("NO SYMBOL ERROR\n");
		exit(-1);
	}
	if(symbols.first_symbol != symbols.last_symbol)
	{
		ERROR();
		printf("SYNTACTIC ANALYSIS ERROR\n");
		print_symbols(symbols);
		exit(-1);
	}
	semantic_analysis(symbols.first_symbol);
	#ifdef DEBUG
	printf("\n");
	print_symbols(symbols);
	printf("\n");
	#endif
	if(symbols.first_symbol != symbols.last_symbol || symbols.first_symbol->type != formula)
	{
		ERROR();
		printf("SEMANTIC ANALYSIS ERROR\n");
		print_symbols(symbols);
		exit(-1);
	}
	printf("%.10llf%+.10llfi\n", symbols.first_symbol->value.real, symbols.first_symbol->value.imag);
	delete_symbols(symbols);
	free(input_string);
	return 0;
}

char *combine_argv(int argc, char const * const * const argv)
{
	char *combined_string;
	char *writer;
	unsigned int sum_length = 0;
	for(int argi = 0; argi < argc; argi++)sum_length += strlen(argv[argi]);
	combined_string = malloc(sum_length + 1);
	writer = combined_string;
	for(int argi = 0; argi < argc; argi++)
	{
		strcpy(writer, argv[argi]);
		writer += strlen(argv[argi]);
	}
	return combined_string;
}

Complex complex_abs(Complex a)
{
	Complex result;
	result.real = sqrt(a.real * a.real + a.imag * a.imag);
	result.imag = 0.0;
	return result;
}

Complex complex_addition(Complex a, Complex b)
{
	Complex result;
	result.real = a.real + b.real;
	result.imag = a.imag + b.imag;
	return result;
}

Complex complex_acos(Complex x)
{
	Complex minus_i;
	Complex one;
	minus_i.real = 0.0;
	minus_i.imag = -1.0;
	one.real = 1.0;
	one.imag = 0.0;
	return complex_multiplication(minus_i, complex_log(complex_addition(x, complex_sqrt(complex_subtraction(complex_multiplication(x, x), one)))));
}

Complex complex_acosh(Complex x)
{
	Complex one;
	one.real = 1.0;
	one.imag = 0.0;
	return complex_log(complex_addition(x, complex_sqrt(complex_subtraction(complex_multiplication(x, x), one))));
}

Complex complex_asin(Complex x)
{
	Complex i;
	Complex minus_i;
	Complex one;
	i.real = 0.0;
	i.imag = 1.0;
	minus_i.real = 0.0;
	minus_i.imag = -1.0;
	one.real = 1.0;
	one.imag = 0.0;
	return complex_multiplication(minus_i, complex_log(complex_addition(complex_multiplication(x, i), complex_sqrt(complex_subtraction(one, complex_multiplication(x, x))))));
}

Complex complex_asinh(Complex x)
{
	Complex one;
	one.real = 1.0;
	one.imag = 0.0;
	return complex_log(complex_addition(x, complex_sqrt(complex_addition(complex_multiplication(x, x), one))));
}

Complex complex_atan(Complex x)
{
	Complex i;
	Complex half_i;
	i.real = 0.0;
	i.imag = 1.0;
	half_i.real = 0.0;
	half_i.imag = 0.5;
	return complex_multiplication(half_i, complex_log(complex_division(complex_addition(x, i), complex_subtraction(x, i))));
}

Complex complex_atanh(Complex x)
{
	Complex half;
	Complex one;
	half.real = 0.5;
	half.imag = 0.0;
	one.real = 1.0;
	one.imag = 0.0;
	return complex_multiplication(half, complex_log(complex_division(complex_addition(one, x), complex_subtraction(one, x))));
}

Complex complex_cos(Complex theta)
{
	Complex i;
	Complex minus_i;
	Complex two;
	i.real = 0.0;
	i.imag = 1.0;
	minus_i.real = 0.0;
	minus_i.imag = -1.0;
	two.real = 2.0;
	two.imag = 0.0;
	return complex_division(complex_addition(complex_exp(complex_multiplication(theta, i)), complex_exp(complex_multiplication(theta, minus_i))), two);
}

Complex complex_cosh(Complex x)
{
	Complex minus_one;
	Complex two;
	minus_one.real = -1.0;
	minus_one.imag = 0.0;
	two.real = 2.0;
	two.imag = 0.0;
	return complex_division(complex_addition(complex_exp(x), complex_exp(complex_multiplication(minus_one, x))), two);
}

Complex complex_division(Complex a, Complex b)
{
	Complex result;
	result.real = (a.real * b.real + a.imag * b.imag) / (b.real * b.real + b.imag * b.imag);
	result.imag = (a.imag * b.real - a.real * b.imag) / (b.real * b.real + b.imag * b.imag);
	return result;
}

Complex complex_exp(Complex exponent)
{
	Complex result;
	result.real = exp(exponent.real) * cos(exponent.imag);
	result.imag = exp(exponent.real) * sin(exponent.imag);
	return result;
}

Complex complex_log(Complex power)
{
	Complex result;
	result.real = log(complex_abs(power).real);
	result.imag = atan2(power.imag, power.real);
	return result;
}

Complex complex_multiplication(Complex a, Complex b)
{
	Complex result;
	result.real = a.real * b.real - a.imag * b.imag;
	result.imag = a.real * b.imag + a.imag * b.real;
	return result;
}

Complex complex_pow(Complex base, Complex exponent)
{
	return complex_exp(complex_multiplication(exponent, complex_log(base)));
}

Complex complex_sin(Complex theta)
{
	Complex i;
	Complex minus_i;
	Complex two_i;
	i.real = 0.0;
	i.imag = 1.0;
	minus_i.real = 0.0;
	minus_i.imag = -1.0;
	two_i.real = 0.0;
	two_i.imag = 2.0;
	return complex_division(complex_subtraction(complex_exp(complex_multiplication(theta, i)), complex_exp(complex_multiplication(theta, minus_i))), two_i);
}

Complex complex_sinh(Complex x)
{
	Complex minus_one;
	Complex two;
	minus_one.real = -1.0;
	minus_one.imag = 0.0;
	two.real = 2.0;
	two.imag = 0.0;
	return complex_division(complex_subtraction(complex_exp(x), complex_exp(complex_multiplication(minus_one, x))), two);
}

Complex complex_subtraction(Complex a, Complex b)
{
	Complex result;
	result.real = a.real - b.real;
	result.imag = a.imag - b.imag;
	return result;
}

Complex complex_sqrt(Complex x)
{
	Complex exponent;
	exponent.real = 0.5;
	exponent.imag = 0.0;
	return complex_pow(x, exponent);
}

Complex complex_tan(Complex theta)
{
	return complex_division(complex_sin(theta), complex_cos(theta));
}

Complex complex_tanh(Complex x)
{
	return complex_division(complex_sinh(x), complex_cosh(x));
}

void delete_symbol(Symbol *symbol)
{
	#ifdef DEBUG
	char *symbol_string = symbol_to_string(symbol);
	free(symbol_string);
	#endif
	switch(symbol->type)
	{
	case absolute:
		if(symbol->component.absolute.integer)delete_symbol(symbol->component.absolute.integer);
		if(symbol->component.absolute.dot)delete_symbol(symbol->component.absolute.dot);
		if(symbol->component.absolute.decimal)delete_symbol(symbol->component.absolute.decimal);
		break;
	case alphabet:
		break;
	case alphabets:
		if(symbol->component.alphabets.alphabets)delete_symbol(symbol->component.alphabets.alphabets);
		if(symbol->component.alphabets.alphabet)delete_symbol(symbol->component.alphabets.alphabet);
		break;
	case asterisk:
		break;
	case circumflex:
		break;
	case comma:
		break;
	case dot:
		break;
	case e:
		if(symbol->component.e.alphabets)delete_symbol(symbol->component.e.alphabets);
		break;
	case factor:
		if(symbol->component.factor.factor)delete_symbol(symbol->component.factor.factor);
		if(symbol->component.factor.operator)delete_symbol(symbol->component.factor.operator);
		if(symbol->component.factor.power)delete_symbol(symbol->component.factor.power);
		break;
	case formula:
		if(symbol->component.formula.term)delete_symbol(symbol->component.formula.term);
		if(symbol->component.formula.comma)delete_symbol(symbol->component.formula.comma);
		if(symbol->component.formula.left_formula)delete_symbol(symbol->component.formula.left_formula);
		if(symbol->component.formula.right_formula)delete_symbol(symbol->component.formula.right_formula);
		break;
	case function:
		if(symbol->component.function.function)delete_symbol(symbol->component.function.function);
		break;
	case function_acos:
		if(symbol->component.function_acos.alphabets)delete_symbol(symbol->component.function_acos.alphabets);
		break;
	case function_acosh:
		if(symbol->component.function_acosh.alphabets)delete_symbol(symbol->component.function_acosh.alphabets);
		break;
	case function_asin:
		if(symbol->component.function_asin.alphabets)delete_symbol(symbol->component.function_asin.alphabets);
		break;
	case function_asinh:
		if(symbol->component.function_asinh.alphabets)delete_symbol(symbol->component.function_asinh.alphabets);
		break;
	case function_atan:
		if(symbol->component.function_atan.alphabets)delete_symbol(symbol->component.function_atan.alphabets);
		break;
	case function_atanh:
		if(symbol->component.function_atanh.alphabets)delete_symbol(symbol->component.function_atanh.alphabets);
		break;
	case function_cos:
		if(symbol->component.function_cos.alphabets)delete_symbol(symbol->component.function_cos.alphabets);
		break;
	case function_cosh:
		if(symbol->component.function_cosh.alphabets)delete_symbol(symbol->component.function_cosh.alphabets);
		break;
	case function_log:
		if(symbol->component.function_log.alphabets)delete_symbol(symbol->component.function_log.alphabets);
		break;
	case function_sin:
		if(symbol->component.function_sin.alphabets)delete_symbol(symbol->component.function_sin.alphabets);
		break;
	case function_sinh:
		if(symbol->component.function_sinh.alphabets)delete_symbol(symbol->component.function_sinh.alphabets);
		break;
	case function_sqrt:
		if(symbol->component.function_sqrt.alphabets)delete_symbol(symbol->component.function_sqrt.alphabets);
		break;
	case function_tan:
		if(symbol->component.function_tan.alphabets)delete_symbol(symbol->component.function_tan.alphabets);
		break;
	case function_tanh:
		if(symbol->component.function_tanh.alphabets)delete_symbol(symbol->component.function_tanh.alphabets);
		break;
	case i:
		if(symbol->component.i.alphabets)delete_symbol(symbol->component.i.alphabets);
		break;
	case left_parenthesis:
		break;
	case minus:
		break;
	case number:
		break;
	case numbers:
		if(symbol->component.numbers.numbers)delete_symbol(symbol->component.numbers.numbers);
		if(symbol->component.numbers.number)delete_symbol(symbol->component.numbers.number);
		break;
	case operand:
		if(symbol->component.operand.function)delete_symbol(symbol->component.operand.function);
		if(symbol->component.operand.left_parenthesis)delete_symbol(symbol->component.operand.left_parenthesis);
		if(symbol->component.operand.right_parenthesis)delete_symbol(symbol->component.operand.right_parenthesis);
		if(symbol->component.operand.value)delete_symbol(symbol->component.operand.value);
		break;
	case pi:
		if(symbol->component.pi.alphabets)delete_symbol(symbol->component.pi.alphabets);
		break;
	case plus:
		break;
	case power:
		if(symbol->component.power.power)delete_symbol(symbol->component.power.power);
		if(symbol->component.power.circumflex)delete_symbol(symbol->component.power.circumflex);
		if(symbol->component.power.operand)delete_symbol(symbol->component.power.operand);
		break;
	case right_parenthesis:
		break;
	case slash:
		break;
	case term:
		if(symbol->component.term.term)delete_symbol(symbol->component.term.term);
		if(symbol->component.term.operator)delete_symbol(symbol->component.term.operator);
		if(symbol->component.term.factor)delete_symbol(symbol->component.term.factor);
		break;
	default:
		ERROR(); // Invalid symbol
		exit(-1);
		break;
	}
	free(symbol);
}

void delete_symbols(Symbols symbols)
{
	Symbol *next_symbol;
	for(Symbol *symbol = symbols.first_symbol; symbol; symbol = next_symbol)
	{
		next_symbol = symbol->next;
		delete_symbol(symbol);
	}
}

Symbols lexical_analysis(char const * const input_string)
{
	Symbols symbols;
	symbols.first_symbol = NULL;
	symbols.last_symbol = NULL;
	for(char const *reader = input_string; *reader; reader++)
	{
		Symbol *symbol = malloc(sizeof(*symbol));
		symbol->previous = symbols.last_symbol;
		symbol->next = NULL;
		symbol->string.initial = reader;
		symbol->string.length = 1;
		symbol->type = substring2symbol_type(symbol->string);
		if(!symbols.first_symbol)symbols.first_symbol = symbol;
		if(symbols.last_symbol)
		{
			symbols.last_symbol->next = symbol;
			symbols.last_symbol = symbol;
		}
		else symbols.last_symbol = symbol;
	}
	return symbols;
}

void print_symbols(Symbols const symbols)
{
	for(Symbol const *symbol = symbols.first_symbol; symbol; symbol = symbol->next)
	{
		char *symbol_string = symbol_to_string(symbol);
		printf("%s", symbol_string);
		free(symbol_string);
	}
}

SymbolType substring2symbol_type(Substring substring)
{
	switch(substring.length)
	{
	case 1:
		switch(*substring.initial)
		{
		case '(':
			return left_parenthesis;
		case ')':
			return right_parenthesis;
		case '+':
			return plus;
		case '-':
			return minus;
		case '*':
			return asterisk;
		case '/':
			return slash;
		case '^':
			return circumflex;
		case '.':
			return dot;
		case ',':
			return comma;
		default:
			if('0' <= *substring.initial && *substring.initial <= '9')return number;
			else if(('a' <= *substring.initial && *substring.initial <= 'z') || ('A' <= *substring.initial && *substring.initial <= 'Z'))return alphabet;
			else
			{
				ERROR(); // Invalid substring initial
				exit(-1);
				return invalid_symbol_type;
			}
		}
		break;
	default:
		ERROR(); // Invalid substring length
		exit(-1);
		return invalid_symbol_type;
	}
}

ChainString *symbol_to_chain_string(Symbol const *symbol)
{
	ChainString *output;
	ChainString *alphabet_chain_string;
	ChainString *alphabets_chain_string;
	ChainString *circumflex_chain_string;
	ChainString *comma_chain_string;
	ChainString *decimal_chain_string;
	ChainString *dot_chain_string;
	ChainString *factor_chain_string;
	ChainString *function_chain_string;
	ChainString *i_chain_string;
	ChainString *integer_chain_string;
	ChainString *left_formula_chain_string;
	ChainString *left_parenthesis_chain_string;
	ChainString *number_chain_string;
	ChainString *numbers_chain_string;
	ChainString *operand_chain_string;
	ChainString *operator_chain_string;
	ChainString *power_chain_string;
	ChainString *right_formula_chain_string;
	ChainString *right_parenthesis_chain_string;
	ChainString *term_chain_string;
	ChainString *value_chain_string;
	char *alphabet_char_array;
	char *alphabets_char_array;
	char *circumflex_char_array;
	char *comma_char_array;
	char *decimal_char_array;
	char *dot_char_array;
	char *factor_char_array;
	char *function_char_array;
	char *i_char_array;
	char *integer_char_array;
	char *left_formula_char_array;
	char *left_parenthesis_char_array;
	char *number_char_array;
	char *numbers_char_array;
	char *operand_char_array;
	char *operator_char_array;
	char *power_char_array;
	char *right_formula_char_array;
	char *right_parenthesis_char_array;
	char *term_char_array;
	char *value_char_array;
	if(!symbol)return create_chain_string("");
	switch(symbol->type)
	{
	case alphabet:
	case asterisk:
	case circumflex:
	case comma:
	case dot:
	case left_parenthesis:
	case minus:
	case number:
	case plus:
	case right_parenthesis:
	case slash:
		return create_format_chain_string("%s \"%0.*s\"\n", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial);
	case absolute:
		if(symbol->component.absolute.integer)
		{
			integer_chain_string = symbol_to_chain_string(symbol->component.absolute.integer);
			insert_char_front(integer_chain_string, integer_chain_string->first_character, ' ');
			replace_chain_string(integer_chain_string, "\n", "\n ");
			integer_char_array = create_char_array_from_chain_string(integer_chain_string);
		}
		else integer_char_array = "";
		if(symbol->component.absolute.dot)
		{
			dot_chain_string = symbol_to_chain_string(symbol->component.absolute.dot);
			insert_char_front(dot_chain_string, dot_chain_string->first_character, ' ');
			replace_chain_string(dot_chain_string, "\n", "\n ");
			dot_char_array = create_char_array_from_chain_string(dot_chain_string);
		}
		else dot_char_array = "";
		if(symbol->component.absolute.decimal)
		{
			decimal_chain_string = symbol_to_chain_string(symbol->component.absolute.decimal);
			insert_char_front(decimal_chain_string, decimal_chain_string->first_character, ' ');
			replace_chain_string(decimal_chain_string, "\n", "\n ");
			decimal_char_array = create_char_array_from_chain_string(decimal_chain_string);
		}
		else decimal_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.10llf%+.10llfi\n%s%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value.real, symbol->value.imag, integer_char_array, dot_char_array, decimal_char_array);
		if(symbol->component.absolute.integer)
		{
			delete_chain_string(integer_chain_string);
			free(integer_char_array);
		}
		if(symbol->component.absolute.dot)
		{
			delete_chain_string(dot_chain_string);
			free(dot_char_array);
		}
		if(symbol->component.absolute.decimal)
		{
			delete_chain_string(decimal_chain_string);
			free(decimal_char_array);
		}
		return output;
	case alphabets:
		if(symbol->component.alphabets.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.alphabets.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		if(symbol->component.alphabets.alphabet)
		{
			alphabet_chain_string = symbol_to_chain_string(symbol->component.alphabets.alphabet);
			insert_char_front(alphabet_chain_string, alphabet_chain_string->first_character, ' ');
			replace_chain_string(alphabet_chain_string, "\n", "\n ");
			alphabet_char_array = create_char_array_from_chain_string(alphabet_chain_string);
		}
		else alphabet_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array, alphabet_char_array);
		if(symbol->component.alphabets.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		if(symbol->component.alphabets.alphabet)
		{
			delete_chain_string(alphabet_chain_string);
			free(alphabet_char_array);
		}
		return output;
	case e:
		if(symbol->component.e.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.e.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.10llf%+.10llfi\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value.real, symbol->value.imag, alphabets_char_array);
		if(symbol->component.e.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case factor:
		if(symbol->component.factor.factor)
		{
			factor_chain_string = symbol_to_chain_string(symbol->component.factor.factor);
			insert_char_front(factor_chain_string, factor_chain_string->first_character, ' ');
			replace_chain_string(factor_chain_string, "\n", "\n ");
			factor_char_array = create_char_array_from_chain_string(factor_chain_string);
		}
		else factor_char_array = "";
		if(symbol->component.factor.operator)
		{
			operator_chain_string = symbol_to_chain_string(symbol->component.factor.operator);
			insert_char_front(operator_chain_string, operator_chain_string->first_character, ' ');
			replace_chain_string(operator_chain_string, "\n", "\n ");
			operator_char_array = create_char_array_from_chain_string(operator_chain_string);
		}
		else operator_char_array = "";
		if(symbol->component.factor.power)
		{
			power_chain_string = symbol_to_chain_string(symbol->component.factor.power);
			insert_char_front(power_chain_string, power_chain_string->first_character, ' ');
			replace_chain_string(power_chain_string, "\n", "\n ");
			power_char_array = create_char_array_from_chain_string(power_chain_string);
		}
		else power_char_array = "";
		if(symbol->component.factor.i)
		{
			i_chain_string = symbol_to_chain_string(symbol->component.factor.i);
			insert_char_front(i_chain_string, i_chain_string->first_character, ' ');
			replace_chain_string(i_chain_string, "\n", "\n ");
			i_char_array = create_char_array_from_chain_string(i_chain_string);
		}
		else i_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.10llf%+.10llfi\n%s%s%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value.real, symbol->value.imag, factor_char_array, operator_char_array, power_char_array, i_char_array);
		if(symbol->component.factor.factor)
		{
			delete_chain_string(factor_chain_string);
			free(factor_char_array);
		}
		if(symbol->component.factor.operator)
		{
			delete_chain_string(operator_chain_string);
			free(operator_char_array);
		}
		if(symbol->component.factor.power)
		{
			delete_chain_string(power_chain_string);
			free(power_char_array);
		}
		if(symbol->component.factor.i)
		{
			delete_chain_string(i_chain_string);
			free(i_char_array);
		}
		return output;
	case formula:
		if(symbol->component.formula.term)
		{
			term_chain_string = symbol_to_chain_string(symbol->component.formula.term);
			insert_char_front(term_chain_string, term_chain_string->first_character, ' ');
			replace_chain_string(term_chain_string, "\n", "\n ");
			term_char_array = create_char_array_from_chain_string(term_chain_string);
		}
		else term_char_array = "";
		if(symbol->component.formula.comma)
		{
			comma_chain_string = symbol_to_chain_string(symbol->component.formula.comma);
			insert_char_front(comma_chain_string, comma_chain_string->first_character, ' ');
			replace_chain_string(comma_chain_string, "\n", "\n ");
			comma_char_array = create_char_array_from_chain_string(comma_chain_string);
		}
		else comma_char_array = "";
		if(symbol->component.formula.left_formula)
		{
			left_formula_chain_string = symbol_to_chain_string(symbol->component.formula.left_formula);
			insert_char_front(left_formula_chain_string, left_formula_chain_string->first_character, ' ');
			replace_chain_string(left_formula_chain_string, "\n", "\n ");
			left_formula_char_array = create_char_array_from_chain_string(left_formula_chain_string);
		}
		else left_formula_char_array = "";
		if(symbol->component.formula.right_formula)
		{
			right_formula_chain_string = symbol_to_chain_string(symbol->component.formula.right_formula);
			insert_char_front(right_formula_chain_string, right_formula_chain_string->first_character, ' ');
			replace_chain_string(right_formula_chain_string, "\n", "\n ");
			right_formula_char_array = create_char_array_from_chain_string(right_formula_chain_string);
		}
		else right_formula_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.10llf%+.10llfi\n%s%s%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value.real, symbol->value.imag, term_char_array, left_formula_char_array, comma_char_array, right_formula_char_array);
		if(symbol->component.formula.term)
		{
			delete_chain_string(term_chain_string);
			free(term_char_array);
		}
		if(symbol->component.formula.comma)
		{
			delete_chain_string(comma_chain_string);
			free(comma_char_array);
		}
		if(symbol->component.formula.left_formula)
		{
			delete_chain_string(left_formula_chain_string);
			free(left_formula_char_array);
		}
		if(symbol->component.formula.right_formula)
		{
			delete_chain_string(right_formula_chain_string);
			free(right_formula_char_array);
		}
		return output;
	case function:
		if(symbol->component.function.function)
		{
			function_chain_string = symbol_to_chain_string(symbol->component.function.function);
			insert_char_front(function_chain_string, function_chain_string->first_character, ' ');
			replace_chain_string(function_chain_string, "\n", "\n ");
			function_char_array = create_char_array_from_chain_string(function_chain_string);
		}
		else function_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, function_char_array);
		if(symbol->component.function.function)
		{
			delete_chain_string(function_chain_string);
			free(function_char_array);
		}
		return output;
	case function_acos:
		if(symbol->component.function_acos.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_acos.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_acos.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case function_acosh:
		if(symbol->component.function_acosh.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_acosh.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_acosh.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case function_asin:
		if(symbol->component.function_asin.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_asin.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_asin.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case function_asinh:
		if(symbol->component.function_asinh.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_asinh.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_asinh.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case function_atan:
		if(symbol->component.function_atan.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_atan.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_atan.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case function_atanh:
		if(symbol->component.function_atanh.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_atanh.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_atanh.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case function_cos:
		if(symbol->component.function_cos.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_cos.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_cos.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case function_cosh:
		if(symbol->component.function_cosh.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_cosh.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_cosh.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case function_log:
		if(symbol->component.function_log.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_log.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_log.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case function_sin:
		if(symbol->component.function_sin.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_sin.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_sin.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case function_sinh:
		if(symbol->component.function_sinh.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_sinh.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_sinh.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case function_sqrt:
		if(symbol->component.function_sqrt.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_sqrt.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_sqrt.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case function_tan:
		if(symbol->component.function_tan.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_tan.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_tan.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case function_tanh:
		if(symbol->component.function_tanh.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.function_tanh.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\"\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, alphabets_char_array);
		if(symbol->component.function_tanh.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case i:
		if(symbol->component.i.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.i.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.10llf%+.10llfi\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value.real, symbol->value.imag, alphabets_char_array);
		if(symbol->component.i.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case numbers:
		if(symbol->component.numbers.numbers)
		{
			numbers_chain_string = symbol_to_chain_string(symbol->component.numbers.numbers);
			insert_char_front(numbers_chain_string, numbers_chain_string->first_character, ' ');
			replace_chain_string(numbers_chain_string, "\n", "\n ");
			numbers_char_array = create_char_array_from_chain_string(numbers_chain_string);
		}
		else numbers_char_array = "";
		if(symbol->component.numbers.number)
		{
			number_chain_string = symbol_to_chain_string(symbol->component.numbers.number);
			insert_char_front(number_chain_string, number_chain_string->first_character, ' ');
			replace_chain_string(number_chain_string, "\n", "\n ");
			number_char_array = create_char_array_from_chain_string(number_chain_string);
		}
		else number_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.10llf%+.10llfi\n%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value.real, symbol->value.imag, numbers_char_array, number_char_array);
		if(symbol->component.numbers.numbers)
		{
			delete_chain_string(numbers_chain_string);
			free(numbers_char_array);
		}
		if(symbol->component.numbers.number)
		{
			delete_chain_string(number_chain_string);
			free(number_char_array);
		}
		return output;
	case operand:
		if(symbol->component.operand.function)
		{
			function_chain_string = symbol_to_chain_string(symbol->component.operand.function);
			insert_char_front(function_chain_string, function_chain_string->first_character, ' ');
			replace_chain_string(function_chain_string, "\n", "\n ");
			function_char_array = create_char_array_from_chain_string(function_chain_string);
		}
		else function_char_array = "";
		if(symbol->component.operand.left_parenthesis)
		{
			left_parenthesis_chain_string = symbol_to_chain_string(symbol->component.operand.left_parenthesis);
			insert_char_front(left_parenthesis_chain_string, left_parenthesis_chain_string->first_character, ' ');
			replace_chain_string(left_parenthesis_chain_string, "\n", "\n ");
			left_parenthesis_char_array = create_char_array_from_chain_string(left_parenthesis_chain_string);
		}
		else left_parenthesis_char_array = "";
		if(symbol->component.operand.right_parenthesis)
		{
			right_parenthesis_chain_string = symbol_to_chain_string(symbol->component.operand.right_parenthesis);
			insert_char_front(right_parenthesis_chain_string, right_parenthesis_chain_string->first_character, ' ');
			replace_chain_string(right_parenthesis_chain_string, "\n", "\n ");
			right_parenthesis_char_array = create_char_array_from_chain_string(right_parenthesis_chain_string);
		}
		else right_parenthesis_char_array = "";
		if(symbol->component.operand.value)
		{
			value_chain_string = symbol_to_chain_string(symbol->component.operand.value);
			insert_char_front(value_chain_string, value_chain_string->first_character, ' ');
			replace_chain_string(value_chain_string, "\n", "\n ");
			value_char_array = create_char_array_from_chain_string(value_chain_string);
		}
		else value_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.10llf%+.10llfi\n%s%s%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value.real, symbol->value.imag, function_char_array, left_parenthesis_char_array, value_char_array, right_parenthesis_char_array);
		if(symbol->component.operand.function)
		{
			delete_chain_string(function_chain_string);
			free(function_char_array);
		}
		if(symbol->component.operand.left_parenthesis)
		{
			delete_chain_string(left_parenthesis_chain_string);
			free(left_parenthesis_char_array);
		}
		if(symbol->component.operand.right_parenthesis)
		{
			delete_chain_string(right_parenthesis_chain_string);
			free(right_parenthesis_char_array);
		}
		if(symbol->component.operand.value)
		{
			delete_chain_string(value_chain_string);
			free(value_char_array);
		}
		return output;
	case pi:
		if(symbol->component.pi.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.pi.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.10llf%+.10llfi\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value.real, symbol->value.imag, alphabets_char_array);
		if(symbol->component.pi.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case power:
		if(symbol->component.power.power)
		{
			power_chain_string = symbol_to_chain_string(symbol->component.power.power);
			insert_char_front(power_chain_string, power_chain_string->first_character, ' ');
			replace_chain_string(power_chain_string, "\n", "\n ");
			power_char_array = create_char_array_from_chain_string(power_chain_string);
		}
		else power_char_array = "";
		if(symbol->component.power.circumflex)
		{
			circumflex_chain_string = symbol_to_chain_string(symbol->component.power.circumflex);
			insert_char_front(circumflex_chain_string, circumflex_chain_string->first_character, ' ');
			replace_chain_string(circumflex_chain_string, "\n", "\n ");
			circumflex_char_array = create_char_array_from_chain_string(circumflex_chain_string);
		}
		else circumflex_char_array = "";
		if(symbol->component.power.operand)
		{
			operand_chain_string = symbol_to_chain_string(symbol->component.power.operand);
			insert_char_front(operand_chain_string, operand_chain_string->first_character, ' ');
			replace_chain_string(operand_chain_string, "\n", "\n ");
			operand_char_array = create_char_array_from_chain_string(operand_chain_string);
		}
		else operand_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.10llf%+.10llfi\n%s%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value.real, symbol->value.imag, power_char_array, circumflex_char_array, operand_char_array);
		if(symbol->component.power.power)
		{
			delete_chain_string(power_chain_string);
			free(power_char_array);
		}
		if(symbol->component.power.circumflex)
		{
			delete_chain_string(circumflex_chain_string);
			free(circumflex_char_array);
		}
		if(symbol->component.power.operand)
		{
			delete_chain_string(operand_chain_string);
			free(operand_char_array);
		}
		return output;
	case term:
		if(symbol->component.term.term)
		{
			term_chain_string = symbol_to_chain_string(symbol->component.term.term);
			insert_char_front(term_chain_string, term_chain_string->first_character, ' ');
			replace_chain_string(term_chain_string, "\n", "\n ");
			term_char_array = create_char_array_from_chain_string(term_chain_string);
		}
		else term_char_array = "";
		if(symbol->component.term.operator)
		{
			operator_chain_string = symbol_to_chain_string(symbol->component.term.operator);
			insert_char_front(operator_chain_string, operator_chain_string->first_character, ' ');
			replace_chain_string(operator_chain_string, "\n", "\n ");
			operator_char_array = create_char_array_from_chain_string(operator_chain_string);
		}
		else operator_char_array = "";
		if(symbol->component.term.factor)
		{
			factor_chain_string = symbol_to_chain_string(symbol->component.term.factor);
			insert_char_front(factor_chain_string, factor_chain_string->first_character, ' ');
			replace_chain_string(factor_chain_string, "\n", "\n ");
			factor_char_array = create_char_array_from_chain_string(factor_chain_string);
		}
		else factor_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.10llf%+.10llfi\n%s%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value.real, symbol->value.imag, term_char_array, operator_char_array, factor_char_array);
		if(symbol->component.term.term)
		{
			delete_chain_string(term_chain_string);
			free(term_char_array);
		}
		if(symbol->component.term.operator)
		{
			delete_chain_string(operator_chain_string);
			free(operator_char_array);
		}
		if(symbol->component.term.factor)
		{
			delete_chain_string(factor_chain_string);
			free(factor_char_array);
		}
		return output;
	default:
		ERROR(); // Invalid symbol
		exit(-1);
		return NULL;
	}
}

char *symbol_to_string(Symbol const *symbol)
{
	ChainString *chain_string = symbol_to_chain_string(symbol);
	char *string = create_char_array_from_chain_string(chain_string);
	delete_chain_string(chain_string);
	return string;
}

char const *symbol_type_name(SymbolType symbol_type)
{
	static char const * const absolute_name = "absolute";
	static char const * const alphabet_name = "alphabet";
	static char const * const alphabets_name = "alphabets";
	static char const * const asterisk_name = "asterisk";
	static char const * const circumflex_name = "circumflex";
	static char const * const comma_name = "comma";
	static char const * const dot_name = "dot";
	static char const * const e_name = "e";
	static char const * const factor_name = "factor";
	static char const * const formula_name = "formula";
	static char const * const function_name = "function";
	static char const * const function_acos_name = "function_acos";
	static char const * const function_acosh_name = "function_acosh";
	static char const * const function_asin_name = "function_asin";
	static char const * const function_asinh_name = "function_asinh";
	static char const * const function_atan_name = "function_atan";
	static char const * const function_atanh_name = "function_atanh";
	static char const * const function_cos_name = "function_cos";
	static char const * const function_cosh_name = "function_cosh";
	static char const * const function_log_name = "function_log";
	static char const * const function_sin_name = "function_sin";
	static char const * const function_sinh_name = "function_sinh";
	static char const * const function_sqrt_name = "function_sqrt";
	static char const * const function_tan_name = "function_tan";
	static char const * const function_tanh_name = "function_tanh";
	static char const * const i_name = "i";
	static char const * const left_parenthesis_name = "left parenthesis";
	static char const * const minus_name = "minus";
	static char const * const number_name = "number";
	static char const * const numbers_name = "numbers";
	static char const * const operand_name = "operand";
	static char const * const pi_name = "pi";
	static char const * const plus_name = "plus";
	static char const * const power_name = "power";
	static char const * const right_parenthesis_name = "right parenthesis";
	static char const * const slash_name = "slash";
	static char const * const term_name = "term";
	switch(symbol_type)
	{
	case absolute:
		return absolute_name;
	case alphabet:
		return alphabet_name;
	case alphabets:
		return alphabets_name;
	case asterisk:
		return asterisk_name;
	case circumflex:
		return circumflex_name;
	case comma:
		return comma_name;
	case dot:
		return dot_name;
	case e:
		return e_name;
	case factor:
		return factor_name;
	case formula:
		return formula_name;
	case function:
		return function_name;
	case function_acos:
		return function_acos_name;
	case function_acosh:
		return function_acosh_name;
	case function_asin:
		return function_asin_name;
	case function_asinh:
		return function_asinh_name;
	case function_atan:
		return function_atan_name;
	case function_atanh:
		return function_atanh_name;
	case function_cos:
		return function_cos_name;
	case function_cosh:
		return function_cosh_name;
	case function_log:
		return function_log_name;
	case function_sin:
		return function_sin_name;
	case function_sinh:
		return function_sinh_name;
	case function_sqrt:
		return function_sqrt_name;
	case function_tan:
		return function_tan_name;
	case function_tanh:
		return function_tanh_name;
	case i:
		return i_name;
	case left_parenthesis:
		return left_parenthesis_name;
	case minus:
		return minus_name;
	case number:
		return number_name;
	case numbers:
		return numbers_name;
	case operand:
		return operand_name;
	case pi:
		return pi_name;
	case plus:
		return plus_name;
	case power:
		return power_name;
	case right_parenthesis:
		return right_parenthesis_name;
	case slash:
		return slash_name;
	case term:
		return term_name;
	default:
		ERROR(); // Invalid symbol
		exit(-1);
		return NULL;
	}
}

void semantic_analysis(Symbol* symbol)
{
	if(!symbol)
	{
		ERROR(); // no symbol error
		exit(-1);
	}
	switch(symbol->type)
	{
	case absolute:
		if(symbol->component.absolute.integer)semantic_analysis(symbol->component.absolute.integer);
		if(symbol->component.absolute.dot)semantic_analysis(symbol->component.absolute.dot);
		if(symbol->component.absolute.decimal)semantic_analysis(symbol->component.absolute.decimal);
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		if(symbol->component.absolute.decimal)
		{
			double divisor = 1.0;
			unsigned int divisor_level;
			for(divisor_level = 0; divisor_level <= symbol->component.absolute.decimal->component.numbers.level; divisor_level++)divisor *= 10.0;
			symbol->value.real = symbol->component.absolute.decimal->value.real / divisor;
		}
		if(symbol->component.absolute.integer)symbol->value.real += symbol->component.absolute.integer->value.real;
		break;
	case alphabet:
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		break;
	case alphabets:
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		break;
	case asterisk:
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		break;
	case circumflex:
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		break;
	case comma:
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		break;
	case dot:
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		break;
	case e:
		symbol->value.real = M_E;
		symbol->value.imag = 0.0;
		break;
	case factor:
		if(symbol->component.factor.factor)semantic_analysis(symbol->component.factor.factor);
		if(symbol->component.factor.operator)semantic_analysis(symbol->component.factor.operator);
		if(symbol->component.factor.power)semantic_analysis(symbol->component.factor.power);
		if(symbol->component.factor.i)semantic_analysis(symbol->component.factor.i);
		if(symbol->component.factor.operator && symbol->component.factor.factor)switch(symbol->component.factor.operator->type)
		{
		case asterisk:
			symbol->value = complex_multiplication(symbol->component.factor.factor->value, symbol->component.factor.power->value);
			break;
		case slash:
			symbol->value = complex_division(symbol->component.factor.factor->value, symbol->component.factor.power->value);
			break;
		default:
			ERROR(); // Invalid symbol
			exit(-1);
			break;
		}
		else if(symbol->component.factor.factor && symbol->component.factor.i)symbol->value = complex_multiplication(symbol->component.factor.factor->value, symbol->component.factor.i->value);
		else symbol->value = symbol->component.factor.power->value;
		break;
	case formula:
		if(symbol->component.formula.term)semantic_analysis(symbol->component.formula.term);
		if(symbol->component.formula.comma)semantic_analysis(symbol->component.formula.comma);
		if(symbol->component.formula.left_formula)semantic_analysis(symbol->component.formula.left_formula);
		if(symbol->component.formula.right_formula)semantic_analysis(symbol->component.formula.right_formula);
		if(symbol->component.formula.term)symbol->value = symbol->component.formula.term->value;
		else
		{
			symbol->value.real = 0.0 / 0.0;
			symbol->value.imag = 0.0 / 0.0;
		}
		break;
	case function:
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		break;
	case i:
		symbol->value.real = 0.0;
		symbol->value.imag = 1.0;
		break;
	case left_parenthesis:
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		break;
	case minus:
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		break;
	case number:
		symbol->value.real = (double)((int)(*symbol->string.initial - '0'));
		symbol->value.imag = 0.0;
		break;
	case numbers:
		if(symbol->component.numbers.numbers)semantic_analysis(symbol->component.numbers.numbers);
		if(symbol->component.numbers.number)semantic_analysis(symbol->component.numbers.number);
		symbol->value.real = symbol->component.numbers.numbers ? 10.0 * symbol->component.numbers.numbers->value.real : 0.0;
		symbol->value.real += symbol->component.numbers.number->value.real;
		symbol->value.imag = 0.0;
		break;
	case operand:
		if(symbol->component.operand.function)semantic_analysis(symbol->component.operand.function);
		if(symbol->component.operand.value)semantic_analysis(symbol->component.operand.value);
		if(symbol->component.operand.left_parenthesis)semantic_analysis(symbol->component.operand.left_parenthesis);
		if(symbol->component.operand.right_parenthesis)semantic_analysis(symbol->component.operand.right_parenthesis);
		if(symbol->component.operand.value)
		{
			if(symbol->component.operand.function && symbol->component.operand.function->type == function && symbol->component.operand.function->component.function.function)switch(symbol->component.operand.function->component.function.function->type)
			{
			case function_acos:
				symbol->value = complex_acos(symbol->component.operand.value->value);
				break;
			case function_acosh:
				symbol->value = complex_acosh(symbol->component.operand.value->value);
				break;
			case function_asin:
				symbol->value = complex_asin(symbol->component.operand.value->value);
				break;
			case function_asinh:
				symbol->value = complex_asinh(symbol->component.operand.value->value);
				break;
			case function_atan:
				symbol->value = complex_atan(symbol->component.operand.value->value);
				break;
			case function_atanh:
				symbol->value = complex_atanh(symbol->component.operand.value->value);
				break;
			case function_cos:
				symbol->value = complex_cos(symbol->component.operand.value->value);
				break;
			case function_cosh:
				symbol->value = complex_cosh(symbol->component.operand.value->value);
				break;
			case function_log:
				if(symbol->component.operand.value->type == formula && symbol->component.operand.value->component.formula.left_formula && symbol->component.operand.value->component.formula.right_formula)symbol->value = complex_division(complex_log(symbol->component.operand.value->component.formula.right_formula->value), complex_log(symbol->component.operand.value->component.formula.left_formula->value));
				else
				{
					ERROR(); // left_formula or right_formula is not found.
					exit(-1);
					break;
				}
				break;
			case function_sin:
				symbol->value = complex_sin(symbol->component.operand.value->value);
				break;
			case function_sinh:
				symbol->value = complex_sinh(symbol->component.operand.value->value);
				break;
			case function_sqrt:
				symbol->value = complex_sqrt(symbol->component.operand.value->value);
				break;
			case function_tan:
				symbol->value = complex_tan(symbol->component.operand.value->value);
				break;
			case function_tanh:
				symbol->value = complex_tanh(symbol->component.operand.value->value);
				break;
			default:
				ERROR(); // Invalid function type.
				exit(-1);
				break;
			}
			else symbol->value = symbol->component.operand.value->value;
		}
		else
		{
			ERROR(); // value is not found.
			exit(-1);
			break;
		}
		break;
	case pi:
		symbol->value.real = M_PI;
		symbol->value.imag = 0.0;
		break;
	case plus:
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		break;
	case power:
		if(symbol->component.power.power)semantic_analysis(symbol->component.power.power);
		if(symbol->component.power.circumflex)semantic_analysis(symbol->component.power.circumflex);
		if(symbol->component.power.operand)semantic_analysis(symbol->component.power.operand);
		if(symbol->component.power.operand)
		{
			if(symbol->component.power.power && symbol->component.power.circumflex)symbol->value = complex_pow(symbol->component.power.power->value, symbol->component.power.operand->value);
			else symbol->value = symbol->component.power.operand->value;
		}
		else
		{
			ERROR(); // Operand is not found.
			exit(-1);
			break;
		}
		break;
	case right_parenthesis:
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		break;
	case slash:
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		break;
	case term:
		if(symbol->component.term.term)semantic_analysis(symbol->component.term.term);
		if(symbol->component.term.operator)semantic_analysis(symbol->component.term.operator);
		if(symbol->component.term.factor)semantic_analysis(symbol->component.term.factor);
		if(symbol->component.term.term && symbol->component.term.operator)switch(symbol->component.term.operator->type)
		{
		case plus:
			symbol->value = complex_addition(symbol->component.term.term->value, symbol->component.term.factor->value);
			break;
		case minus:
			symbol->value = complex_subtraction(symbol->component.term.term->value, symbol->component.term.factor->value);
			break;
		default:
			ERROR(); // Invalid symbol
			exit(-1);
			break;
		}
		else if(symbol->component.term.operator)switch(symbol->component.term.operator->type)
		{
		case plus:
			symbol->value = symbol->component.term.factor->value;
			break;
		case minus:
			symbol->value.real = 0.0 - symbol->component.term.factor->value.real;
			symbol->value.imag = 0.0 - symbol->component.term.factor->value.imag;
			break;
		default:
			ERROR(); // Invalid symbol
			exit(-1);
			break;
		}
		else symbol->value = symbol->component.term.factor->value;
		break;
	case invalid_symbol_type:
		symbol->value.real = 0.0;
		symbol->value.imag = 0.0;
		break;
	default:
		ERROR(); // Invalid symbol
		exit(-1);
		break;
	}
}

Symbols syntactic_analysis(Symbols symbols)
{
	Symbol *new_symbol;
	Symbol *next_symbol;
	unsigned char flags = 0;
	#define SYNTACTIC_ANALYSIS_FLAG_CHANGED	0x01
	for(Symbol *symbol = symbols.first_symbol; symbol; symbol = next_symbol)
	{
		next_symbol = symbol->next;
		switch(symbol->type)
		{
		case absolute:
			// <operand> ::= <absolute>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = operand;
			new_symbol->component.operand.function = NULL;
			new_symbol->component.operand.value = symbol;
			new_symbol->component.operand.left_parenthesis = NULL;
			new_symbol->component.operand.right_parenthesis = NULL;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<operand> ::= <absolute>\n");
			print_symbols(symbols);
			#endif
			break;
		case alphabet:
			// <alphabets> ::= <alphabet>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = alphabets;
			new_symbol->component.alphabets.alphabets = NULL;
			new_symbol->component.alphabets.alphabet = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<alphabets> ::= <alphabet>\n");
			print_symbols(symbols);
			#endif
			break;
		case alphabets:
			if(symbol->next && symbol->next->type == alphabet)
			{
				// <alphabets> ::= <alphabets> <alphabet>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = alphabets;
				new_symbol->component.alphabets.alphabets = symbol;
				new_symbol->component.alphabets.alphabet = symbol->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next)symbols.last_symbol = new_symbol;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<alphabets> ::= <alphabets> <alphabet>\n");
				print_symbols(symbols);
				#endif
			}
			else
			{
				char *word = create_format_char_array("%0.*s", symbol->string.length, symbol->string.initial);
				if(!strcmp(word, "acos"))
				{
					// <function_acos> ::= <alphabets "acos">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_acos;
					new_symbol->component.function_acos.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_acos> ::= <alphabets \"acos\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "acosh"))
				{
					// <function_acosh> ::= <alphabets "acosh">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_acosh;
					new_symbol->component.function_acosh.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_acosh> ::= <alphabets \"acosh\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "asin"))
				{
					// <function_asin> ::= <alphabets "asin">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_asin;
					new_symbol->component.function_asin.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_asin> ::= <alphabets \"asin\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "asinh"))
				{
					// <function_asinh> ::= <alphabets "asinh">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_asinh;
					new_symbol->component.function_asinh.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_asinh> ::= <alphabets \"asinh\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "atan"))
				{
					// <function_atan> ::= <alphabets "atan">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_atan;
					new_symbol->component.function_atan.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_atan> ::= <alphabets \"atan\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "atanh"))
				{
					// <function_atanh> ::= <alphabets "atanh">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_atanh;
					new_symbol->component.function_atanh.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_atanh> ::= <alphabets \"atanh\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "cos"))
				{
					// <function_cos> ::= <alphabets "cos">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_cos;
					new_symbol->component.function_cos.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_cos> ::= <alphabets \"cos\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "cosh"))
				{
					// <function_cosh> ::= <alphabets "cosh">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_cosh;
					new_symbol->component.function_cosh.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_cosh> ::= <alphabets \"cosh\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "e"))
				{
					// <e> ::= <alphabets "e">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = e;
					new_symbol->component.e.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<e> ::= <alphabets \"e\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "i"))
				{
					// <i> ::= <alphabets "i">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = i;
					new_symbol->component.i.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<i> ::= <alphabets \"i\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "pi"))
				{
					// <pi> ::= <alphabets "pi">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = pi;
					new_symbol->component.pi.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<pi> ::= <alphabets \"pi\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "log"))
				{
					// <function_log> ::= <alphabets "log">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_log;
					new_symbol->component.function_log.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_log> ::= <alphabets \"log\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "sin"))
				{
					// <function_sin> ::= <alphabets "sin">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_sin;
					new_symbol->component.function_sin.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_sin> ::= <alphabets \"sin\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "sinh"))
				{
					// <function_sinh> ::= <alphabets "sinh">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_sinh;
					new_symbol->component.function_sinh.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_sinh> ::= <alphabets \"sinh\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "sqrt"))
				{
					// <function_sqrt> ::= <alphabets "sqrt">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_sqrt;
					new_symbol->component.function_sqrt.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_sqrt> ::= <alphabets \"sqrt\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "tan"))
				{
					// <function_tan> ::= <alphabets "tan">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_tan;
					new_symbol->component.function_tan.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_tan> ::= <alphabets \"tan\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "tanh"))
				{
					// <function_tanh> ::= <alphabets "tanh">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = function_tanh;
					new_symbol->component.function_tanh.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<function_tanh> ::= <alphabets \"tanh\">\n");
					print_symbols(symbols);
					#endif
				}
				free(word);
			}
			break;
		case asterisk:
			break;
		case circumflex:
			break;
		case comma:
			break;
		case dot:
			break;
		case e:
			// <operand> ::= <e>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = operand;
			new_symbol->component.operand.function = NULL;
			new_symbol->component.operand.value = symbol;
			new_symbol->component.operand.left_parenthesis = NULL;
			new_symbol->component.operand.right_parenthesis = NULL;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<operand> ::= <e>\n");
			print_symbols(symbols);
			#endif
			break;
		case factor:
			if(symbol->next && (symbol->next->type == asterisk || symbol->next->type == slash) && symbol->next->next && symbol->next->next->type == power)
			{
				if(symbol->next->next->next && symbol->next->next->next->type == circumflex)break;
				// <factor> ::= <factor> <asterisk> <power> | <factor> <slash> <power>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = factor;
				new_symbol->component.factor.factor = symbol;
				new_symbol->component.factor.operator = symbol->next;
				new_symbol->component.factor.power = symbol->next->next;
				new_symbol->component.factor.i = NULL;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length + symbol->next->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next->next)symbols.last_symbol = new_symbol;
				symbol->next->next->previous = NULL;
				symbol->next->next->next = NULL;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<factor> ::= <factor> <asterisk> <power> | <factor> <slash> <power>\n");
				print_symbols(symbols);
				#endif
			}
			else if(symbol->next && symbol->next->type == i)
			{
				// <factor> ::= <factor> <i>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = factor;
				new_symbol->component.factor.factor = symbol;
				new_symbol->component.factor.operator = NULL;
				new_symbol->component.factor.power = NULL;
				new_symbol->component.factor.i = symbol->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next)symbols.last_symbol = new_symbol;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<factor> ::= <factor> <i>\n");
				print_symbols(symbols);
				#endif
			}
			else if(symbol->next && symbol->next->type == alphabet)break;
			else if(symbol->next && symbol->next->type == asterisk)break;
			else if(symbol->next && symbol->next->type == slash)break;
			else if(symbol->previous && symbol->previous->type == plus)break;
			else if(symbol->previous && symbol->previous->type == minus)break;
			else
			{
				// <term> ::= <factor>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = term;
				new_symbol->component.term.term = NULL;
				new_symbol->component.term.operator = NULL;
				new_symbol->component.term.factor = symbol;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<term> ::= <factor>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case formula:
			if(symbol->next && symbol->next->type == comma && symbol->next->next && symbol->next->next->type == formula)
			{
				// <formula> ::= <formula> <comma> <formula>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = formula;
				new_symbol->component.formula.term = NULL;
				new_symbol->component.formula.left_formula = symbol;
				new_symbol->component.formula.comma = symbol->next;
				new_symbol->component.formula.right_formula = symbol->next->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length + symbol->next->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next->next)symbols.last_symbol = new_symbol;
				symbol->next->next->previous = NULL;
				symbol->next->next->next = NULL;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<formula> ::= <formula> <comma> <formula>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case function:
			if(symbol->next && symbol->next->type == left_parenthesis && symbol->next->next && symbol->next->next->type == formula && symbol->next->next->next && symbol->next->next->next->type == right_parenthesis)
			{
				// <operand> ::= <function> <left_parenthesis> <formula> <right_parenthesis>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = operand;
				new_symbol->component.operand.function = symbol;
				new_symbol->component.operand.left_parenthesis = symbol->next;
				new_symbol->component.operand.value = symbol->next->next;
				new_symbol->component.operand.right_parenthesis = symbol->next->next->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length + symbol->next->next->string.length + symbol->next->next->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next->next->next)symbols.last_symbol = new_symbol;
				symbol->next->next->next->previous = NULL;
				symbol->next->next->next->next = NULL;
				symbol->next->next->previous = NULL;
				symbol->next->next->next = NULL;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<operand> ::= <function> <left_parenthesis> <formula> <right_parenthesis>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case function_acos:
			// <function> ::= <function_acos>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_acos>\n");
			print_symbols(symbols);
			#endif
			break;
		case function_acosh:
			// <function> ::= <function_acosh>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_acosh>\n");
			print_symbols(symbols);
			#endif
			break;
		case function_asin:
			// <function> ::= <function_asin>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_asin>\n");
			print_symbols(symbols);
			#endif
			break;
		case function_asinh:
			// <function> ::= <function_asinh>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_asinh>\n");
			print_symbols(symbols);
			#endif
			break;
		case function_atan:
			// <function> ::= <function_atan>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_atan>\n");
			print_symbols(symbols);
			#endif
			break;
		case function_atanh:
			// <function> ::= <function_atanh>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_atanh>\n");
			print_symbols(symbols);
			#endif
			break;
		case function_cos:
			// <function> ::= <function_cos>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_cos>\n");
			print_symbols(symbols);
			#endif
			break;
		case function_cosh:
			// <function> ::= <function_cosh>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_cosh>\n");
			print_symbols(symbols);
			#endif
			break;
		case function_log:
			// <function> ::= <function_log>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_log>\n");
			print_symbols(symbols);
			#endif
			break;
		case function_sin:
			// <function> ::= <function_sin>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_sin>\n");
			print_symbols(symbols);
			#endif
			break;
		case function_sinh:
			// <function> ::= <function_sinh>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_sinh>\n");
			print_symbols(symbols);
			#endif
			break;
		case function_sqrt:
			// <function> ::= <function_sqrt>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_sqrt>\n");
			print_symbols(symbols);
			#endif
			break;
		case function_tan:
			// <function> ::= <function_tan>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_tan>\n");
			print_symbols(symbols);
			#endif
			break;
		case function_tanh:
			// <function> ::= <function_tanh>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = function;
			new_symbol->component.function.function = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<function> ::= <function_tanh>\n");
			print_symbols(symbols);
			#endif
			break;
		case i:
			if(symbol->previous && symbol->previous->type == factor)break;
			if(symbol->previous && symbol->previous->type == operand)break;
			if(symbol->previous && symbol->previous->type == power)break;
			if(symbol->previous && symbol->previous->type == numbers)break;
			// <operand> ::= <i>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = operand;
			new_symbol->component.operand.function = NULL;
			new_symbol->component.operand.value = symbol;
			new_symbol->component.operand.left_parenthesis = NULL;
			new_symbol->component.operand.right_parenthesis = NULL;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<operand> ::= <i>\n");
			print_symbols(symbols);
			#endif
			break;
		case left_parenthesis:
			if(symbol->next && symbol->next->type == formula && symbol->next->next && symbol->next->next->type == right_parenthesis)
			{
				// <operand> ::= <left_parenthesis> <formula> <right_parenthesis>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = operand;
				new_symbol->component.operand.function = NULL;
				new_symbol->component.operand.value = symbol->next;
				new_symbol->component.operand.left_parenthesis = symbol;
				new_symbol->component.operand.right_parenthesis = symbol->next->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length + symbol->next->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next->next)symbols.last_symbol = new_symbol;
				symbol->next->next->previous = NULL;
				symbol->next->next->next = NULL;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<operand> ::= <left_parenthesis> <formula> <right_parenthesis>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case minus:
		case plus:
			if(symbol->next && symbol->next->type == factor)
			{
				if(symbol->previous && symbol->previous->type != left_parenthesis && symbol->previous->type != comma)break;
				if(symbol->next->next && symbol->next->next->type == asterisk)break;
				if(symbol->next->next && symbol->next->next->type == slash)break;
				// <term> ::= <minus> <factor> | <plus> <factor>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = term;
				new_symbol->component.term.term = NULL;
				new_symbol->component.term.operator = symbol;
				new_symbol->component.term.factor = symbol->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next)symbols.last_symbol = new_symbol;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<term> ::= <minus> <factor> | <plus> <factor>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case number:
			// <numbers> ::= <number>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = numbers;
			new_symbol->component.numbers.numbers = NULL;
			new_symbol->component.numbers.number = symbol;
			new_symbol->component.numbers.level = 0;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<numbers> ::= <number>\n");
			print_symbols(symbols);
			#endif
			break;
		case numbers:
			if(symbol->next && symbol->next->type == number)
			{
				// <numbers> ::= <numbers> <number>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = numbers;
				new_symbol->component.numbers.numbers = symbol;
				new_symbol->component.numbers.number = symbol->next;
				new_symbol->component.numbers.level = symbol->component.numbers.level + 1;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next)symbols.last_symbol = new_symbol;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<numbers> ::= <numbers> <number>\n");
				print_symbols(symbols);
				#endif
			}
			else if(symbol->next && symbol->next->type == dot && symbol->next->next && symbol->next->next->type == numbers)
			{
				// <absolute> ::= <numbers> <dot> <numbers>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = absolute;
				new_symbol->component.absolute.integer = symbol;
				new_symbol->component.absolute.dot = symbol->next;
				new_symbol->component.absolute.decimal = symbol->next->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length + symbol->next->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next->next)symbols.last_symbol = new_symbol;
				symbol->next->next->previous = NULL;
				symbol->next->next->next = NULL;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<absolute> ::= <numbers> <dot> <numbers>\n");
				print_symbols(symbols);
				#endif
			}
			else if(symbol->previous && symbol->previous->type == dot)break;
			else if(symbol->previous && symbol->previous->type == number)break;
			else if(symbol->next && symbol->next->type == dot)break;
			else if(symbol->next && symbol->next->type == number)break;
			else
			{
				// <absolute> ::= <numbers>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = absolute;
				new_symbol->component.absolute.integer = symbol;
				new_symbol->component.absolute.dot = NULL;
				new_symbol->component.absolute.decimal = NULL;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<absolute> ::= <numbers>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case operand:
			if(symbol->previous && symbol->previous->type == circumflex)break;
			else
			{
				// <power> ::= <operand>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = power;
				new_symbol->component.power.power = NULL;
				new_symbol->component.power.circumflex = NULL;
				new_symbol->component.power.operand = symbol;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<power> ::= <operand>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case pi:
			// <operand> ::= <pi>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = operand;
			new_symbol->component.operand.function = NULL;
			new_symbol->component.operand.value = symbol;
			new_symbol->component.operand.left_parenthesis = NULL;
			new_symbol->component.operand.right_parenthesis = NULL;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<operand> ::= <pi>\n");
			print_symbols(symbols);
			#endif
			break;
		case power:
			if(symbol->next && symbol->next->type == circumflex && symbol->next->next && symbol->next->next->type == operand)
			{
				// <power> ::= <power> <circumflex> <operand>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = power;
				new_symbol->component.power.power = symbol;
				new_symbol->component.power.circumflex = symbol->next;
				new_symbol->component.power.operand = symbol->next->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length + symbol->next->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next->next)symbols.last_symbol = new_symbol;
				symbol->next->next->previous = NULL;
				symbol->next->next->next = NULL;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<power> ::= <power> <circumflex> <operand>\n");
				print_symbols(symbols);
				#endif
			}
			else if(symbol->next && symbol->next->type == circumflex)break;
			else if(symbol->previous && symbol->previous->type == asterisk)break;
			else if(symbol->previous && symbol->previous->type == slash)break;
			else
			{
				// <factor> ::= <power>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = factor;
				new_symbol->component.factor.factor = NULL;
				new_symbol->component.factor.operator = NULL;
				new_symbol->component.factor.power = symbol;
				new_symbol->component.factor.i = NULL;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<factor> ::= <power>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case right_parenthesis:
			break;
		case slash:
			break;
		case term:
			if(symbol->next && (symbol->next->type == plus || symbol->next->type == minus) && symbol->next->next && symbol->next->next->type == factor)
			{
				if(symbol->next->next->next && symbol->next->next->next->type == asterisk)break;
				if(symbol->next->next->next && symbol->next->next->next->type == i)break;
				if(symbol->next->next->next && symbol->next->next->next->type == slash)break;
				// <term> ::= <term> <plus> <factor> | <term> <minus> <factor>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = term;
				new_symbol->component.term.term = symbol;
				new_symbol->component.term.operator = symbol->next;
				new_symbol->component.term.factor = symbol->next->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length + symbol->next->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next->next)symbols.last_symbol = new_symbol;
				symbol->next->next->previous = NULL;
				symbol->next->next->next = NULL;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<term> ::= <term> <plus> <factor> | <term> <minus> <factor>\n");
				print_symbols(symbols);
				#endif
			}
			else if(symbol->next && symbol->next->type == plus)break;
			else if(symbol->next && symbol->next->type == minus)break;
			else
			{
				// <formula> ::= <term>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = formula;
				new_symbol->component.formula.term = symbol;
				new_symbol->component.formula.comma = NULL;
				new_symbol->component.formula.left_formula = NULL;
				new_symbol->component.formula.right_formula = NULL;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<formula> ::= <term>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		default:
			ERROR(); // Invalid symbol
			exit(-1);
			break;
		}
	}
	if(flags & SYNTACTIC_ANALYSIS_FLAG_CHANGED)return syntactic_analysis(symbols);
	return symbols;
}

