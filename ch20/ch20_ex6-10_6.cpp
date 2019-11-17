/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *       G E N E R I C   P R O G R A M M I N G
 *
 *    Programming: Principles and Practice Using C++ by Bjarne Stroustrup
 *
 *  Chapter 20: Containers and Iterators
 *
 *  Example Simple Text Editor  - Version 6
 *   *** renamed members of class Text_iterator
 *   *** from 5: changed print(Document& d) to
 *          std::ostream& operator<<(std::ostream& os, Document& d)
 *
 *  Exercise 6 : Write a find and replace operation for Document
 *                 based on 20.6.2.
 *
 *  Exercise 8: define a function that counts the number of characters
 *               in a Document.
 *
 *  Exercise 9: count words in a Document. Two versions,
 *              (1) one with "whitespace-separated sequences of characters"
 *              as words; (2) one with "sequence of consecutive alphabetic
 *              characters" as words.
 *
 *  Exercise 10: word count program where user can specifiy the set of
 *              whitespace characters
 *
 */

#include <iostream>
#include <stdexcept>
#include <vector>
#include <list>
#include <string>
#include <sstream>
// This version does not use nor support std::find
// (no need for #include <algorithm>)
#include <fstream>  // for ifstream

//typedef std::vector<char> Line;  // the old way of doing this
using Line = std::vector<char>;    // a line is a vector of characters

// keep track of line and character position within a line

class Text_iterator {
    std::list<Line>::iterator line_pos;    // ln
    Line::iterator char_pos;               // pos

public:
    // start the iterator at line lp's character position cp:
    Text_iterator(std::list<Line>::iterator lp, Line::iterator cp)
        :line_pos(lp), char_pos(cp) { }

    std::list<Line>::iterator get_line()
           { return line_pos; }

    Line::iterator get_ch_pos()
           { return char_pos; }

    char& operator*() { return *char_pos; }
    Text_iterator& operator++();

    bool operator==(const Text_iterator& other) const
        {
          return (line_pos==other.line_pos
                 && char_pos==other.char_pos);
        }

    bool operator!=(const Text_iterator& other) const
        { return !(*this==other); }
};





Text_iterator& Text_iterator::operator++()
{
    ++char_pos;            // proceed to next character
    if (char_pos==(*line_pos).end()) {
        ++line_pos;             // proceed to next line
        // bad if line_pos==line.end(); so make sure it isn't
        char_pos = (*line_pos).begin();
    }
    return *this;
}

template<class Iter>
bool match(Iter first, Iter last, const std::string& s)
{
    std::string::const_iterator str_itr = s.begin();
    while ( first != last  &&  str_itr != s.end() ) {
        if (*first != *str_itr) return false;
        ++first;
        ++str_itr;
    }
    return true;
}

// This is a home-grown version of std::find
template<class Iter, class T>
Iter find(Iter first, Iter last, const T& val)
{
    Iter p = first;
    while (p != last) {
        if (*p == val) return p;
        ++p;
    }
    return p;
}

Text_iterator find_txt(Text_iterator first, Text_iterator last, \
                       const std::string& s)
{
    if (s.size() == 0) return last;   // can't find an empty string
    char first_char = s[0];
    while (true) {
        // not using std::find
        Text_iterator p = ::find(first, last, first_char);
        if ( p == last  ||  match(p, last, s) ) return p;
        first = ++p;    // look at the next character
    }
}

template<class Iter>
void advance(Iter& p, int n)
{
    if (n > 0) {
        while (n > 0) { ++p; --n; }
    }
    else if (n < 0) {
        while (n < 0) { --p; ++n; }
    }
}


struct Document {
    std::list<Line> line;    // a document is a list of lines
    Document() { line.push_back(Line{}); }

    Text_iterator begin()   // first character of first line
        { return Text_iterator(line.begin(),(*line.begin()).begin()); }
    Text_iterator end()     // one beyond the last character of the last line
    {
        std::list<Line>::iterator last = line.end();
        --last;             // we know that the document is not empty
        return Text_iterator(last, (*last).end());
    }

    Text_iterator erase(Text_iterator position);
    Text_iterator insert(Text_iterator position, const char& ch);

