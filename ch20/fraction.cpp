// September 2019: Making std::cin more "verbose"
// November 2018:
// Added rat_0_1 to approximate decimal (between 0 and 1) to fraction
// to assist constructor Fraction::Fraction(double)

#include "fraction.h"
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <cmath>

Fraction::Fraction(Fraction const &src)
{
     num = src.num;
     den = src.den;
}

Fraction::Fraction(double decimal)  {
  double d = decimal - floor(decimal);  // 3.333 - 3 = 0.3333; -3.333 - (-4) = 0.6667
  Fraction f = rat_0_1(d, MAX_DENOMINATOR);
                                                          // 1/3 + 3 = 10/3
  Fraction g = f.add(Fraction(std::floor(decimal), 1));   // 2/3 - 4 = -10/3
  num = g.num;
  den = g.den;
}

Fraction::Fraction(std::string fractString)
{
     int divPos = fractString.find("/");
     char numString[80] = ""; //Quick patch, make the array big enough (hopefully)
     char denString[80] = "";

     strcpy(numString, fractString.substr(0, divPos).c_str());
     num = atol(numString);

     if (divPos == -1)
     {
        den = 1;
     }

     else
     {
        strcpy(denString, fractString.substr(divPos + 1).c_str());
        den = atol(denString);
     }
}

void Fraction::normalize()
{
     if (den == 0 || num == 0)
     {
          num = 0;
          den = 1;
     }

     if (den < 0)
     {
          num *= -1;
          den *= -1;
     }

     long n = gcf(num, den);
     num = num/n;
     den = den/n;
}

long Fraction::gcf(long a, long b)
{
    if (a % b == 0)
       return abs(b);
    else
       return gcf(b, a % b);
}

long Fraction::lcm(long a, long b)
{
    return (a / gcf(a, b)) * b;
}

Fraction Fraction::rat_0_1(double x, long N)   {
     Fraction rat;
     long a = 0;
     long b = 1;
     long c = 1;
     long d = 1;
     double mediant;

     while ( (b <= N) && (d <= N) )  {
         mediant = (double)((double)a + (double)c)/((double)b + (double)d);
         if (std::fabs(x - mediant) < tol)   {
             if (b + d <= N)  {
               rat.set(a + c, b + d);
               rat.normalize();
               return rat;
             }
             else {
               if (d > b) {
                 rat.set(c, d);
                 rat.normalize();
                 return rat;
               }
               else {
                 rat.set(a, b);
                 rat.normalize();
                 return rat;
               }
             }
        }
         else {
           if  (x - mediant > 0.0) {
              a += c;
              b += d;
           }
           else {
              c += a;
              d += b;
           }
         }
     }
     if (b > N) {
       rat.set(c, d);
       rat.normalize();
       return rat;
     }
     else  {
       rat.set(a, b);
       rat.normalize();
       return rat;
     }

}

Fraction Fraction::add(const Fraction &other)
{
    Fraction fract;
    long lcd = lcm(den, other.den);
    long quot1 = lcd/den;
    long quot2 = lcd/other.den;
    fract.set(num * quot1 + other.num * quot2, lcd);
    fract.normalize();
    return fract;
}

Fraction Fraction::sub(const Fraction &other)
{
    Fraction fract;
    //Fraction negativeFrac(-1/1)
    long lcd = lcm(den, other.den);
    long quot1 = lcd/den;
    long quot2 = lcd/other.den;
    fract.set(num * quot1 + -other.num * quot2, lcd);
    fract.normalize();
    return fract;
}

Fraction Fraction::mult(const Fraction &other)
{
    Fraction fract;
    fract.set(num * other.num, den * other.den);
    return fract;
}

Fraction Fraction::div(const Fraction &other)
{
    Fraction fract;
    fract.set(num * other.den, den * other.num);
    return fract;
}


//long Fraction::operator==(const Fraction &other)
//{
//    return (num == other.num && den == other.den);
//}

std::ostream &operator<<(std::ostream &os, const Fraction &fr)
{
    if (fr.num == 0) os << 0;
    else {
      if (fr.den == 1) os << fr.num;  // prints 2 instead of 2/1
      else os << fr.num << "/" << fr.den;
    }
    return os;
}

// Note about rat_0_1:
// code section to convert decimal between 0 and 1 to fraction
// Idea:
//      INPUT:  double
//      RETURN: Fraction  to be used as constructor in Fraction class

