enum data_t {
	DATA_CHR, DATA_INT, DATA_FLT, DATA_STR
};
	// DATA_CHRV, DATA_INTV, DATA_FLTV, DATA_STRV

char *data_t_str = "DATA_CHRDATA_INTDATA_FLTDATA_STR";

struct ht_item {
	char *identifier;
	size_t size;
	enum data_t type;
	void *next;
};

struct hashtable {
	size_t size;
	char count;
	struct ht_item **items;
};

struct hashtable *new_ht(size_t s);
void rm_ht(struct hashtable *ht);
void rm_ht_helper(struct ht_item *item);

struct ht_item *new_ht_item(const char *identifier, size_t size, enum data_t type);
char rm_ht_item(struct hashtable *ht, char *str);

unsigned long hash(const char *str);
void hash_item(struct hashtable *ht, struct ht_item *item);
struct ht_item *lookup_item(struct hashtable *ht, char *label);
