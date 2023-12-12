#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define allOnes 0xFFFF
#define allZeros 0x0000

typedef struct {
    uint16_t positive;
    uint16_t negative;
} RB;

void printBitString(uint16_t string) {
    char value;
    printf("0b");
    for (uint8_t i = 0; i < 16; i++) {
        if ((string << i) & 0x8000) {
            value = '1';
        } else {
            value = '0';
        }
        printf("%c", value);
    }
    printf(" (%d)", (int)string);
}

void print8BitString(uint8_t string) {
    char value;
    printf("0b");
    for (uint8_t i = 0; i < 8; i++) {
        if ((string << i) & 0x80) {
            value = '1';
        } else {
            value = '0';
        }
        printf("%c", value);
    }
    printf(" (%d)", (int)string);
}

void printPartials(RB* partials, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("partials[%d].positive: ", (int)i);
        printBitString(partials[i].positive);
        printf("\npartials[%d].negative: ", (int)i);
        printBitString(partials[i].negative);
        printf(" %d - %d = %d\n\n", (int)(partials[i].positive), (int)(partials[i].negative), (int)(partials[i].positive - partials[i].negative));
    }
}

RB add(RB first, RB second) {
    RB carry;
    RB sum;

    //RB adder stage 1

    sum.positive = ((first.negative << 1) | (second.negative << 1)) 
        & (first.positive ^ first.negative ^ second.positive ^ second.negative);
    sum.negative = (first.positive ^ first.negative ^ second.positive ^ second.negative) 
        & ~((first.negative << 1) | (second.negative << 1));
    carry.positive = (first.positive & second.positive) 
        | ((first.positive ^ second.positive) 
            & ~(first.negative | second.negative) 
            & ~((first.negative << 1) | (second.negative << 1)));
    carry.negative = (((first.negative << 1) | (second.negative << 1)) 
            & ~(first.positive | second.positive) & (first.negative ^ second.negative))
         | (first.negative & second.negative);

    RB result;

    //RB adder stage 2

    result.positive = (sum.positive | (carry.positive << 1)) & ~(sum.negative) & ~(carry.negative << 1);
    result.negative = (sum.negative | (carry.negative << 1)) & ~(sum.positive) & ~(carry.positive << 1);

    return result;
}

int main(int argc, char** argv) {

    if (argc != 3) {
        printf("Usage: ./build-c-example [multiplicand] [multiplier]\n");
        return 0;
    }

    int8_t num1 = atoi(argv[1]);
    uint8_t num2 = atoi(argv[2]); //it is a uint so right shifts can be logical

    RB partials[8];

    for (size_t i = 0; i < 8; i++) {
        uint16_t num2_expanded = ((num2 >> i) & 0x01) ? allOnes : allZeros;

        if (i < 7) {
            partials[i].negative = ((num1 << i) & num2_expanded) & (0x0080 << i);
            partials[i].positive = ((num1 << i) & num2_expanded) & (0x007F << i);
        } else {
            partials[i].positive = ((num1 << i) & num2_expanded) & (0x0080 << i);
            partials[i].negative = ((num1 << i) & num2_expanded) & (0x007F << i);
        }
    }

    printf("partials round 1\n");
    printPartials(partials, 8);
    printf("\n");

    RB partials_round2[4];

    for (uint8_t i = 0; i < 4; i++) {
        partials_round2[i] = add(partials[i << 1], partials[(i << 1) | 1]);
    }

    printf("partials round 2\n");
    printPartials(partials_round2, 4);
    printf("\n");

    RB partials_round3[2];

    for (uint8_t i = 0; i < 2; i++) {
        partials_round3[i] = add(partials_round2[i << 1], partials_round2[(i << 1) | 1]);
    }

    printf("partials round 3: \n");
    printPartials(partials_round3, 2);
    printf("\n");

    RB partial_final = add(partials_round3[0], partials_round3[1]);

    printf("partials final positive: ");
    printBitString(partial_final.positive);
    printf("\n");
    printf("partials final negative: ");
    printBitString(partial_final.negative);
    printf("\n\n");

    int16_t final = partial_final.positive - partial_final.negative;

    printf("Final result %d - %d = %d\n", partial_final.positive, partial_final.negative, final);
}