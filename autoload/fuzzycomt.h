// Copyright 2013 Stanislav Golovanov <stgolovanov@gmail.com>
// Matching algorithm by Wincent Colaiuta, 2010.
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

#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>



typedef enum mmode {
    fullLine,
    filenameOnly,
    firstNonTab,
    untilLastTab
} mmode_t;

typedef struct {
    PyObject *str;                       // Python object with file path
    double  score;                       // score of string
} matchobj_t;

typedef struct {
    char    *haystack_p;                 // pointer to string to be searched
    long    haystack_len;                // length of same
    char    *needle_p;                   // pointer to search string (abbreviation)
    long    needle_len;                  // length of same
    double  max_score_per_char;
    int     dot_file;                    // boolean: true if str is a dot-file
    double  *memo;                       // memoization
} matchinfo_t;

matchobj_t ctrlp_find_match(PyObject* str, PyObject* abbrev, mmode_t mmode);

void ctrlp_get_line_matches(PyObject* paths, PyObject* abbrev, matchobj_t matches[], char *mode);

PyObject* ctrlp_fuzzycomt_match(PyObject* self, PyObject* args);

PyObject* ctrlp_fuzzycomt_sorted_match_list(PyObject* self, PyObject* args);
