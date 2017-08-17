#ifndef M_STRING_H
#define M_STRING_H

#include "m_type.h"

/*
* Converts all of the characters in this String to lower.
*
* Parameters:
*   src - The string should be converted.
*   dest  - The lowercase string.
*   len - The length of result string.Must be "strlen(src) + 1"
* Returns:
*   The string, converted to lowercase,or NULL for fail.
*/
void*
str_tolower
(
    const void *src,
    void *dest,
    size_t len
);

/*
* Converts all of the characters in this String to upper case.
*
* Parameters:
*   src - The string should be converted.
*   dest  - The uppercase string.
*   len - The length of result string.Must be "strlen(str_ptr) + 1"
* Returns:
*   The string, converted to uppercase.or NULL for fail.
*/
void*
str_toupper
(
    const void *src,
    void *dest,
    size_t len
);

/*
* Returns the index within this string of the first occurrence of the specified substring.
* If no such substring, then -1 is returned.
*
* Parameters:
*   str   -   The substring to search from.
*   sub_str - The substring to search for.
* Returns:
*   The index of the first occurrence of the specified substring,
*   or -1 if there is no such occurrence.
*/
ssize_t
str_indexof
(
    const void* str,
    const void* sub_str
);

/*
* Returns true if and only if this string contains the specified sequence of char values.
*
* Parameters:
*   str   -   The substring to search from.
*   sub_str - The substring to search for.
* Returns:
*   True if str contains sub_str, false otherwise.
*/
bool
str_contains
(
    const void* str,
    const void* sub_str
);

/*
* Tests if this string starts with the specified prefix.
*
* Parameters:
*   str       The substring to search from.
*   prefix    The prefix.
* Returns:
*   TRUE if the character sequence represented by the argument is a prefix of
*   the character sequence represented by this string; FALSE otherwise.
*   Note also that TRUE will be returned if the argument is an empty string or
*   is equal to this String.
*/
bool
str_startwith
(
    const void* str,
    const void* prefix
);

/*
* This same as str_startwith().The only difference is ignoring case.
*/
bool
str_startwith_ignorecase
(
    const void* str,
    const void* prefix
);


/*
* Tests if this string ends with the specified suffix.
*
* Parameters:
*   str       The substring to search from.
*   suffix    The suffix.
* Returns:
*   TRUE if the character sequence represented by the argument is a suffix of
*   the character sequence represented by this string; FALSE otherwise.
*   Note also that TRUE will be returned if the argument is an empty string or
*   is equal to this String.
*/
bool
str_endwith
(
    const void* str,
    const void* suffix
);


/*
* Returns a new string that is a substring of this string. The substring begins
* at the specified beginIndex and extends to the character at index endIndex - 1.
* Thus the length of the substring is endIndex-beginIndex.
*
* Examples:
*   "hamburger".substring(4, 8) returns "urge"
*   "smiles".substring(1, 5) returns "mile"
*
* Parameters:
*   begin_index     The beginning index, inclusive.
*   end_index       The ending index, exclusive.
* Returns:
*   The specified substring or NULL.
*/
void*
str_substring
(
    const void* str,
    size_t begin_index,
    size_t end_index,
    void *sub_str,
    size_t len
);

/*
* Return the count of sub_str appear in str.
*
* Parameters:
*   str - The string search from.
*   sub_str - The string search for.
* Returns:
*   The count of sub_str appear in str.
*/
size_t
str_contains_count
(
    const void* str,
    const void* sub_str
);

/*
*
* Replaces each substring of this string that matches the literal target sequence with the
* specified literal replacement sequence. The replacement proceeds from the beginning of the
* string to the end, for example, replacing "aa" with "b" in the string "aaa" will result
* in "ba" rather than "ab".
*
* Parameters:
*   str - The string.
*   target - The sequence of char values to be replaced
*   replacement - The replacement sequence of char values
* Returns:
*   The resulting string.
*/
void*
str_replace
(
    const void* src,
    const void* target,
    const void* replacement,
    void *dest,
    size_t len
);

/*
* Remove the head and tail spaces.
*/
void*
str_trim
(
    const void* str,
    void *result,
    size_t len
);

size_t
str_array_len(const char** str);

/**
 * Exchange string to number.
 *
 * Return TRUE if success.
 */
bool
str_atoi
(
    const void *str,
    ssize_t *result
);

/*
* Split string as char in regex.For example:
* str : "abcadcca"
* regex : "bd"
* Result : {"a","ca","cca"}
*/
char**
str_split_by_char
(
    void *str,
    const void *regex
);

/*
* Splits this string around matches of the given regular expression.
* This method works as if by invoking the two-argument split method with the given expression and a limit
* argument of zero. Trailing empty strings are therefore not included in the resulting array.
* The string "boo:and:foo", for example, yields the following results with these expressions:
*           Regex	       Result
*               :	{ "boo", "and", "foo" }
*               o	{ "b", "", ":and:f", "", "" }
* Parameters:
*   str   - The string.
*   regex - The delimiting regular expression.
* Returns:
*   The array of strings computed by splitting this string around matches of the given regular expression
*/
char**
str_split_by_string
(
    const void *str,
    const void *regex
);

#endif /* M_STRING_H */
