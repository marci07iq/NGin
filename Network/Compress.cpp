///
/// @file
/// @author Julius Pettersson
/// @copyright MIT/Expat License.
/// @brief LZW file compressor
/// @version 5
/// @remark This version borrows heavily from Juha Nieminen's work.
///
/// This is the C++11 implementation of a Lempel-Ziv-Welch single-file command-line compressor.
/// It uses the simpler fixed-width code compression method.
/// It was written with Doxygen comments.
///
/// @see http://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Welch
/// @see http://marknelson.us/2011/11/08/lzw-revisited/
/// @see http://www.cs.duke.edu/csed/curious/compression/lzw.html
/// @see http://warp.povusers.org/EfficientLZW/index.html
/// @see http://en.cppreference.com/
/// @see http://www.doxygen.org/
///

#include "Compress.h"

/// Type used to store and retrieve codes.
using CodeType = uint16_t;

namespace globals {

/// Dictionary Maximum Size (when reached, the dictionary will be reset)
  const CodeType dms{ numeric_limits<CodeType>::max() };

} // namespace globals

///
/// @brief Encoder's custom dictionary type.
///
class EncoderDictionary {

    ///
    /// @brief Binary search tree node.
    ///
  struct Node {

      ///
      /// @brief Default constructor.
      /// @param c    byte that the Node will contain
      ///
    explicit Node(char c) : first(globals::dms), c(c), left(globals::dms), right(globals::dms) {
    }

    CodeType    first;  ///< Code of first child string.
    char        c;      ///< Byte.
    CodeType    left;   ///< Code of child node with byte < `c`.
    CodeType    right;  ///< Code of child node with byte > `c`.
  };

public:

    ///
    /// @brief Default constructor.
    /// @details It builds the `initials` cheat sheet.
    ///
  EncoderDictionary() {
    const long int minc = numeric_limits<char>::min();
    const long int maxc = numeric_limits<char>::max();
    CodeType k{ 0 };

    for (long int c = minc; c <= maxc; ++c)
      initials[static_cast<unsigned char> (c)] = k++;

    vn.reserve(globals::dms);
    reset();
  }

  ///
  /// @brief Resets dictionary to its initial contents.
/// @see `EncoderDictionary::EncoderDictionary()`
  ///
  void reset() {
    vn.clear();

    const long int minc = numeric_limits<char>::min();
    const long int maxc = numeric_limits<char>::max();

    for (long int c = minc; c <= maxc; ++c)
      vn.push_back(Node(c));
  }

  ///
  /// @brief Searches for a pair (`i`, `c`) and inserts the pair if it wasn't found.
  /// @param i                code to search for
  /// @param c                attached byte to search for
  /// @returns The index of the pair, if it was found.
  /// @retval globals::dms    if the pair wasn't found
  ///
  CodeType search_and_insert(CodeType i, char c) {
      // dictionary's maximum size was reached
    if (vn.size() == globals::dms)
      reset();

    if (i == globals::dms)
      return search_initials(c);

    const CodeType vn_size = vn.size();
    CodeType ci{ vn[i].first }; // Current Index

    if (ci != globals::dms) {
      while (true)
        if (c < vn[ci].c) {
          if (vn[ci].left == globals::dms) {
            vn[ci].left = vn_size;
            break;
          } else
            ci = vn[ci].left;
        } else
          if (c > vn[ci].c) {
            if (vn[ci].right == globals::dms) {
              vn[ci].right = vn_size;
              break;
            } else
              ci = vn[ci].right;
          } else // c == vn[ci].c
            return ci;
    } else
      vn[i].first = vn_size;

    vn.push_back(Node(c));
    return globals::dms;
  }

  ///
  /// @brief Fakes a search for byte `c` in the one-byte area of the dictionary.
  /// @param c    byte to search for
  /// @returns The code associated to the searched byte.
  ///
  CodeType search_initials(char c) const {
    return initials[static_cast<unsigned char> (c)];
  }

private:

