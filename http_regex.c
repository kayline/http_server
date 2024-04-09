#include <regex.h>
#include <stdio.h>

int main() {
	int result;
	regex_t get_regex;
	regmatch_t get_matches[2];
	char *example_request = "GET /test HTTP/1.1";

	regcomp(&get_regex, "GET /([^ ]*) HTTP/1", REG_EXTENDED);

	result = regexec(&get_regex, example_request, 2, get_matches, 0);

	printf("match outcome: %d\n", result);
}