    // move next line to end of current line
    void concat(Text_iterator& position);

     // move rest of line to beginning of next
    void break_line(Text_iterator position);

    void find_replace(Text_iterator first, Text_iterator last,
            const std::string& find_str, const std::string& replace_str);
};

void erase_line(Document& d, unsigned long n)
{
    if ( n < 0 || (d.line.size() - 1) <= n ) return;
    std::list<Line>::iterator p = d.line.begin();
    advance(p, n);
    d.line.erase(p);
}

Text_iterator Document::erase(Text_iterator position)
{
    std::list<Line>::iterator list_itr = position.get_line();
    Line::iterator line_itr = position.get_ch_pos();
    line_itr = (*list_itr).erase(line_itr);
    return Text_iterator(list_itr, line_itr);
}

Text_iterator Document::insert(Text_iterator position, const char& ch)
{
    std::list<Line>::iterator list_itr = position.get_line();
    Line::iterator line_itr = position.get_ch_pos();
    line_itr = (*list_itr).insert(line_itr, ch);
    return Text_iterator(list_itr, line_itr);
}

// insert contents of next line to current line, erase next line
// pass-by-reference so position can be reset after invalidating
// because of resizing
void Document::concat(Text_iterator& position)
{
    std::list<Line>::iterator current_line = position.get_line();
    Line::iterator line_pos = position.get_ch_pos();
    Line::size_type index = line_pos - (*current_line).begin();
    std::list<Line>::iterator next_line = current_line;
    ++next_line;
    (*current_line).insert((*current_line).end(), (*next_line).begin(), \
                       (*next_line).end() );
    // reset iterator
    position = Text_iterator(current_line, \
                             (*current_line).begin() + index );

    // find number of line to erase, erase it
    std::list<Line>::iterator list_itr = begin().get_line();
    int line_ctr = 0;
    while (list_itr != next_line) {
        ++list_itr;
        ++line_ctr;
    }
    erase_line(*this, line_ctr);
}

// move rest of line to beginning of next line
void Document::break_line(Text_iterator position)
{
    std::list<Line>::iterator current_line = position.get_line();
    std::list<Line>::iterator next_line = current_line;
    ++next_line;
    (*next_line).insert((*next_line).begin(),position.get_ch_pos()+1, \
                        (*current_line).end() );

    (*current_line).erase(position.get_ch_pos()+1, \
                          (*current_line).end() );
}

void Document::find_replace(Text_iterator first,
                            Text_iterator last,
                            const std::string& find_str,
                            const std::string& replace_str)
{
    if (find_str == "") return;   // replace empty string - do nothing
    Text_iterator pos = find_txt(first, last, find_str);
    while (pos != last) {
        std::string::const_iterator find_itr = find_str.begin();
        std::string::const_iterator replace_itr = replace_str.begin();
        while ( find_itr != find_str.end()
                && replace_itr != replace_str.end() )
        { 
            if (*find_itr != *replace_itr) {
                *pos = *replace_itr;
                if (*find_itr=='\n') {   // deleted '\n', append next line
                    concat(pos);
                }
                if (*replace_itr == '\n') break_line(pos);  // inserted '\n', add line break
            }
            ++pos;
            ++find_itr;
            ++replace_itr;
        }

        // delete characters from find string in case it was longer
        while (find_itr != find_str.end()) {
            if (*pos=='\n') concat(pos);
            pos = erase(pos);
            ++find_itr;
        }

        // insert characters from replace string in case it was longer
        while (replace_itr != replace_str.end()) {
            pos = insert(pos,*replace_itr);
            if (*replace_itr=='\n') break_line(pos);
            ++pos;
            ++replace_itr;
        }
        pos = find_txt(pos, last, find_str);    // search next occurrence
    }
}

std::istream& operator>>(std::istream& is, Document& d)
{
    char ch;

    while (is.get(ch)) {
        d.line.back().push_back(ch);    // add the character
        if (ch=='\n')
            d.line.push_back(Line{});   // add another line
    }
    if (d.line.back().size()) d.line.push_back(Line{}); // add final empty line
    return is;
}

std::ostream& operator<<(std::ostream& os, Document& d)
{
    for (auto p : d) os << p;
    return os;
}