//  In Fraction::Fraction(double decimal), take decimal
//  double d = decimal - floor(decimal)
//  d = 0.333 = 3.333 - 3
//  Feed d to this function rat_0_1(double, int), where int is largest denominator
//
// Fraction::Fraction(double decimal)  {
//  double d = decimal - floor(decimal);  // 3.333 - 3 = 0.3333; -3.333 - (-4) = 0.6667
//  Fraction f = rat_0_1(d, MAX_DENOMINATOR);
//                                                          // 1/3 + 3 = 10/3
//  Fraction g = f.add(Fraction(std::floor(decimal), 1));   // 2/3 - 4 = -10/3
//  num = g.num;
//  den = g.den;
// }

// Explanation of rat_0_1:
// N: largest acceptable denominator
/******************************************************************
 * The idea is to start with two fractions, a/b = 0/1 and c/d = 1/1.
 * We update either a/b or c/d at each step so that a/b will be the
 * best lower bound of x with denominator no bigger than b,
 * and c/d will be the best upper bound with denominator no bigger
 * than d.
 *
 * At each step we do a sort of binary search by introducing
 * the mediant of the upper and lower bounds.
 * The mediant of a/b and c/d is the fraction (a+c)/(b+d)
 * which always lies between a/b and c/d.
******************************************************************/


Fraction Fraction::operator+=(const Fraction &r)
{
	num = r.num * den + r.den * num;
	den = r.den*den;
	normalize();
	return *this;
}


Fraction Fraction::operator-=(const Fraction &r)
{
	num = num * r.den - den * r.num;
	den = r.den*den;
	normalize();
	return *this;
}


Fraction Fraction::operator*=(const Fraction &r)
{
	num = r.num*num;
	den = r.den * den;
	normalize();
	return *this;
}


Fraction Fraction::operator/=(const Fraction &r)
{
	if (r.num == 0)
		throw "Cannot divide by zero";

	num = num * r.den;
	den = den * r.num;
   normalize();
	return *this;
}


Fraction Fraction::operator -()const
{
	Fraction t(-num,den);
	return t;
}


Fraction& Fraction::operator =(const Fraction& r)
{
	if (this == &r)
		return *this;
	num = r.num;
	den = r.den;
	return *this;
}


int Fraction::less (const Fraction &s)const
{
	long num1 = num * s.den;
	long num2 = den * s.num;
	return(num1 < num2);
}


int Fraction::equal (const Fraction &s)const
{
	if (num != s.num)
		return 0;
	return(den == s.den);
}


int Fraction::lessEqual (const Fraction &s)const
{
	return less(s)||equal(s);
}


int Fraction::greater (const Fraction &s)const
{
	return !lessEqual(s);
}


int Fraction::greaterEqual (const Fraction &s)const
{
	return !less(s);
}


int Fraction::notEqual (const Fraction &s)const
{
	return !equal(s);
}


int operator < (const Fraction &r, const Fraction & s)
{
	return r.less(s);
}


int operator == (const Fraction &r, const Fraction & s)
{
	return r.equal(s);
}


int operator <= (const Fraction &r, const Fraction & s)
{
	return r.lessEqual(s);
}


int operator > (const Fraction &r, const Fraction & s)
{
	return (r.greater(s));
}


int operator >= (const Fraction &r, const Fraction & s)
{
	return r.greaterEqual(s);
}


int operator != (const Fraction &r, const Fraction & s)
{
	return r.notEqual(s);
}


std::istream& operator >>(std::istream &is, Fraction &fr)
{
	return fr.read(is);
}

std::istream& Fraction::read(std::istream &is)
{
  std::cout << "\nEnter Rational Number as fraction a/b,\n"
            << " where a and b are Integers (Z/Z) separated by '/'\n"
            << " or enter single Integer (Z) : ";
	num =0; den = 1;
	long mult = 1;
	char line[80];
	int i = 0;
	is >> line;
	if (line[0] == '-')
	{
		i++;
		mult *= -1;
	}

	if (line[0] == '+')
		i++;

	while ( (line[i]) && (line[i] != '/') )
	{
		if (line[i] >= '0' && line[i] <= '9')
		{
			num = 10*num + (line[i] - '0');
			i++;
		}
		else
			throw "You must type a number" ;
	}
	if (line[i] == '/')
	{
		i++;
		den = 0;
		if (line[i] == '-')
		{
			mult *= -1;
			i++;
		}
	}
	while (line[i])
	{
		if (line[i]>= '0' && line[i] <='9')
		{
			den = 10*den + (line[i] - '0');
			i++;
		}
		else
			throw "You must type a number";
	}
	if(den == 0)
	{
		throw "Denominator of a fraction can not be 0";
	  //	den = 1;
	}
	else
	  normalize();
	num *= mult;
	return is;
}
