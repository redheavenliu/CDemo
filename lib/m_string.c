#include "m_stdio.h"
#include <ctype.h>

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
)
{
    const char *s = (char*)src;
    char *d = (char*)dest;
    if(!s
        || !d
        || (strlen(d) + 1 > len)){
        return NULL;
    }

    char* temp = d;
    while(*s){
        *temp++ = tolower(*s++);
    }
    *temp = '\0';

    return dest;
}

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
)
{
    const char *s = (char*)src;
    char *d = (char*)dest;
    if(!s
        || !d
        || (strlen(d) + 1 > len)){
        return NULL;
    }
    char* temp = d;
    while(*s){
        *temp++ = toupper(*s++);
    }
    *temp = '\0';
    return dest;
}

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
)
{
    const char *s = (char*)str;
    const char *s_sub = (char*)sub_str;

    if(!s || !s_sub){
            return -1;
    }
    char* position = strstr(s,s_sub);
    if(!position){
        return -1;
    }else{
        return position - s;
    }
}

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
)
{
    const char *s = (char*)str;
    const char *s_sub = (char*)sub_str;

    if(!s || !s_sub){
        return FALSE;
    }

    return str_indexof(s,s_sub)==-1?FALSE:TRUE;
}

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
)
{
    const char *s = (char*)str;
    const char *s_prefix = (char*)prefix;

    if(!s || !s_prefix){
        return FALSE;
    }
    if(strlen(s) < strlen(s_prefix)){
        return FALSE;
    }

    return str_indexof(s,s_prefix)?FALSE:TRUE;
}

/*
* This same as str_startwith().The only difference is ignoring case.
*/
bool
str_startwith_ignorecase
(
    const void* str,
    const void* prefix
)
{
    const char *s = (char*)str;
    const char *s_prefix = (char*)prefix;
    if(!s || !s_prefix){
     return FALSE;
    }
    if(strlen(s) < strlen(s_prefix)){
     return FALSE;
    }

    char *str_lower = (char*)malloc(strlen(s) + 1);
    char *prefix_lower = (char*)malloc(strlen(s_prefix)+1);
    bool result = str_startwith(
        str_tolower(s,str_lower,strlen(s) + 1),
        str_tolower(s_prefix,prefix_lower,strlen(s_prefix)+1));
    if(str_lower){
        free(str_lower);
        str_lower = NULL;
    }
    if(prefix_lower){
        free(prefix_lower);
        prefix_lower = NULL;
    }
    return result;
}


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
)
{
    const char *s = (char*)str;
    const char *s_suffix = (char*)suffix;
    if(!s || !s_suffix){
        return FALSE;
    }
    if(strlen(s) < strlen(s_suffix)){
        return FALSE;
    }
    return str_indexof(s + strlen(s) - strlen(s_suffix),s_suffix)?FALSE:TRUE;
}


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
)
{
    const char* s = (char*)str;
    char *result = (char*)sub_str;
    if(!s
        || !result
        || begin_index < 0
        || begin_index >= end_index
        || begin_index >= strlen(s)
        || end_index - begin_index + 1 > len){
        return NULL;
    }

    if(end_index > strlen(s)){
        end_index = strlen(s);
    }

    memcpy(result,s + begin_index,end_index - begin_index);
    result[end_index - begin_index] = '\0';

    return result;
}

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
)
{
    const char* s = (char*)str;
    const char* s_sub = (char*)sub_str;
    if(!s || !strlen(s)){
        return 0;
    }
    if(!s_sub || !strlen(s_sub)){
        return 0;
    }
    int result = 0;
    int position = 0;
    int temp = 0;
    while(position < strlen(s) && (temp = str_indexof(s + position,s_sub)) > -1){
        result++;
        position += (temp + strlen(s_sub));
    }
    return result;
}

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
)
{
    const char *s = (char*)src;
    const char *s_target = (char*)target;
    const char *s_replacement = (char*)replacement;
    char *s_dest = (char*)dest;
    if(!s || !s_target || !s_replacement || !s_dest){
        return NULL;
    }
    if(!strlen(s) || !strlen(s_target)){
        return "";
    }
    int position_old = 0;
    int position_new = 0;
    int temp = 0;
    int length = strlen(s) + (strlen(s_replacement) - strlen(s_target)) * str_contains_count(s,s_target);
    while(position_old < strlen(s) && (temp = str_indexof(s + position_old,s_target)) > -1){
        memcpy(s_dest + position_new,s + position_old,temp);
        position_new += temp;
        memcpy(s_dest + position_new,s_replacement,strlen(s_replacement));
        position_new += strlen(s_replacement);
        position_old += (temp + strlen(s_target));
    }
    memcpy(s_dest + position_new,s + position_old,length - position_new);
    s_dest[length] = '\0';
    return s_dest;
}