// exercise 8
int char_count(Document& d)
{
    Text_iterator txt_itr = d.begin();
    int ctr = 0;
    while (txt_itr != d.end()) {
        ++txt_itr;
        ++ctr;
    }
    return ctr;
}

// error() simply disguises throws:
inline void error(const std::string& s)
{
	throw std::runtime_error(s);
}

inline void error(const std::string& s, const std::string& s2)
{
	error(s + s2);
}

inline void error(const std::string& s, int i)
{
	std::ostringstream os;
	os << s <<": " << i;
	error(os.str());
}

// count number of words in file
int file_word_count(const std::string& fname)
{
    std::ifstream ifs(fname.c_str());
    if (!ifs) error("can't open file ",fname);
    std::string s;
    int ctr = 0;
    while (ifs>>s) ++ctr;
    return ctr;
}

// exercise 9a: count whitespace-separated words. To take advantage of the
// fact that reading from an istream into a string is using whitespace to
// separate strings, first print document to file, then read from file using
// the >> operator
int word_count1(Document& d)
{
    // write Document to file
    Text_iterator p = d.begin();
    std::string ofname = "ch20_ex06_out.txt";
    std::ofstream ofs(ofname.c_str());
    if (!ofs) error("can't open file ", ofname);
    while (p!=d.end()) {
        ofs << *p;
        ++p;
    }
    ofs.close();

    int ctr = file_word_count(ofname);

    // delete file and return value
    remove(ofname.c_str());
    return ctr;
}

// exercise 9b: count words defined as "sequence of alphabetic characters"
// do the same as word_count1, but when printing the temp file, replace any
// non-alphabetic character with a space
int word_count2(Document& d)
{
    // write Document to file
    Text_iterator p = d.begin();
    std::string ofname = "ch20_ex06_out.txt";
    std::ofstream ofs(ofname.c_str());
    if (!ofs) error("can't open no goddang file at ", ofname);
    while (p != d.end()) {
        if (isalpha(*p)) ofs << *p;
        else ofs << ' ';
        ++p;
    }
    ofs.close();

    int ctr = file_word_count(ofname);

    // delete file and return value
    remove(ofname.c_str());
    return ctr;
}

// true if c is contained in white
bool is_whitespace(char c, const std::string& white)
{
    for (unsigned long i = 0; i < white.size(); ++i) {
        if (white[i] == c) return true;
    }
    return false;
}

// exercise 10: count whitespace-separated words, where whitespace is user
// defined (the user can add to the standard whitespace characters)
int word_count3(Document& d, const std::string& white)
{
    // write Document to file
    Text_iterator p = d.begin();
    std::string ofname = "ch20_ex06_out.txt";
    std::ofstream ofs(ofname.c_str());
    if (!ofs) error("can't open file ", ofname);
    while (p!=d.end()) {
        if (is_whitespace(*p,white))
            ofs << ' ';
        else
            ofs << *p;
        ++p;
    }
    ofs.close();

    int ctr = file_word_count(ofname);

    // delete file and return value
    remove(ofname.c_str());
    return ctr;
}