    /// Vector of nodes on top of which the binary search tree is implemented.
  vector<Node> vn;

  /// Cheat sheet for mapping one-byte strings to their codes.
  array<CodeType, 1u << CHAR_BIT> initials;
};

///
/// @brief Compresses the contents of `is` and writes the result to `os`.
/// @param [in] is      input stream
/// @param [out] os     output stream
///
void compress(DataPair* in, DataPair** out) {
  EncoderDictionary ed;
  CodeType i{ globals::dms }; // Index
  char c;

  list<CodeType> res;

  for (size_t it = 0; it < in->_len; it++) {
    c = in->_data[it];
    const CodeType temp{ i };

    if ((i = ed.search_and_insert(temp, in->_data[it])) == globals::dms) {
      res.push_back(temp);
      i = ed.search_initials(c);
    }
  }

  if (i != globals::dms)
    res.push_back(i);

  *out = new DataPair(res.size() * sizeof(CodeType) + PACKET_HEADER_LEN);

  Packet_Header_Convertor conv;
  conv.i = in->_len;

  for (int lit = 0; lit < PACKET_HEADER_LEN; lit++) {
    (*out)->_data[lit] = conv.chararr.chars[lit];
  }

  int itnum = 0;
  for (auto&& lit : res) {
    *(reinterpret_cast<CodeType*>((*out)->_data + PACKET_HEADER_LEN + itnum*sizeof(CodeType))) = lit;
    ++itnum;
  }
}

///
/// @brief Decompresses the contents of `is` and writes the result to `os`.
/// @param [in] is      input stream
/// @param [out] os     output stream
///
void decompress(DataPair* in, DataPair** out) {
  vector<pair<CodeType, char>> dictionary;

  Packet_Header_Convertor conv;
  for (int lit = 0; lit < PACKET_HEADER_LEN; lit++) {
    conv.chararr.chars[lit] = in->_data[lit];
  }

  int oit = 0;

  *out = new DataPair(conv.i);

  // "named" lambda function, used to reset the dictionary to its initial contents
  const auto reset_dictionary = [&dictionary] {
    dictionary.clear();
    dictionary.reserve(globals::dms);

    const long int minc = numeric_limits<char>::min();
    const long int maxc = numeric_limits<char>::max();

    for (long int c = minc; c <= maxc; ++c)
      dictionary.push_back({ globals::dms, static_cast<char> (c) });
  };

  const auto rebuild_string = [&dictionary](CodeType k) -> const vector<char> * {
    static vector<char> s; // String

    s.clear();

    // the length of a string cannot exceed the dictionary's number of entries
    s.reserve(globals::dms);

    while (k != globals::dms) {
      s.push_back(dictionary[k].second);
      k = dictionary[k].first;
    }

    reverse(s.begin(), s.end());
    return &s;
  };

  reset_dictionary();

  CodeType i{ globals::dms }; // Index
  CodeType k; // Key

  int dit = PACKET_HEADER_LEN;
  while (dit < in->_len) {
    k = *reinterpret_cast<CodeType*>(in->_data + dit);
    // dictionary's maximum size was reached
    if (dictionary.size() == globals::dms)
      reset_dictionary();

    if (k > dictionary.size())
      throw runtime_error("invalid compressed code");

    const vector<char> *s; // String

    if (k == dictionary.size()) {
      dictionary.push_back({ i, rebuild_string(i)->front() });
      s = rebuild_string(k);
    } else {
      s = rebuild_string(k);

      if (i != globals::dms)
        dictionary.push_back({ i, s->front() });
    }

    for (size_t rit = 0; rit < s->size(); ++rit) {
      (*out)->_data[oit] = (*s)[rit];
      ++oit;
    }

    i = k;
    dit += sizeof(CodeType);
  }

  if (oit != conv.i) {
    cout << "Corrupt decrypt data!" << endl;
    throw 1;
  }
}