/*
* Remove the head and tail spaces.
*/
void*
str_trim
(
    const void* str,
    void *result,
    size_t len
)
{
    const char *s = (char*)str;
    char *s_result = (char*)result;

    if(!s
        || !s_result
        || strlen(s) + 1 > len){
        return NULL;
    }

    if(!strlen(s)){
        return "";
    }

    char* result_tmp = (char*)malloc(sizeof(char) * (strlen(s) + 1));
    memcpy(result_tmp,s,strlen(s) + 1);
    char* tmp = result_tmp + strlen(result_tmp) - 1;
    while(*tmp == ' ' || *tmp == '\t' || *tmp == '\n' || *tmp == '\r'){
        *tmp = '\0';
        tmp--;
    }
    tmp = result_tmp;
    while(*tmp == ' ' || *tmp == '\t' || *tmp == '\n' || *tmp == '\r'){
        tmp++;
    }

    int i = tmp - result_tmp;
    memset(s_result,0,strlen(tmp) + 1);
    memcpy(s_result,result_tmp + i,strlen(tmp));
    free(result_tmp);
    result_tmp = NULL;
    return s_result;
}

size_t
str_array_len(const char** str)
{
    const char **s = str;
    if(s == NULL){
        return 0;
    }
    int count = 0;
    while(*(s+count)){
        count++;
    }
    return count;
}

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
)
{
    const char *ptr = (char*)str;
    while(*ptr){
        if(!isdigit(*ptr)){
            return FALSE;
        }
        ptr++;
    }
    *result = atoi((char*)str);
    return TRUE;
}

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
)
{
    char* s = (char*)str;
    const char* s_regex = (char*)regex;
    char* temp_buf = (char*)malloc(sizeof(char) * (strlen(s) + 1));
    memcpy(temp_buf,s,strlen(s) + 1);
    int i = 0;
    char* temp = strtok(s,s_regex);
    while(temp) {
       i++;
       temp = strtok(NULL,s_regex);
    }

    char** result = (char**)malloc(sizeof(char*) * (i + 1));
    i = 0;
    result[i] = strtok(temp_buf,s_regex);
    while(result[i++]) {
       result[i] = strtok(NULL,s_regex);
    }
    return result;
}

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
)
{
    const char *s = (char*)str;
    const char *s_regex = (char*)regex;
    if(!s || !strlen(s)){
        return NULL;
    }
    if(!s_regex|| !strlen(s_regex)){
        return NULL;
    }
    int count = str_contains_count(s,s_regex);
    if(count == 0){
        return NULL;
    }
    char** result = (char**)malloc(sizeof(char*) * (count + 2));
    int position_old = 0;
    int temp = 0;
    while(position_old < strlen(s) && (temp = str_indexof(s + position_old,s_regex)) > -1){
        char* temp_ptr = (char*)malloc(sizeof(char) * (temp + 1));
        memcpy(temp_ptr,s + position_old,temp);
        temp_ptr[temp] = '\0';
        *result++ = temp_ptr;
        position_old += (temp + strlen(s_regex));
    }
    char* temp_ptr = (char*)malloc(sizeof(char) * (strlen(s) - position_old + 1));
    memcpy(temp_ptr,s + position_old,strlen(s) - position_old);
    temp_ptr[strlen(s) - position_old] = '\0';
    *result++ = temp_ptr;

    *result = NULL;
    return result - count - 1;
}