int main()
try {
    Document d_is;
    std::istringstream iss {"\nThis is a different way to enter lines.\n \
Mike is a symbolic name for { Mike }.\n Programming is like merging \
with the machine/human hybrid.\n"};
    iss >> d_is;
    std::cout << "\nFrom istringstream iss >> d_is = \n" << d_is;
    //print(d_is);

    // Ex 6 find & replace
    // replace with longer
    std::cout << "\nReplacing Mike with Hentrich in Document d.\n";
    d_is.find_replace(d_is.begin(), d_is.end(), "Mike", "Hentrich");
    std::cout << "\nUsing ostream& os, d_is = \n" << d_is;
   // print(d_is);

    // replace with shorter
    std::cout << "\nReplacing machine/human hybrid with\n"
              << " parahumanoidarianized in Document d.\n";
    d_is.find_replace(d_is.begin(), d_is.end(), "machine/human hybrid.",
                 "parahumanoidarianized.");

    std::cout << "\nUsing ostream& os, d_is = \n" << d_is;
    // print(d_is);

    std::cout << "\nNumber of characters in this document: "
              << char_count(d_is) << "\n";

    std::cout << "Number of words (whitespace-separated) in this document: "
              << word_count1(d_is) << "\n";

    std::cout << "Number of words (sequences of alphabetic characters) in this "
              << "document: " << word_count2(d_is) << "\n";

    std::cout << "Number of words separated by whitespace or \".!'\" in this "
             << "document: " << word_count3(d_is,".!'")
             << "\n_____________________________________________\n";

    std::string ifname = "ch20_ex06.txt";
    std::ifstream ifs(ifname.c_str());
    if (!ifs) error("Unable to open no goddamn ", ifname);
    Document d_if;
    ifs >> d_if;
    //print(d_if);
    std::cout << "\nd_if = \n" << d_if;
    std::cout << "\n\nReplace computer\\nalgebra systems. with "
              << "abstract\\nalgebra.\n\n";
    std::string f_str = " computer\nalgebra systems. ";
    std::string r_str = " abstract\nalgebra. ";
    d_if.find_replace(d_if.begin(), d_if.end(), f_str, r_str);

    std::cout << "\nCODEMODE TRANCE!\nNow d2 = \n\n" << d_if;
    // print(d_if);

     std::cout << "\nNumber of characters in this document: "
        << char_count(d_if) << "\n";

    std::cout << "Number of words (whitespace-separated) in this document: "
        << word_count1(d_if) << "\n";

    std::cout << "Number of words (sequences of alphabetic characters) in this "
        << "document: " << word_count2(d_if) << "\n";

    std::cout << "Number of words separated by whitespace or \".!'\" in this "
        << "document: " << word_count3(d_if,".!'") << "\n";

    std::istringstream iss2 {"For the past four months I have been going \
through many mathematics\ntextbooks, especially those that have incorporated \
the use of computer programming.\nI have now become curious about \
\"algorithm-oriented\"\ngeneric programming, mostly due to being inspired \
by the work of Alexander Stepanov\n(generic programming as a mathematical \
discipline).\n\nMy own interests are in using phenomenological methods of\n\
detachment to help induce codemode trance."};

    Document d2;

    iss2 >> d2;
    std::cout << "d2 = \n" << d2;
    // print(d2);

    std::cout << "\n\nReplace \"four months\" with \"three years\"\n"
              << " (same length, no \\n in either):\n\n";
    f_str = "four months";
    r_str = "three years";
    d2.find_replace(d2.begin(), d2.end(),f_str, r_str);

    std::cout << "\nCODEMODE TRANCE!\nNow d2 = \n" << d2;
    // print(d2);

    std::cout << "\n\nReplace \"algorithm-oriented\"\\ngeneric programming, "
              << "\nwith generic\\n\"algorithm-oriented\" programming, "
              << "\n (same length, \\n in same place):\n\n";
    f_str = "\"algorithm-oriented\"\ngeneric programming, ";
    r_str = "generic\n \"algorithm-oriented\" programming, ";
    d2.find_replace(d2.begin(), d2.end(), f_str, r_str);
    std::cout << "\nCODEMODE TRANCE!\nNow d2 =\n" << d2;
    //print(d2);

    std::cout << "\n\nReplace methods of\\ndetachment to help induce with "
              << "detachment\\nto induce\n"
              << " (same length, \\n is removed?):\n\n";
    f_str = "methods of\ndetachment to help induce";
    r_str = "detachment\nto induce";
    d2.find_replace(d2.begin(), d2.end(), f_str, r_str);

    // std::cout << "\nUsing print(d2); :\n\n";
    // print(d2);
    std::cout << "\nCODEMODE TRANCE!\nNow d2 =\n" << d2;

     std::cout << "\nNumber of characters in this document: "
        << char_count(d2) << "\n";

    std::cout << "Number of words (whitespace-separated) in this document: "
        << word_count1(d2) << "\n";

    std::cout << "Number of words (sequences of alphabetic characters) in this "
        << "document: " << word_count2(d2) << "\n";

    std::cout << "Number of words separated by whitespace or \".!'\" in this "
        << "document: " << word_count3(d2,".!'") << "\n";
}
catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << std::endl;
}
catch (...) {
    std::cerr << "exception\n";
}
