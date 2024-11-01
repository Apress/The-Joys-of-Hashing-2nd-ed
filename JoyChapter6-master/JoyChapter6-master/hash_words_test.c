#include "hash_words.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

const char *walrus = \
"The Walrus And The Carpenter" \
"" \
"The sun was shining on the sea," \
"Shining with all his might:" \
"He did his very best to make" \
"The billows smooth and bright—" \
"And this was odd, because it was" \
"The middle of the night." \
"" \
"The moon was shining sulkily," \
"Because she thought the sun" \
"Had got no business to be there" \
"After the day was done—" \
"'It's very rude of him,' she said," \
"'To come and spoil the fun!'" \
"" \
"The sea was wet as wet could be," \
"The sands were dry as dry." \
"You could not see a cloud, because" \
"No cloud was in the sky:" \
"No birds were flying overhead—" \
"There were no birds to fly." \
"" \
"The Walrus and the Carpenter" \
"Were walking close at hand;" \
"They wept like anything to see" \
"Such quantities of sand:" \
"'If this were only cleared away,'" \
"They said, 'it would be grand!'" \
"" \
"'If seven maids with seven mops" \
"Swept it for half a year." \
"Do you suppose,' the Walrus said," \
"'That they could get it clear?'" \
"'I doubt it,' said the Carpenter," \
"And shed a bitter tear." \
"" \
"'O Oysters, come and walk with us!'" \
"The Walrus did beseech." \
"'A pleasant walk, a pleasant talk," \
"Along the briny beach:" \
"We cannot do with more than four," \
"To give a hand to each.'" \
"" \
"The eldest Oyster looked at him," \
"But never a word he said:" \
"The eldest Oyster winked his eye," \
"And shook his heavy head—" \
"Meaning to say he did not choose" \
"To leave the oyster-bed." \
"" \
"But four young Oysters hurried up," \
"All eager for the treat:" \
"Their coats were brushed, their faces washed," \
"Their shoes were clean and neat—" \
"And this was odd, because, you know," \
"They hadn't any feet." \
"" \
"Four other Oysters followed them," \
"And yet another four;" \
"And thick and fast they came at last," \
"And more, and more, and more—" \
"All hopping through the frothy waves," \
"And scrambling to the shore." \
"" \
"The Walrus and the Carpenter" \
"Walked on a mile or so," \
"And then they rested on a rock" \
"Conveniently low:" \
"And all the little Oysters stood" \
"And waited in a row." \
"" \
"'The time has come,' the Walrus said," \
"'To talk of many things:" \
"Of shoes—and ships—and sealing-wax—" \
"Of cabbages—and kings—" \
"And why the sea is boiling hot—" \
"And whether pigs have wings.'" \
"" \
"'But wait a bit,' the Oysters cried," \
"'Before we have our chat;" \
"For some of us are out of breath," \
"And all of us are fat!'" \
"'No hurry!' said the Carpenter." \
"They thanked him much for that." \
"" \
"'A loaf of bread,' the Walrus said," \
"'Is what we chiefly need:" \
"Pepper and vinegar besides" \
"Are very good indeed—" \
"Now if you're ready, Oysters dear," \
"We can begin to feed.'" \
"" \
"'But not on us!' the Oysters cried," \
"Turning a little blue." \
"'After such kindness, that would be" \
"A dismal thing to do!'" \
"'The night is fine,' the Walrus said." \
"'Do you admire the view?" \
"" \
"'It was so kind of you to come!" \
"And you are very nice!'" \
"The Carpenter said nothing but" \
"'Cut us another slice:" \
"I wish you were not quite so deaf—" \
"I've had to ask you twice!'" \
"" \
"'It seems a shame,' the Walrus said," \
"'To play them such a trick," \
"After we've brought them out so far," \
"And made them trot so quick!'" \
"The Carpenter said nothing but" \
"'The butter's spread too thick!'" \
"" \
"'I weep for you,' the Walrus said:" \
"'I deeply sympathize.'" \
"With sobs and tears he sorted out" \
"Those of the largest size," \
"Holding his pocket-handkerchief" \
"Before his streaming eyes." \
"" \
"'O Oysters,' said the Carpenter," \
"'You've had a pleasant run!" \
"Shall we be trotting home again?'" \
"But answer came there none—" \
"And this was scarcely odd, because" \
"They'd eaten every one." ;

void print_distribution(int mask_size, int prime_size,
                        uint32_t keys[], uint32_t no_keys)
{
    int mask_table[mask_size];
    for (int i = 0; i < mask_size; ++i) {
        mask_table[i] = 0;
    }
    
    int prime_table[prime_size];
    for (int i = 0; i < prime_size; ++i) {
        prime_table[i] = 0;
    }
    
    int bitmask = mask_size - 1;
    for (int i = 0; i < no_keys; ++i) {
        mask_table[keys[i] & bitmask]++;
        prime_table[keys[i] % prime_size]++;
    }
    
    for (int i = 0; i < mask_size; ++i) {
        printf("mask %d %d %d\n", mask_size, i, mask_table[i]);
    }
    for (int i = 0; i < prime_size; ++i) {
        printf("prime %d %d %d\n", prime_size, i, prime_table[i]);
    }
}


void extract_words(char **words, int *no_words) {
    char *str = strdup(walrus);
    char* word = strtok(str, " ");
    int i = 0;
    while (word != NULL && i < *no_words) {
        printf("%s\n", word);
        words[i++] = strdup(word);
        word = strtok(NULL, " ");
    }
    *no_words = i;
    free(str);
}


int main(int argc, const char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s hash-function\n",
               argv[0]);
        return EXIT_FAILURE;
    }
    
    const char *hash_name = argv[1];
    uint32_t (*hash_function)(uint32_t state, char *val, int len) = 0;
    if (strcmp(hash_name, "additive") == 0) {
        hash_function = additive_hash;
    } else if (strcmp(hash_name, "rotating") == 0) {
        hash_function = rotating_hash;
    } else if (strcmp(hash_name, "one_at_a_time") == 0) {
        hash_function = one_at_a_time_hash;
    } else if (strcmp(hash_name, "jenkins") == 0) {
        hash_function = jenkins_hash;
    } else {
        printf("Unknown hash function.\n");
        return EXIT_FAILURE;
    }
    
    int no_keys = 627;
    char **keys = malloc(no_keys * sizeof *keys);
    extract_words(keys, &no_keys);

    printf("Hashing %d keys with %s\n", no_keys, hash_name);
    
    uint32_t hash_keys[no_keys];
    for (uint32_t i = 0; i < no_keys; ++i) {
        hash_keys[i] = 0;
    }
    for (uint32_t i = 0; i < no_keys; ++i) {
        hash_keys[i] = hash_function(0, keys[i], strlen(keys[i]));
    }
    
    print_distribution(8, 7, hash_keys, sizeof(hash_keys)/sizeof(uint32_t));
    print_distribution(16, 17, hash_keys, sizeof(hash_keys)/sizeof(uint32_t));
    print_distribution(32, 31, hash_keys, sizeof(hash_keys)/sizeof(uint32_t));
    print_distribution(64, 67, hash_keys, sizeof(hash_keys)/sizeof(uint32_t));
    
    free(keys);
    
    return EXIT_SUCCESS;
}
