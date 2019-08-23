/*

   Simple calculator reimagined by Mike Hentrich with help of several resources.
   12 December 2018
   Revamped as Integer Calculator ====> count
   Version 1.0 attempts to improve '!' factorial from 20! to 170! with newly designed function
             Also plans to implement C(n, r) for "combinations"
             and P(n,r) for "permutations" using this new factorial function

   g++ -lgmpxx -lgmp -g integer_calculator.cpp -std=c++11 -o count

   I removed power (exponentiation) and modular arithmetic until
   I learn more about:

   Function: void mpz_pow_ui (mpz_t rop, const mpz_t base, unsigned long int exp)
   Function: void mpz_ui_pow_ui (mpz_t rop, unsigned long int base, unsigned long int exp)

       Set rop to base raised to exp. The case 0^0 yields 1.

  Function: void mpz_mod (mpz_t r, const mpz_t n, const mpz_t d)

  Function: unsigned long int mpz_mod_ui
                    (mpz_t r, const mpz_t n, unsigned long int d)

         Set r to n mod d. The sign of the divisor is ignored; the result is always non-negative.

Original:  2 December 2018
   Version 1.2  with recursive definition of factorial dealing with just integers
    (also binds '!' tighter than square root)
    // declare secondary before primary defition so as to allow square_root unary operator
       to bind factorial ! tighter than @ (sqrt)
              This makes @6! ---> @(6!),
              otherwise if r = primary(), then @6! ---> (@6)! which is factorial of double. not int

   This program implements a basic expression calculator.
   Input from cin; output to cout
   The grammar for input is:


   /*
      Calculation:
          Statement
          Print
          Quit
          Calculation Statement

      Statement:
          Declaration
          Expression

      Declaration:
          let Name = Expression

     Statement:
        Expression
        Print
        Quit

     Print:
        ;

     Quit:
        q

    Expression:
       Term
       Expression + Term
       Expression - Term

   Term:
       Secondary
       Term * Secondary
       Term / Secondary
       Term % Secondary

   Secondary:
       Primary
       Number !

   Primary:
       Number
       ( Expression )
       - Primary
       + Primary

   Number:
       //floating-point-literal
       mpz_class literal
*/

// Stroustrup - Programming Principles & Practice
//
// Chapter 7 - Calculator
//
// Adding fmod() floating-point modulo from <cmath>
// modified term() and token_stream::get()
// fmod(n,d) is defined as n%d == n - y*(int(n/d))
//
// Also added:
// const char number = '8';
// const char quit = 'q';
// const char print = ';';
// const string prompt = "> ";
// const string result = "= ";
//
// We also created function calculate() for expression evaluation.
// This makes main() cleaner so it provides general "scaffolding":
//    start the program, end the program, and handle "fatal" errors

// Adding class Variable


#include "std_lib_facilities.h"
#include <cstdio>
//#include <iomanip>
//#include <cmath>  // for lgamma()
#include <gmpxx.h>   // g++ -lgmpxx -lgmp -g nPk_gmp.cpp -std=c++17 -o nPk

// SYMBOLIC CONSTANTS
const char number = '8';
const char quit = 'q';
const char print = ';';
const char name = 'a';
const char let = 'L';
const char constant = 'g';
const char help = '?';
//const char c_sin = 's';
//const char c_cos = 'c';

const string prompt = "> ";
const string result = "= ";  // used to indicate that what follows is a result
const string declkey = "let";   //declaration keyword
const string constkey = "constant";  // constant keyword

// Removing things from "double floating point" calculator to create a calculator
// that uses just integers for counting (factorial, combinations, and permuations)

