/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *       G E N E R I C   P R O G R A M M I N G
 *
 *    Programming: Principles and Practice Using C++ by Bjarne Stroustrup
 *
 *  Chapter 20: Containers and Iterators 
 * 
 *  Exercise 11
 *            
 *  Given a list<int> as a by-reference parameter, make a vector<double> and copy 
 *  the elements of the list into it. Verify that the copy was complete and correct, 
 *  then print the elements in order of increasing value.
 * 
 *  Extra:  Generic Version 4 (using Fraction class)
 *  
 *   I had to add the following to fraction.h (Fraction_2020):
 * 
 *   // Overloaded double cast
 *   operator double() const { 
 *      return double((double)num/(double)den); 
 *   }
 * 
 *  replaced:  
 *   typename std::list<T1>::const_iterator lst_itr = lst.begin();
 *   while (lst_itr != lst.end())  {
 *       v.push_back(static_cast<T2>(*lst_itr));
 *       ++lst_itr;
 *    }
 * 
 *  with:
 *     for (const auto& element : lst)  {
 *       v.push_back(static_cast<T2>(element));
 *     }
 * 
 * 
*/
 
 #include "fraction.h"
 #include <iostream>
 #include <stdexcept>
 #include <string>
 #include <vector>
 #include <list>
 #include <sstream>
 #include <experimental/random>
 #include <algorithm>  // for sort


 template<class T> std::string to_string(const T& t)
 {
 	std::ostringstream os;
 	os << t;
 	return os.str();
 }


 struct Range_error : std::out_of_range {
   // enhanced vector range error reporting
 	int index;
 	Range_error(int i)
      : out_of_range("Range error: " + to_string(i)), index(i) { }
 };


template<class Iter>
void print_container(Iter first, Iter last)
{
    std::cout << "{";
    while (first != last) {
        std::cout << " " << *first;
        ++first;
    }
    std::cout << " }\n";
}


template<typename T1, typename T2>
std::vector<T2> list_to_vector(const std::list<T1>& lst, T2 dummy)
{
   std::vector<T2> v;

   for (const auto& element : lst)  {
        v.push_back(static_cast<T2>(element));
   }  

   return v;    
}


// to verify copy
template<typename T1, typename T2>
void verify(const std::list<T1>& lst, const std::vector<T2>& vec)
{
    std::cout << "Length of list:   " << lst.size() << "\n";
    std::cout << "Length of vector: " << vec.size() << "\n";
    std::cout << "\nList:   ";
    print_container(lst.begin(), lst.end());
    std::cout << "\nVector: ";
    print_container(vec.begin(), vec.end());
    std::vector<T2> vec_sorted = vec;
    std::sort(vec_sorted.begin(), vec_sorted.end());
    std::cout << "\nSorted: ";
    print_container(vec_sorted.begin(), vec_sorted.end());
    std::cout << std::endl;
}


int main()
try {

    std::list<double> lst_R;
    for (int i = 0; i < 7; ++i)
        lst_R.push_back((double)std::experimental::randint(0,99) /
                                std::experimental::randint(0,99) );

    std::vector<Fraction> vec_Q = list_to_vector(lst_R, Fraction(1, 1));
    verify(lst_R, vec_Q);

    std::cout << "\n-------------------------------\n";
    std::list<Fraction> lst_Q;
    for (int i = 0; i < 7; ++i)
        lst_Q.push_back(Fraction(std::experimental::randint(0,99),
                                 std::experimental::randint(0,99)) );

    std::vector<double> vd = list_to_vector(lst_Q, 1.0 );
    verify(lst_Q, vd);
}
catch (Range_error& re) {
    std::cerr << "bad index: " << re.index << "\n";
}
catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << std::endl;
}
catch (...) {
    std::cerr << "exception\n";
}