/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *    Programming: Principles and Practice Using C++ by Bjarne Stroustrup
 *
 *    Chapter 20: Containers and Iterators  Exercise 07: 
 *                Find the lexicographical last string in an unsorted
 *               vector<string>. Beware that "lexicographical" is not
 *               the same as "alphabetical", as "Zzzz" < "aaa"
 *       (uppercase letters are lexicographically before lowercase letters).
 */

 #include <iostream>
 #include <stdexcept>
 #include <string>
 #include <vector>
 #include<sstream>

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

// returns an iterator to the biggest element of the argument
template<class Iter>
Iter biggest_element(Iter first, Iter last)
{
    Iter high = first;
    while (first != last) {
        if (*first > *high) high = first;
        std::cout << "\n*high = " << *high;
        ++first;
        std::cout << "\n*first = " << *first;
    }
    return high; 
}

int main()
try {
    std::vector<std::string> vs;
    std::string s;
    std::cout << "\n[Ctrl-D (Linux), Ctrl-Z (Windows) to end]"
              << "\n\nEnter a few words: ";
    while (std::cin >> s)
        vs.push_back(s);

    std::vector<std::string>::iterator  \
     last = biggest_element(vs.begin(),vs.end());
    if (last != vs.end())
        std::cout << "The lexicographically last string in the vector is \""
                  << *last << "\".\n\n";
    else
        std::cout << "Something went wrong.\n";
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