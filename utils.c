#define max(a, b) ((a) > (b) ? (a) : (b))

#define arr_len(arr) (sizeof(arr) / sizeof(arr[0]))

char *append(char *array, char a) {
	size_t len = strlen(array);
	char *ret = malloc(len + 2);

	strcpy(ret, array);
	ret[len] = a;
	ret[len + 1] = '\0';

	return ret;
}