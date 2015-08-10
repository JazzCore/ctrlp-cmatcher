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

#include "float.h"
#include "fuzzycomt.h"

// Forward declaration for ctrlp_get_line_matches
matchobj_t ctrlp_find_match(PyObject* str, PyObject* abbrev, mmode_t mmode);

mmode_t getMMode(char *mmode) {
    mmode_t result = fullLine;
    if (mmode[0] == 'f') {
        if (mmode[1] == 'i') {
            if (mmode[2] == 'l') {
                result = filenameOnly;
            } else {
                result = firstNonTab;
            }
        } else {
            result = fullLine;
        }
    } else {
        result = untilLastTab;
    }

    return result;
}

void ctrlp_get_line_matches(PyObject* paths,
                            PyObject* abbrev,
                            matchobj_t matches[],
                            char *mmode)
{
    int i;
    int max;

    mmode_t mmodeEnum = getMMode(mmode);

    // iterate over lines and get match score for every line
    for (i = 0, max = PyList_Size(paths); i < max; i++) {
        PyObject* path = PyList_GetItem(paths, i);
        matches[i] = ctrlp_find_match(path, abbrev, mmodeEnum);
    }
}

char *strduplicate(const char *s) {
    char *d = malloc (strlen (s) + 1);
    if (d == NULL)
       return NULL;
    strcpy (d,s);
    return d;
}

char *slashsplit(char *line) {
    char *fname = line;
    char *scan = fname;
    while (scan != '\0')
    {   
        if (*scan == '/' || *scan == '\\') {
            fname = ++scan;
        } else {
            ++scan;
        }
    }

   return fname;
}

// comparison function for use with qsort
int ctrlp_comp_alpha(const void *a, const void *b) {
    matchobj_t a_val = *(matchobj_t *)a;
    matchobj_t b_val = *(matchobj_t *)b;

    char *a_p = PyString_AsString(a_val.str);
    long a_len = PyString_Size(a_val.str);
    char *b_p = PyString_AsString(b_val.str);
    long b_len = PyString_Size(b_val.str);

    int order = 0;
    if (a_len > b_len) {
        order = strncmp(a_p, b_p, b_len);
        if (order == 0)
            order = 1; // shorter string (b) wins
    }
    else if (a_len < b_len) {
        order = strncmp(a_p, b_p, a_len);
        if (order == 0)
            order = -1; // shorter string (a) wins
    }
    else {
        order = strncmp(a_p, b_p, a_len);
    }

    return order;
}

int ctrlp_comp_score_alpha(const void *a, const void *b) {
    matchobj_t a_val = *(matchobj_t *)a;
    matchobj_t b_val = *(matchobj_t *)b;
    double a_score = a_val.score;
    double b_score = b_val.score;
    if (a_score > b_score)
        return -1; // a scores higher, a should appear sooner
    else if (a_score < b_score)
        return 1;  // b scores higher, a should appear later
    else
        return ctrlp_comp_alpha(a, b);
}

