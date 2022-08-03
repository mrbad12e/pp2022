//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef VEINS_INET_JUTE_H_
#define VEINS_INET_JUTE_H_

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>

namespace jute {
  enum jType {JSTRING, JOBJECT, JARRAY, JBOOLEAN, JNUMBER, JNULL, JUNKNOWN};
  class jValue {
    private:
      std::string makesp(int);
      std::string svalue;
      jType type;
      std::vector<std::pair<std::string, jValue> > properties;
      std::map<std::string, size_t> mpindex;
      std::vector<jValue> arr;
      std::string to_string_d(int);
    public:
      jValue();
      jValue(jType);
      std::string to_string();
      jType get_type();
      void set_type(jType);
      void add_property(std::string key, jValue v);
      void add_element(jValue v);
      void set_string(std::string s);
      int as_int();
      double as_double();
      bool as_bool();
      void* as_null();
      std::string as_string();
      int size();
      jValue operator[](int i);
      jValue operator[](std::string s);
  };

  class parser {
    private:
    enum token_type {UNKNOWN, STRING, NUMBER, CROUSH_OPEN, CROUSH_CLOSE, BRACKET_OPEN, BRACKET_CLOSE, COMMA, COLON, BOOLEAN, NUL};

    struct token;
    static bool is_whitespace(const char c);
    static int next_whitespace(const std::string& source, int i);
    static int skip_whitespaces(const std::string& source, int i);

    static std::vector<token> tokenize(std::string source);
    static jValue json_parse(std::vector<token> v, int i, int& r);
    public:
    static jValue parse(const std::string& str);
    static jValue parse_file(const std::string& str);
  };
}
#endif /* VEINS_INET_JUTE_H_ */
