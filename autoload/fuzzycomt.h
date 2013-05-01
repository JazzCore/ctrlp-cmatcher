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

#include <boost/python.hpp>
#include <string>

typedef struct
{
  std::string str;           // Python object with file path
  double score;              // score of string
} Match;

typedef struct
{
  std::string str;           // string to be searched
  long str_len;              // length of same
  std::string query;         // query string
  long query_len;            // length of same
  double max_score_per_char;
  int is_dot_file;           // boolean: true if str is a dot-file
} MatchObject;

double CalculateMatchScore(
  MatchObject *m,            // sharable meta-data
  long str_index,            // where in the path string to start
  long query_index,          // where in the search string to start
  long last_matched_index,   // location of last matched character
  double score);             // cumulative score so far

bool CompareMatchScore(Match a_val, Match b_val);

boost::python::list FilterAndSortMultiEncodedCandidates(
  const boost::python::list &candidates,
  const std::string &query );