double ctrlp_recursive_match(matchinfo_t *m,    // sharable meta-data
                       long haystack_idx, // where in the path string to start
                       long needle_idx,   // where in the needle string to start
                       long last_idx,     // location of last matched character
                       double score,      // cumulative score so far
                       mmode_t mmode)
{
    double seen_score = 0;  // remember best score seen via recursion
    long i, j, distance;
    int found;
    double score_for_char;
    long memo_idx = haystack_idx;

    // do we have a memoized result we can return?
    double memoized = m->memo[needle_idx * m->needle_len + memo_idx];
    if (memoized != DBL_MAX)
        return memoized;

    // bail early if not enough room (left) in haystack for (rest of) needle
    if (m->haystack_len - haystack_idx < m->needle_len - needle_idx) {
        score = 0.0;
        goto memoize;
    }

    for (i = needle_idx; i < m->needle_len; i++) {
        char c = m->needle_p[i];
        found = 0;

        // similar to above, we'll stop iterating when we know we're too close
        // to the end of the string to possibly match
        for (j = haystack_idx;
             j <= m->haystack_len - (m->needle_len - i);
             j++, haystack_idx++) {

            char d = m->haystack_p[j];
            if (d == '\t' && mmode == firstNonTab) {
                break;
            } else if (d == '.') {
                if (j == 0 || m->haystack_p[j - 1] == '/') {
                    m->dot_file = 1; // this is a dot-file
                }
            } else if (d >= 'A' && d <= 'Z') {
                d += 'a' - 'A'; // add 32 to downcase
            }

            if (c == d) {
                found = 1;

                // calculate score
                score_for_char = m->max_score_per_char;
                distance = j - last_idx;

                if (distance > 1) {
                    double factor = 1.0;
                    char last = m->haystack_p[j - 1];
                    char curr = m->haystack_p[j]; // case matters, so get again
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

                if (++j < m->haystack_len) {
                    // bump cursor one char to the right and
                    // use recursion to try and find a better match
                    double sub_score = ctrlp_recursive_match(m, j, i, last_idx, score, mmode);
                    if (sub_score > seen_score)
                        seen_score = sub_score;
                }

                score += score_for_char;
                last_idx = ++haystack_idx;
                break;
            }
        }

        if (!found) {
            score = 0.0;
            goto memoize;
        }
    }

    score = score > seen_score ? score : seen_score;

memoize:
    m->memo[needle_idx * m->needle_len + memo_idx] = score;
    return score;
}

PyObject* ctrlp_fuzzycomt_match(PyObject* self, PyObject* args) {
    PyObject *paths, *abbrev, *returnlist;
    Py_ssize_t limit;
    char *mmode;
    int i;
    int max;

    if (!PyArg_ParseTuple(args, "OOns", &paths, &abbrev, &limit, &mmode)) {
       return NULL;
    }
    returnlist = PyList_New(0);

    // Type checking
    if (PyList_Check(paths) != 1) {
        PyErr_SetString(PyExc_TypeError,"expected a list");
        return 0;
    }

    if (PyString_Check(abbrev) != 1) {
        PyErr_SetString(PyExc_TypeError,"expected a string");
        return 0;
    }

    matchobj_t matches[PyList_Size(paths)];

    if ( (limit > PyList_Size(paths)) || (limit == 0) ) {
        limit = PyList_Size(paths);
    }

    if ( PyString_Size(abbrev) == 0) {
        // if string is empty - just return first (:param limit) lines
        PyObject *initlist;

        initlist = PyList_GetSlice(paths,0,limit);
        return initlist;
    }
    else {
        // find matches and place them into matches array.
        ctrlp_get_line_matches(paths,abbrev, matches, mmode);

        // sort array of struct by struct.score key
        qsort(matches, PyList_Size(paths),
              sizeof(matchobj_t),
              ctrlp_comp_score_alpha);
    }

    for (i = 0, max = PyList_Size(paths); i < max; i++) {
            if (i == limit)
                break;
            // generate python dicts { 'line' : line, 'value' : value }
            // and place dicts to list
            PyObject *container;
            container = PyDict_New();
            // TODO it retuns non-encoded string. So cyrillic literals
            // arent properly showed.
            // There are PyString_AsDecodedObject, it works in interactive
            // session but it fails in Vim for some reason
            // (probably because we dont know what encoding vim returns)
            PyDict_SetItemString(container, "line", matches[i].str);
            PyDict_SetItemString(container,
                                 "value",
                                 PyFloat_FromDouble(matches[i].score));
            PyList_Append(returnlist,container);
    }

    return returnlist;
}

PyObject* ctrlp_fuzzycomt_sorted_match_list(PyObject* self, PyObject* args) {
    PyObject *paths, *abbrev, *returnlist;
    Py_ssize_t limit;
    char *mmode;
    int i;
    int max;

    if (!PyArg_ParseTuple(args, "OOns", &paths, &abbrev, &limit, &mmode)) {
       return NULL;
    }
    returnlist = PyList_New(0);

    // Type checking
    if (PyList_Check(paths) != 1) {
        PyErr_SetString(PyExc_TypeError,"expected a list");
        return 0;
    }

    if (PyString_Check(abbrev) != 1) {
        PyErr_SetString(PyExc_TypeError,"expected a string");
        return 0;
    }

    matchobj_t matches[PyList_Size(paths)];

    if ( (limit > PyList_Size(paths)) || (limit == 0) ) {
        limit = PyList_Size(paths);
    }

    if ( PyString_Size(abbrev) == 0) {
        // if string is empty - just return first (:param limit) lines
        PyObject *initlist;

        initlist = PyList_GetSlice(paths,0,limit);
        return initlist;
    }
    else {
        // find matches and place them into matches array.
        ctrlp_get_line_matches(paths,abbrev, matches, mmode);

        // sort array of struct by struct.score key
        qsort(matches,
              PyList_Size(paths),
              sizeof(matchobj_t),
              ctrlp_comp_score_alpha);
    }

    for (i = 0, max = PyList_Size(paths); i < max; i++) {
       if (i == limit)
          break;
        if ( matches[i].score> 0 ) {
            // TODO it retuns non-encoded string. So cyrillic literals
            // arent properly showed.
            // There are PyString_AsDecodedObject, it works in interactive
            // session but it fails in Vim for some reason
            // (probably because we dont know what encoding vim returns)
            PyList_Append(returnlist,matches[i].str);
        }
    }

    return returnlist;
}


matchobj_t ctrlp_find_match(PyObject* str, PyObject* abbrev, mmode_t mmode)
{
    long i, max;
    double score;
    matchobj_t returnobj;

    matchinfo_t m;
    if (mmode == filenameOnly) {
        // get file name by splitting string on slashes
        m.haystack_p = slashsplit(PyString_AsString(str));
        m.haystack_len = strlen(m.haystack_p);
    }
    else {
        m.haystack_p                 = PyString_AsString(str);
        m.haystack_len               = PyString_Size(str);
    }
    m.needle_p              = PyString_AsString(abbrev);
    m.needle_len            = PyString_Size(abbrev);
    m.max_score_per_char    = (1.0 / m.haystack_len + 1.0 / m.needle_len) / 2;
    m.dot_file              = 0;

    // calculate score
    score = 1.0;

    // special case for zero-length search string
    if (m.needle_len == 0) {

        // filter out dot files
        for (i = 0; i < m.haystack_len; i++) {
           char c = m.haystack_p[i];
           if (c == '.' && (i == 0 || m.haystack_p[i - 1] == '/')) {
               score = 0.0;
               break;
           }
        }
    } else if (m.haystack_len > 0) { // normal case

        // prepare for memoization
        double memo[m.haystack_len * m.needle_len];
        for (i = 0, max = m.haystack_len * m.needle_len; i < max; i++)
            memo[i] = DBL_MAX;
        m.memo = memo;

        score = ctrlp_recursive_match(&m, 0, 0, 0, 0.0, mmode);
    }

    returnobj.str = str;
    returnobj.score = score;

    return returnobj;
}

static PyMethodDef fuzzycomt_funcs[] = {
    {"match", (PyCFunction)ctrlp_fuzzycomt_match, METH_NOARGS, NULL},
    { "match", ctrlp_fuzzycomt_match, METH_VARARGS, NULL },
    {"sorted_match_list", (PyCFunction)ctrlp_fuzzycomt_sorted_match_list, METH_NOARGS, NULL},
    { "sorted_match_list", ctrlp_fuzzycomt_sorted_match_list, METH_VARARGS, NULL },
    {NULL}
};

PyMODINIT_FUNC initfuzzycomt()
{
    Py_InitModule3("fuzzycomt", fuzzycomt_funcs,
                   "Fuzzy matching module");
}
