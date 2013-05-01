// Copyright 2010 Wincent Colaiuta. All rights reserved.
// Additional work by Stanislav Golovanov, 2013.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
#define BOOST_PYTHON_STATIC_LIB
#include "fuzzycomt.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/foreach.hpp>
#include <vector>

#define foreach BOOST_FOREACH

using boost::algorithm::any_of;
using boost::algorithm::is_upper;
using boost::python::len;
using boost::python::extract;
using boost::python::object;
typedef boost::python::list pylist;

double CalculateMatchScore( MatchObject *m, long str_index, long query_index,
                            long last_matched_index, double score) {

  double seen_score = 0;      // remember best score seen via recursion
  int is_dot_file_match = 0;     // true if abbrev matches a dot-file
  int dot_search = 0;         // true if searching for a dot

  for (long i = query_index; i < m->query_len; i++)
  {
      char c = m->query[i];
      if (c == '.')
          dot_search = 1;
      int found = 0;
      for (long j = str_index; j < m->str_len; j++, str_index++)
      {
          char d = m->str[j];
          if (d == '.')
          {
              if (j == 0 || m->str[j - 1] == '/')
              {
                  m->is_dot_file = 1;        // this is a dot-file
                  if (dot_search)         // and we are searching for a dot
                      is_dot_file_match = 1; // so this must be a match
              }
          }
          else if (d >= 'A' && d <= 'Z' && !(c >= 'A' && c <= 'Z'))
              d += 'a' - 'A'; // add 32 to downcase
          if (c == d)
          {
              found = 1;
              dot_search = 0;

              // calculate score
              double score_for_char = m->max_score_per_char;
              long distance = j - last_matched_index;
              if (distance > 1)
              {
                  double factor = 1.0;
                  char last = m->str[j - 1];
                  char curr = m->str[j]; // case matters, so get again
                  if (last == '/')
                      factor = 0.9;
                  else if (last == '-' ||
                          last == '_' ||
                          last == ' ' ||
                          (last >= '0' && last <= '9'))
                      factor = 0.8;
                  else if (last >= 'a' && last <= 'z' &&
                          curr >= 'A' && curr <= 'Z')
                      factor = 0.8;
                  else if (last == '.')
                      factor = 0.7;
                  else
                      // if no "special" chars behind char, factor diminishes
                      // as distance from last matched char increases
                      factor = (1.0 / distance) * 0.75;
                  score_for_char *= factor;
              }

              if (++j < m->str_len)
              {
                  // bump cursor one char to the right and
                  // use recursion to try and find a better match
                  double sub_score = CalculateMatchScore(m, j, i,
                    last_matched_index, score);

                  if (sub_score > seen_score)
                      seen_score = sub_score;
              }

              score += score_for_char;
              last_matched_index = str_index++;
              break;
          }
      }
      if (!found)
          return 0.0;
  }
  return (score > seen_score) ? score : seen_score;
}


bool CompareMatchScore(Match a_val, Match b_val)
{
  return (a_val.score > b_val.score);
}


boost::python::list FilterAndSortMultiEncodedCandidates(
  const boost::python::list &candidates,
  const std::string &query ) {

  boost::python::list filtered_candidates;
  int num_candidates = len( candidates );
  std::vector< Match > matches;

  if ( query.empty() )
    return candidates;

  for ( int i = 0; i < num_candidates; i++ )
  {
    Match match;
    match.str = extract< std::string > ( candidates[ i ] );

    MatchObject m;
    m.str                = match.str;
    m.str_len            = match.str.length();
    m.query              = query;
    m.query_len          = query.length();
    m.max_score_per_char = ( 1.0 / m.str_len + 1.0 / m.query_len ) / 2;
    m.is_dot_file        = 0;

    match.score = CalculateMatchScore( &m, 0, 0, 0, 0.0 );

    if ( match.score > 0 )
      matches.push_back( match );
  }

  typedef bool ( *comparer_t )( Match, Match );
  comparer_t cmp = &CompareMatchScore;
  std::sort( matches.begin(), matches.end(), cmp );

  foreach ( const Match &match, matches ) {
    filtered_candidates.append( match.str );
  }

  return filtered_candidates;
}


BOOST_PYTHON_MODULE(fuzzycomt)
{
  def( "FilterAndSortCandidates", FilterAndSortMultiEncodedCandidates);
}


