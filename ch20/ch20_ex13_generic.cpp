/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Programming: Principles and Practice Using C++ by Bjarne Stroustrup
 *
 *  Chapter 20: Containers and Iterators 
 *           Extra  G E N E R I C   Version for Exercises 12 and 13 
 *           Tested with Fractions, doubles, ints, chars, strings
 *           Also erase and insert that are not used (yet)
 * 
 *  Exercise 12: complete the definition of list from 20.4.1-2 
 *    and get the high() example to run. Allocate a Link to represent 
 *    one past the end.
 *
 *  Exercise 13: Modify solution to ex 12 to use 0 (nullptr?) to represent 
 *     a pointer to the one-past-the-end Link (list<Elem>::end())
 *
 *   I am keeping size as an attribute of container my_list
 *   because I need it for get_elements into the istream ... 
 *  
 *   g++ -g -Wall fraction.cpp ch20_ex13_generic.cpp -o ex13++
 *
 */

#include "fraction.h"
#include <iostream>
#include <stdexcept>
#include <initializer_list>
#include <string>
#include <limits>  // numeric_limits<streamsize>::max()
#include <initializer_list>


template<typename Elem> struct Link {
    Elem val;           // the value
    Link* prev;         // previous link
    Link* succ;         // successor (next) link
    Link(const Elem& v = Elem{0}, Link* p = nullptr, Link* s = nullptr)
        : val{v}, prev{p}, succ{s} { }
};