//const char square_root = '@';
const char exponent = '^';
const char powexp = 'e';
const char nCr = 'C';
const char fnCr = 'c';    // don't need 'c' for cos when dealing with integers only
const char nPr = 'P';
const char fnPr = 'p';
//const string sqrtkey = "sqrt";
const string expkey = "pow";
const string ncrkey = "nCr";
const string nprkey = "nPr";
//const string sinkey = "sin";
//const string coskey = "cos";
const string quitkey = "quit";
const string helpkey = "help";

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class Token {  // altered for different calculator ***************************
public:
    char kind;
    mpz_class value;  // for Big Integers
    string name;

    Token(char k) : kind{k}, value{0} { }
    Token(char k, mpz_class v) : kind{k}, value{v} { }
    Token(char k, string n) : kind{k}, value{0}, name{n} { }
};

class Token_stream {
public:
    Token get();                // get a Token
    void putback(Token t);      // put a token back
    void ignore(char c);   // discard characters up to and including a c

private:
    bool full { false };   // is there a Token in the buffer?
    Token buffer {' '};    // here is where putback() stores a Token
                     // put back using putback()
};


void Token_stream::ignore(char c)
 // c represents the kind of Token
 {
  // first look in buffer
  if (full && c == buffer.kind)  {
    full = false;
    return;
  }
  full = false;

  // now search input
  char ch = 0;
  while (cin >> ch)
      if (ch == c) return;
 }

void Token_stream::putback(Token t)
{
    buffer = t;                 // copy t to buffer
    full = true;                // buffer is now full
};


