/**
 * @author Jackie Lo
 * @file util.cpp utility
 *
 *
 * COMMENTS:
 *
 * The declaration of some utility functions and global constants.
 *
 */

#ifndef UTIL_HPP
#define UTIL_HPP
#include <iterator>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>
#include <functional>

/**
   The null_output_iterator was cloned from

   https://github.com/masaers/cmdlp

   Thanks Markus!
   Consider cloning the original repository if you like it.
 */

struct null_output_iterator
  : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
  template<typename T>
  inline const null_output_iterator& operator=(T&&) const { return *this; }
  inline const null_output_iterator& operator*() const { return *this; }
  inline const null_output_iterator& operator++() const { return *this; }
  inline const null_output_iterator& operator++(int) const { return *this; }
};

/**
   The assign_iterator was cloned from

   https://github.com/masaers/cmdlp

   Thanks Markus!
   Consider cloning the original repository if you like it.
 */

template<typename T>
class assign_iterator
  : public std::iterator<std::output_iterator_tag, T>
{
public:
  inline assign_iterator() : x_m(NULL) {}
  explicit inline assign_iterator(T& x) : x_m(&x) {}
  inline assign_iterator(const assign_iterator&) = default;
  inline T& operator*() const { return *x_m; }
  inline const assign_iterator& operator++() const { return *this; }
  inline const assign_iterator& operator++(int) const { return *this; }
private:
  T* x_m;
};

namespace yisi{
  std::vector<std::string> tokenize(std::string sent, char d=' ', bool keep_empty=false);
  std::string join(std::vector<std::string> tokens, std::string d=" ");
  std::vector<std::string> read_file(std::string filename);
  void open_ofstream(std::ofstream& fout, std::string filename);
  std::string lowercase(std::string token);
  std::pair<int, char**> str2charss(std::string str, char d=' ');
  static inline std::string lstrip(std::string s){
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
  }
  static inline std::string rstrip(std::string s){
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
  }
  static inline std::string strip(std::string s){
    auto ss = lstrip(rstrip(s));
    return ss;
  }
  static inline std::string debpe(std::string s){
    auto ss=s;
    auto bpe = ss.find("@@ ");
    while (bpe != std::string::npos){
      ss.erase(bpe, bpe+3);
      bpe = ss.find("@@ ");
    }
    return ss;
  }
  static const int REF_MODE = 0;
  static const int HYP_MODE = 1;
  static const int INP_MODE = 2;
}

#endif
