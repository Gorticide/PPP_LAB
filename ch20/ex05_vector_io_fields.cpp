/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *       G E N E R I C   P R O G R A M M I N G
 *
 *    Programming: Principles and Practice Using C++ by Bjarne Stroustrup
 *
 *    Chapter 20: Containers and Iterators
 *
 *    Exercise 5: Extra === Fields Approach
 *                Algebra++ Generic Programming 
 *                inspired by [Dolciani, 1964/1988] Introductory Analysis
 *               
 *      Define input and output operators (<<, >>) for vector.
 *
 *      g++ -g -Wall fraction.cpp ex05_vector_io_fields.cpp -o ex5++
 */

#include "fraction.h"
#include <complex>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>   
#include <limits>  // numeric_limits<streamsize>::max()

void pause()  
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string dummy;
    std::cout << "\n\t Press ENTER [ <-----||| ] to continue ... \n";
    std::getline(std::cin, dummy);
}

template<class T>
void get_elements(std::istream& is3, std::vector<T>& v, unsigned int sz) {
  unsigned int count = 0;
  if (v.size()) { 
      unsigned int init_size = v.size();
      v.shrink_to_fit();

      for (T t : v)  {
          if (count < sz) {
               std::cout << "\nEnter element " << (++count) + init_size
                         << " of " << v.size() << ": ";
               is3 >> t;
               v.push_back(t);
           }
          else break;
      }
   }
   else  {
      v.resize(sz); 
      for (T t : v)  {
          if (count < sz) {
               std::cout << "\nEnter element " << ++count 
                         << " of " << sz << ": ";
               is3 >> t;
               v.push_back(t);
               v.erase(v.begin());
          }
          else break;
       }
  }
}  

template<class T>
void process(std::istream& is2, char Field, std::vector<T>& vec, unsigned int sz) {

    if ((Field == 'Z') || (Field == 'z'))   {  
     	std::cout << "\nEnter elements over Integer Field, x = a\n";
	    get_elements(is2, vec, sz);

      }

    if ((Field == 'Q') || (Field == 'q'))   {  
       std::cout << "\nEnter elements over Rational Field, x = a/b\n";
       std::cout << "\nInclude division symbol between integers, as in 1/2\n";
       get_elements(is2, vec, sz);
      }

   if ((Field == 'R') || (Field == 'r'))   {  
       std::cout << "\nEnter elements over Real Field, x = [decimal number]\n";
 	     get_elements(is2, vec, sz);
      }

     if ((Field == 'C') || (Field == 'c'))   
     { 
       std::cout << "\nEnter elements over Complex Field, x = a + bi\n";
 	   get_elements(is2, vec, sz);
      }

    if ((Field == 'h') || (Field == 'H'))   
     { 
       std::cout << "\nEnter elements over field of characters:\n";
 	   get_elements(is2, vec, sz);
      }
          
    if ((Field == 's') || (Field == 'S'))   
     { 
       std::cout << "\nEnter elements over field of strings, x = a + bi\n";
 	   get_elements(is2, vec, sz);
      }

}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    for (auto& a : v)
        os << a << "  ";
    return os;
}

char choose_Field()  {
   char field = 'R';
   std::cout << "\n--------------------------------------------------------------\n";
   std::cout << "Vector elements over which field?"<< std::endl;
   std::cout << "\n--------------------------------------------------------------\n";
   std::cout << "   Z (Elements over the filed of Integers x = a)\n"
             << "   Q (Elements over the field of rational numbers x = a/b)\n"
             << "   R (Elements over the field of real numbers x = a.)\n"
             << "   C (Elements over the field of complex numbers x = a + i b)\n"
             << "   h (Elements over the field of characters ' ')\n"
             << "   s (Elements over the field of strings)\n"
             << "\n--------------------------------------------------------------\n"
             << "Choose Field :  ";

 std::cin >> field;
 std::cout << std::endl;
 return field;
}


int reserve_space()  {
    std::cout << "\nHow many elements are to be stored in this vector? : ";
    int num;
    std::cin >> num;
    return num;
}

template<typename T>
std::istream& operator>>(std::istream& is, std::vector<T>& v)
{
  char F;  // where F represents Field
  unsigned int size; // how many elements to be stored in this vector?

    F = choose_Field();  // Initialize Field 
    size = reserve_space();
    if (size) process(is, F, v, size);

    return is;
}

 // For setting precision in std::to_string(number), where number is type double
 // needs <sstream> and <iomanip>
  std::string to_string_with_precision(const double a_value, const int n = 4)
 {
     std::ostringstream out;
     out << std::setprecision(n) << a_value;
     return out.str();
 }


std::ostream& operator<<(std::ostream& os, const std::complex<double>& z)
{
  double tol = 0.000001;
  std::string a = "";   // 'a' for Algebraic Form
	double REAL = z.real();
	double IMAG = z.imag();

	if (std::fabs(REAL) < tol) REAL = 0;
	if (std::fabs(IMAG) < tol) IMAG = 0;

  a.append(to_string_with_precision(REAL));
	if (z.imag() < 0) a.append(" - i " + to_string_with_precision(-IMAG));
	else a.append(" + i " + to_string_with_precision(IMAG));
	os << a;
    return os;
}  


 // For dealing with cin and std::complex<double>
std::istream& operator>> (std::istream& is, std::complex<double>& z) {
  double value;
  std::cout << "\nz = x + i y.  Enter real part x: ";
  is >> value;
  z.real(value);
  std::cout << "\nEnter imaginary part y: ";
  is >> value;
  z.imag(value);
  return is;
}


int main()
try {
    // with ints
    std::vector<int> vec_int { 1, 2, 3, 4 , -3, -7};
    std::cout << "\nPrinting vec_int: ";
    std::cout << vec_int << '\n';
    
    std::cout << "\nNow add new integers to vec_int.\n";
    std::cin >> vec_int;
    std::cout << "\nNow vec_int is: ";
    std::cout << vec_int << '\n';
    pause();

    // with doubles
    std::vector<double> vec_real;
    std::cout << "\nEnter real numbers into vec_real.\n";
    std::cin >> vec_real;
    std::cout << "\nNow vec_real is: ";
    std::cout << vec_real << '\n';
    pause();

    // with chars - doesn't play nicely with std::cin
    std::vector<char> vec_char;
    std::cout << "\nEnter characters into vec_char.\n";
    std::cin >> vec_char;
    std::cout << "\nNow vec_char is: ";
    std::cout << vec_char << '\n';
    pause();

    // with complex
    std::vector< std::complex<double> > vec_complex;
    std::cout << "\nEnter complex numbers into vec_complex.\n";
    std::cin >> vec_complex;
    std::cout << "\nNow vec_complex is: ";
    std::cout << vec_complex << '\n';
    pause();


    // with Fractions
    std::vector<Fraction> vec_rational;
    std::cout << "\nEnter Fractions into vec_rational.\n";
    std::cin >> vec_rational;
    std::cout << "\nNow vec_rational is: ";
    std::cout << vec_rational << '\n';
    pause();

    // with strings
    std::vector<std::string> vec_string;
    std::cout << "\nEnter strings into vec_string.\n";
    std::cin >> vec_string;
    std::cout << "\nNow vec_string is: ";
    std::cout << vec_string << '\n';
    pause();
}
catch(std::exception& e) {
    std::cerr << "Exception: " << e.what() << '\n';
    return 1;
}
catch(...) {
    std::cerr << "Unknown exception\n";
    return 2;
}