template<typename Elem> class my_list {
public:
    my_list() : first{nullptr}, last{first}, sz{0} { }

    explicit my_list(unsigned long n, Elem def = Elem{})
        : first{nullptr}, last{first}, sz{n}
    {
        for (unsigned long i = 0; i < n; ++i) {
            first = new Link<Elem>{def, nullptr, first};
            if (first->succ != nullptr) first->succ->prev = first;
            else last = first;
        }
    }

    my_list(std::initializer_list<Elem> lst)
        : first{nullptr}, last{first}
    {
        unsigned long count = 0;
        for (auto it = lst.begin(); it != lst.end(); ++it) {
            first = new Link<Elem>{*it, nullptr, first};
            if (first->succ != nullptr) first->succ->prev = first;
            else last = first;
            ++count;
        }
        sz = count;
    }

    /*
    my_list(const my_list&);                      // copy
    my_list& operator=(const my_list&);

    my_list(my_list&&);                           // move
    my_list& operator=(my_list&&);
    */

    ~my_list()
    {
        while (first != nullptr) {
            auto p = first->succ;
            delete first;
            first = p;
        }
    }

    class iterator;             // member type: iterator

    iterator begin() { return iterator(first); }
    const iterator begin() const { return iterator(first); }
    iterator end() { return iterator(nullptr); }
    const iterator end() const { return iterator(nullptr); }

    iterator insert(iterator p, const Elem& v); // insert v into my_list before p
    iterator erase(iterator p);                 // remove p from the list

    void push_front(const Elem& v);             // insert v at front
    void push_back(const Elem& v);              // insert v at end

    /*
    void pop_front();           // remove the first element
    void pop_back();            // remove the last element

    Elem& front();              // the first element
    Elem& back();               // the last element

    // ...
    */
    unsigned long size() const { return sz; }
 
    template<typename T>
    friend std::istream& operator>>(std::istream& is, my_list<T>& l);

private:
    Link<Elem>* first;
    Link<Elem>* last;
    unsigned long sz;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// iterator member class

template<typename Elem>         // requires Element<Elem>() (19.3.3)
class my_list<Elem>::iterator {
        Link<Elem>* curr;           // current link
public:
    iterator(Link<Elem>* p) : curr{p} { }

    iterator& operator++() {curr = curr->succ; return *this; }  // forward
    iterator& operator--() {curr = curr->prev; return *this; }  // backward
    Elem& operator*() { return curr->val; }     // get value (dereference)
    Link<Elem>* operator->() { return curr; }
    Link<Elem>* ptr() const { return curr; }

    bool operator==(const iterator& b) const { return curr == b.curr; }
    bool operator!=(const iterator& b) const { return curr != b.curr; }
};
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// member functions

template<typename Elem>
typename my_list<Elem>::iterator my_list<Elem>::insert(my_list<Elem>::iterator it,
        const Elem& val)
{
    Link<Elem>* p = new Link<Elem>{val, it->prev, it.ptr()};

    if (p->prev != nullptr) p->prev->succ = p;
    if (p->succ != nullptr) p->succ->prev = p;

    return iterator(p);
}


template<typename Elem>
typename my_list<Elem>::iterator my_list<Elem>::erase(my_list<Elem>::iterator it)
{
    if (it.ptr() == nullptr) return iterator(it);   // already gone
    iterator ret = iterator(it->prev);              // return next Link

    if (it->prev != nullptr) it->prev->succ = it->succ;
    if (it->succ != nullptr) it->succ->prev = it->prev;

    delete it.ptr();
    return ret;
}


template<typename Elem> void my_list<Elem>::push_front(const Elem& v)
{
    first = new Link<Elem>{v, nullptr, first};
    if (first->succ != nullptr) first->succ->prev = first;
}


template<typename Elem> void my_list<Elem>::push_back(const Elem& v)
{
    if (first == nullptr)  {
        Link<Elem>* p = new Link<Elem>{v};
        p->succ = first;
        first = p;
    }
    auto it = begin();
    while(it->succ != nullptr) ++it;
    it->succ = new Link<Elem>{v, it.ptr()};
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// templated functions
template<typename T>
void print(const my_list<T>& li)
{
    std::cout << "{ ";
    for (auto& a : li)
        std::cout << a << "  ";
    std::cout << "}\n";
}

template<typename Iter, typename Elem>
Iter high(Iter first, Iter last, Elem dummy)
{
    Iter high = first;
    for (Iter p = first; p != last; ++p)
        if (*high < *p) high = p;
    return high;
}


template<typename Elem>
void f(my_list<Elem>& lst, Elem dummy)
{
    //my_list<Elem> lst; 
    //for (Elem x; std::cin >> x; ) lst.push_front(x);
    std::cin >> lst;
    //typename my_list<Elem>::iterator 
    auto p = high(lst.begin(), lst.end(), dummy);
    std::cout << "\n-------------------------------\n"
              << " The highest value was " << *p << '\n'
              << "-------------------------------\n";
}


template<typename T>
void get_elements(std::istream& is3, my_list<T>& lst, unsigned long sz) {
  unsigned long count = 0;
  if (lst.size()) { 
      unsigned long init_size = lst.size();

      for (unsigned long i = 0; i < sz; ++i)  { 
          if (count < sz) {
               std::cout << "\nEnter element " << (++count) + init_size
                         << " of " << lst.size() << ": ";
               T t;
               is3 >> t;
               lst.push_back(t);
           }
          else break;
      }
   }
   else  {

      for (unsigned long i = 0; i < sz; ++i)  { 
               if (count < sz) {
                  std::cout << "\nEnter element " << ++count 
                            << " of " << sz << ": ";
                  T t;
                  is3 >> t;
                  lst.push_back(t);
                }
               else break;
       }
  }
}  


char choose_Field()  {
   char field = 'R';
   std::cout << "\n--------------------------------------------------------------\n";
   std::cout << "Vector elements over which field?"<< std::endl;
   std::cout << "\n--------------------------------------------------------------\n";
   std::cout << "   Z (Elements over the filed of Integers x = a)\n"
             << "   Q (Elements over the field of rational numbers x = a/b)\n"
             << "   R (Elements over the field of real numbers x = a.)\n"
             << "   c (Elements over the field of characters ' ')\n"
             << "   s (Elements over the field of strings)\n"
             << "\n--------------------------------------------------------------\n"
             << "Choose Field :  ";

 std::cin >> field;
 std::cout << std::endl;
 return field;
}


template<typename T>
void process(std::istream& is2, char Field, my_list<T>& lst, unsigned int sz) {

    if ((Field == 'Z') || (Field == 'z'))   {  
     	std::cout << "\nEnter elements over Integer Field, x = a\n";
	    get_elements(is2, lst, sz);

      }

    if ((Field == 'Q') || (Field == 'q'))   {  
       std::cout << "\nEnter elements over Rational Field, x = a/b\n";
       std::cout << "\nInclude division symbol between integers, as in 1/2\n";
       get_elements(is2, lst, sz);
      }

   if ((Field == 'R') || (Field == 'r'))   {  
       std::cout << "\nEnter elements over Real Field, x = [decimal number]\n";
 	     get_elements(is2, lst, sz);
      }


    if ((Field == 'c') || (Field == 'C'))   
     { 
       std::cout << "\nEnter elements over field of characters:\n";
 	   get_elements(is2, lst, sz);
      }
          
    if ((Field == 's') || (Field == 'S'))   
     { 
       std::cout << "\nEnter elements over field of strings, x = a + bi\n";
 	   get_elements(is2, lst, sz);
      }

}

unsigned long reserve_space()  {
    std::cout << "\nHow many elements are to be stored in my list? : ";
    unsigned long num;
    std::cin >> num;
    return num;
}

template<typename T>
std::istream& operator>>(std::istream& is, my_list<T>& l)
{
  char F;  // where F represents Field
  unsigned long size; // how many elements to be stored in this list?

    F = choose_Field();  // Initialize Field 
    size = reserve_space();
    if (size) process(is, F, l, size);

    return is;
}


void pause()  
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string dummy;
    std::cout << "\n\t Press ENTER [ <-----||| ] to continue ... \n";
    std::getline(std::cin, dummy);
}




int main()
try {
    // with ints
    my_list<int> list_int { 1, 2, 3, 4 , -3, -7};
    std::cout << "\nPrinting initialized list_int: ";
    print(list_int);
    std::cout << '\n';
    
    std::cout << "\nNow add new integers to list_int.\n";
    // std::cin >> list_int;
    f(list_int, int{1});
    std::cout << "\nNow list_int is: ";
    print(list_int);
    std::cout << '\n';
    pause();

    // with doubles
    my_list<double> list_real;
    std::cout << "\nEnter real numbers into list_real.\n";
    // std::cin >> list_real;
    f(list_real, double{1.0});
    std::cout << "\nNow list_real is: ";
    print(list_real);
    std::cout << '\n';
    pause();

    // with chars - doesn't play nicely with std::cin
    my_list<char> list_char;
    std::cout << "\nEnter characters into list_char.\n";
    //std::cin >> list_char;
    f(list_char, char{'?'});
    std::cout << "\nNow list_char is: ";
    print(list_char);
    std::cout << '\n';
    pause();


    // with Fractions
    my_list<Fraction> list_rational;
    std::cout << "\nEnter Fractions into list_rational.\n";
    //std::cin >> list_rational;
    f(list_rational, Fraction(1, 1));
    std::cout << "\nNow list_rational is: ";
    print(list_rational);
    std::cout << '\n';
    pause();

    // with strings
    my_list<std::string> list_string;
    std::cout << "\nEnter strings into list_string.\n";
    //std::cin >> list_string;
    f(list_string, std::string{""});
    std::cout << "\nNow list_string is: ";
    print(list_string);
    std::cout << '\n';
    pause();

}
catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << '\n';
}
catch (...) {
    std::cerr << "exception\n";
}