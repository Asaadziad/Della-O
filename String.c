#include "String.h"
#include "Memory.h"

U32 GetStringLength(const char* str) {
	if (str == NULL) return 0;
	U32 length = 0;

	for (;*str; str++, length++);

	return length;
}

static S8 SafeStringCompare(const U8* first, const U8* second, U32 first_length, U32 second_length) {
	
	U32 min_length = first_length > second_length ? second_length : first_length;
	/*
		look for the first unmatching character and return :
		 1  - if the ascii value of first greater than the ascii value of second
		-1  - if the ascii value of second greater than the ascii value of first
		 0  - if it never got to a distinguisehd character 
	*/
	for (int i = 0; i < min_length; i++) {
		if (first[i] - second[i] > 0) return 1;
		if (first[i] - second[i] < 0) return -1;
	}

	/* if we got here meaning one of the strings is a substring */
	if (first_length > second_length) return 1; // if the second string is a substring of the first
	if (first_length < second_length) return -1;  // if the first string is a substring of the second


	/* if we got here the two strings are identical */
	return 0;
}

S8 StringCompare(const U8* first, const U8* second) {
	if (first == NULL) return -1;
	if (second == NULL) return 1;

	U32 first_length   = GetStringLength(first);
	U32 second_length  = GetStringLength(second);

	
	return SafeStringCompare(first, second, first_length, second_length);
}