Token Token_stream::get()
    // added '%'
{
    if (full) {             // do we already have a Token?
        full = false;       // remove Token from buffer
        return buffer;
    }

    char ch;
    //cin >> ch;              // note that >> skips whitespace
    cin.get(ch);    // note that cin.get() does NOT skip whitespace
    while (isspace(ch)) {
       if (ch == '\n') return Token(print); // if newline detected,
       cin.get(ch);                         // return print Token
    }

    switch (ch) {
        case print:           // for "print"
        case quit:           // for "quit"
        case help:
        case '(':
        case ')':
        case '{':
        case '}':
        case '+':
        case '-':
        case '*':
        case '/':
        case '!':
        case '%':
        case '=':
        case ',':
        case '^':
        //case '@':
        case 'e':
        case 'C':
        case 'P':
            return Token { ch };  // let each character represent itself
        case '.':
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            {
                cin.putback(ch);    // put digit back into input stream
                double val;
                cin >> val;
                return Token { number, val };  // let '8' represent a number
            }
        default:
            if (isalpha(ch)) {  // if it is a letter
                string s;
                s += ch;  // Put the letter in 's' because it has been already read
                while (cin.get(ch) &&
                        ((isalpha(ch) || isdigit(ch) || ch == '_'))
                      )
                      s += ch;     // Continue to read in 's'
               cin.putback(ch);    // Return the character into the stream
               if (s == declkey) return Token{let};    // declaration keyword
               else if (s == constkey) return Token{constant};
               else if (s == expkey) return Token{powexp};
               else if (s == ncrkey) return Token{fnCr};
               else if (s == nprkey) return Token{fnPr};
               //else if (s == sqrtkey) return Token{square_root};
              // else if (s == sinkey) return Token{c_sin};
              // else if (s == coskey) return Token{c_cos};
               else if (s == quitkey) return Token{quit};
               else if (s == helpkey) return Token{help};
               else return Token{name, s};
            }            // exercise 05 (Chapter 7)
            error("Bad token");
    }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/*  Implementing variables is a major extension.  It touches just about every
    function and adds a completely new concept to the calculator.
    It increased the size of the calculator by 45%.

    See extended notes at end of code (from another student on the Internet)
*/
// * * * *

class Variable {
public:
    string name;
    mpz_class value;
    bool constant;
    Variable(string n, mpz_class v, bool c = false)
        : name{n}, value{v}, constant{c} { }
};

class Symbol_table {
    vector<Variable> var_table;
public:
    bool is_declared(string);
    mpz_class get(string);
    mpz_class set(string, mpz_class);
    mpz_class declare(string, mpz_class, bool con = false);
};

bool Symbol_table::is_declared(string var)
    // is var already in var_table?
{
    for (const Variable& v : var_table)
        if (v.name == var) return true;
    return false;
}

mpz_class Symbol_table::get(string s)
    // return the value of the Variable named s
{
    for (const Variable& v : var_table)
        if (v.name == s) return v.value;
    error("get: undefined variable ", s);
}

mpz_class Symbol_table::set(string s, mpz_class d)
    // set the Variable named s to d
{
    for (Variable& v : var_table)
        if (v.name == s) {
            if (v.constant) error("Can't overwrite constant variable");
            v.value = d;
            return d;
        }
    error("set: undefined variable ", s);
}

mpz_class Symbol_table::declare(string var, mpz_class val, bool con)
    // add {var,val,con} to var_table
{
    if (is_declared(var)) error(var, " declared twice");
    var_table.push_back(Variable{var,val,con});
    return val;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// globals(?)

Symbol_table st;            // allows Variable storage and retrieval
Token_stream ts;            // provides get() and putback()

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// forward declaration for primary() to call
mpz_class expression();

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// additional calculator functions
/*  REPLACED for mpz_class version
long long factorial(int integer){
   if(integer <= 1) return 1;
   return integer*factorial(integer-1);
}


double calc_sqrt()
{
    char ch;
    if (cin.get(ch) && ch != '(') error("'(' expected");
    cin.putback(ch);
    double d = expression();
    if (d < 0) error("sqrt: negative val is imaginary");
    return sqrt(d);
}

mpz_class calc_pow()
{
    Token t = ts.get();
    if (t.kind != '(') error("'(' expected");
    double base = narrow_cast<double>(expression());
    t = ts.get();
    if (t.kind != ',') error("',' expected");
    double power = narrow_cast<double>(expression());
    t = ts.get();
    if (t.kind != ')') error("')' expected");
    return mpz_class( pow(base, power) );
}
/*
double calc_sin()
{
    char ch;
    if (cin.get(ch) && ch != '(') error("'(' expected");
    cin.putback(ch);
    double d = expression();
    if (d == 0 || d == 180) return 0;       // return true zero
    return sin(d*3.1415926535/180);
}

double calc_cos()
{
    char ch;
    if (cin.get(ch) && ch != '(') error("'(' expected");
    cin.putback(ch);
    double d = expression();
    if (d == 90 || d == 270) return 0;      // return 0 instead of 8.766e-11
    return cos(d*3.1415926535/180);
}
*/

mpz_class factorial(mpz_class n)
{
    mpz_class result(n); // initialize an arbitrary-sized integer with 'n'
    if (n == 0) return 1;
    while(n-- > 1) result *= n; // compute the product with every integers < n
    return result;
}

mpz_class nCk(mpz_class n, mpz_class k)  {
    return factorial(n)/(factorial(n-k) * factorial(k) );
}

mpz_class calc_nCk()
{
    Token t = ts.get();
    if (t.kind != '(') error("'(' expected");
    mpz_class n = expression();
    t = ts.get();
    if (t.kind != ',') error("',' expected");
    mpz_class k = expression();
    t = ts.get();
    if (t.kind != ')') error("')' expected");
    return nCk(n, k);
}

mpz_class nPk(mpz_class n, mpz_class k)  {
    return factorial(n)/factorial(n-k);
}

mpz_class calc_nPk()
{
    Token t = ts.get();
    if (t.kind != '(') error("'(' expected");
    mpz_class n = expression();
    t = ts.get();
    if (t.kind != ',') error("',' expected");
    mpz_class k = expression();
    t = ts.get();
    if (t.kind != ')') error("')' expected");
    return nPk(n, k);
}


mpz_class handle_variable(Token& t)
{
    Token t2 = ts.get();
    if (t2.kind == '=')
        return st.set(t.name, expression());
    else {
        ts.putback(t2);
        return st.get(t.name);       // missing in text!
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// input grammar functions
mpz_class secondary();  // declare here so as to allow square_root unary operator
                     // to bind factorial ! tighter than @ (sqrt)
                     // This makes @6! ---> @(6!), otherwise if r = primary(),
                     // then @6! ---> (@6)! which is factorial of double. not int

mpz_class primary()            // deal with numbers and parenthesis/braces
{
    Token t = ts.get();
    switch (t.kind) {
        case '(':                   // handle '(' expression ')'
            {
                mpz_class d = expression();
                t = ts.get();
                if (t.kind != ')') error("')' expected");
                return d;
            }
        case '{':
            {
                mpz_class d = expression();
                t = ts.get();
                if (t.kind != '}') error("'}' expected");
                return d;
            }
        case number:                   // we use '8' to represent a number
            return t.value;         // return the number's value
        case name:
            return handle_variable(t);
        case '-':
            return -primary();
        case '+':
            return primary();
    /*
        case square_root:
           {
             double r = secondary();
             if (r < 0) error("sqrt: negative val is imaginary");
             return sqrt(r);
          }
        case c_sin:
            return calc_sin();
        case c_cos:
            return calc_cos();

        case powexp:
              return mpz_class(calc_pow());
  */
        case fnCr:
             return calc_nCk();
        case fnPr:
             return calc_nPk();
        default:
            error("primary expected");
    }
}

mpz_class secondary()
    // ex 3 - Add a factorial operator '!'
{
    mpz_class left = primary();
    Token t = ts.get();

  while (true) {
        if (t.kind == '!') {
/*            if (left == 0)
                return 1;
            int n = left - 1;
            for (int i = n; i > 0; --i)
                left *= i;
*/
// replace with Big Integer mpz_class version
        mpz_class fac  = factorial(left);
        left = fac;
         t = ts.get();
        }
        else {
            ts.putback(t);
            return left;
        }
    }
}

mpz_class term()               // deal with * and /
{
    mpz_class left = secondary();
    Token t = ts.get();             // get next token from Token_stream

    while (true) {
        switch (t.kind) {
            case '*':
                left *= secondary();
                t = ts.get();
                break;
            case '/':
                {
                    mpz_class d = secondary();
                    if (d == 0) error("divide by zero");
                    left /= d;
                    t = ts.get();
                    break;
                }
                /*
            case '%':
            {
                mpz_class d = primary();
                if (d == 0) error("%:divide by zero");
                mpz_t temp;

                left(temp);
                t = ts.get();
                break;
            }
          case exponent:
                left = pow(narrow_cast<int>(left), \
                           narrow_cast<int>(secondary()) );
                t = ts.get();
                break;
    */
          case nCr:
               left = nCk(left, secondary());
               t = ts.get();
               break;

          case nPr:
              left = nPk(left, secondary());
              t = ts.get();
              break;
          default:
                ts.putback(t);      // put t back into the Token_stream
                return left;
        }
    }
}

mpz_class expression()         // deal with + and -
{
    mpz_class left = term();           // read and evaluate a term
    Token t = ts.get();             // get next token from Token_stream

    while (true) {
        switch (t.kind) {
            case '+':
                left += term();     // evaluate term and add
                t = ts.get();
                break;
            case '-':
                left -= term();     // evaluate term and subtract
                t = ts.get();
                break;
            default:
                ts.putback(t);      // put t back into the token stream
                return left;
        }
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
mpz_class declaration(bool b)
    // assume we have seen "let" or "constant"
    // handle: name = expression
    // declare a variable called "name" with the initial value "expression"
{
    Token t = ts.get();
    if (t.kind != name) error("name expected in declaration");
    string var_name = t.name;

    Token t2 = ts.get();
    if (t2.kind != '=') error("= missing in declaration of ", var_name);

    mpz_class d = expression();
    st.declare(var_name, d, b);
    return d;
}

mpz_class statement()  // handles declarations and expressions
{
    Token t = ts.get();
    switch (t.kind) {
        case let:
            return declaration(false);

        case constant:
            return declaration(true);

        default:
            ts.putback(t);
            return expression();
    }
}

void print_help()
{
    cout << "Big Integer Calculator Tips\n"
         << "========================\n"
         << "This calculator program supports +, -, *, and / operations\n"
         << "Enter any form of compound statement followed by ';' for result\n"
         << "- ex: 4 + 1; (5-2)/{6*(8+14)}\n"
        // << "The modulo operator % may be used on all numbers\n"
         << "An '!' placed after a value will calculate the factorial of it\n"
         << "- ex: 4! = 4 * 3 * 2 * 1\n"
      //   << "You may use ^ for exponent, and enforce binding\n"
      //   << "- ex: > {(2^4)*(5^3)}*{{(2*5*11)/{(2^2)*(5^2)}}^5} = 3221.02\n\n"
         << "- ex: 44!/(12!*32!)\n"
         << " Combinations and Permutations:\n"
         << "nCr(8,4)*nCr(5,2) = 700, but 8C4*5C2 = 61075 (not what we want!)\n"
         << "You need to enforce binding: (8C4)*(5C2) = 700\n\n"
         << "Variable assignment is provided using the 'let' keyword:\n"
         << "- ex: let x = 37; x * 2 = 74; x = 4; x * 2 = 8\n\n";
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void clean_up_mess()  {
    ts.ignore(print);
}

void calculate()   //expression evaluation loop
{
  while (cin)
    try {
      cout << prompt;
      Token t = ts.get();
      while (t.kind == print) t = ts.get();  // eats ';' to discard extra 'prints'
      if (t.kind == help) print_help();
      else if (t.kind == quit)  return;  // for a clean exit!
      else {
        ts.putback(t);
        cout << result << statement() << '\n';
      }

    }
    catch(exception& e) {
        cerr << e.what() << '\n';
        clean_up_mess();
      }
}


int main()
try {
   //st.declare("pi", 4*atan(1), true);       // hardcoded constants
   //st.declare("e", 2.7182818284, true);

   cout << "Big Integer Calculator (type ? for help)\n";
   calculate();
   // keep_window_open();  // cope with Windows console mode
   return 0;
}
catch(exception& e) {
    cerr << e.what() << '\n';
    //keep_window_open("~~");
    return 1;
}
catch(...) {
    cerr << "exception\n";
    return 2;
}

/*
Grammar (added Unary Minus to Primary)
also adding new Top Production (rule) of the grammar: Calculation
See below for updated grammar.
=======
Calculation:
    Statement
    Print
    Quit
    Calculation Statement

Statement:
    Declaration
    Expression

Declaration:
    "let" Name "=" Expression

Expression:
    Term
    Expression "+" Term
    Expression "-" Term
Term:
    Secondary
    Term "*" Secondary
    Term "/" Secondary
Secondary:
    Primary
    Number "!"
Primary:
    Number
    "(" Expression ")"
    "-" Primary
    "+" Primary
Number:
    mpz_class Big Integer

  /*
      The updated grammar for input is:

      Calculation:
          Statement
          Print
          Help
          Quit
          Calculation Statement

      Statemant:
          Declaration
          Expression

      Print:
          ;

      Quit:
          q

      Declaration:
          "let" Name "=" Expression

      Expression:
          Term
          Expression + Term
          Expression - Term

      Term:
          Secondary
          Term * Secondary
          Term / Secondary
          Term % Secondary

      Secondary:
          Primary "!"
          "Sqrt (" Expression ")"
          "Pow (" Expression "," Expression ")"

      Primary:
          Number
          ( Expression )
          { Expression }
          - Primary
          + Primary
          Variable
          "sqrt"( Expression )
          "pow(" Expression "," narrow_cast<int>(Expression) ")"

      Number:
              mpz_class Big Integer

      Input comes from cin through the Token_stream called ts.

*/
