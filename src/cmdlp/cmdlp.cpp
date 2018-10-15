/**
   This command-line option library was cloned from:

   https://github.com/masaers/cmdlp (v0.1 tag, plus our small patch)

   Thanks Markus!
   Consider cloning the original repository if you like it.

   Copyright (c) 2018 Markus S. Saers
 */
#include "cmdlp.h"

cmdlp::parser::~parser() {
  using namespace std;
  for (auto it = options_m.begin(); it != options_m.end(); ++it) {
    delete *it;
    *it = NULL;
  }
}

std::string cmdlp::parser::help() const {
  using namespace std;
  ostringstream s;
  for (auto it = options_m.begin(); it != options_m.end(); ++it) {
    auto jt = bindings_m.find(*it);
    if (jt != bindings_m.end()) {
      print_call(s, jt->second.first, jt->second.second, true);
    } else {
      s << "n/n";
    }
    s << '=';
    (**it).evaluate(s);
    s << endl;
    (**it).describe(s);
    s << endl;
  }
  return s.str();
}

bool cmdlp::parser::bind(option* opt, const char flag) {
  auto p = flags_m.insert(std::make_pair(flag, opt));
  if (! p.second) {
    std::ostringstream s;
    s << "Failed to bind option to flag '-" << flag << "'. "
      << "It already exists.";
    exit(1);
  } else {
    bindings_m[opt].second.push_back(flag);
  }
  return p.second;
}

bool cmdlp::parser::bind(option* opt, const std::string& name) {
  auto p = names_m.insert(std::make_pair(name, opt));
  if (! p.second) {
    std::ostringstream s;
    s << "Failed to bind option to name '--" << name << "'. "
      << "It already exists.";
    exit(1);
  } else {
    bindings_m[opt].first.push_back(name);
  }
  return p.second;
}

void cmdlp::parser::print_call(std::ostream& s, const std::vector<std::string>& names, std::vector<char> flags, bool print_all) {
  using namespace std;
  for (auto it = names.begin(); it != names.end(); ++it) {
    if (it != names.begin()) {
      s << "|";
    }
    const auto pos = find(flags.begin(), flags.end(), it->at(0));
    if (pos != flags.end()) {
      flags.erase(pos);
      s << "-[-" << it->at(0) << "]" << it->substr(1);
    } else {
      s << "--" << *it;
    }
  }
  for (auto it = flags.begin(); it != flags.end(); ++it) {
    if (names.size() != 0 || it != flags.begin()) {
      s << "|";
    }
    s << "-" << *it;
  }
}

