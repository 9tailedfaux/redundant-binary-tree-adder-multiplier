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
        printf("\n\n");
    }
}

RB add(RB first, RB second) {
    RB carry;
    // bool nextLowestNeitherNegative = true;
    RB sum;

    // carry.positive = 0;
    // carry.negative = 0;
    // sum.positive = 0;
    // sum.negative = 0;

    sum.positive = ((first.negative << 1) | (second.negative << 1)) & (first.positive ^ first.negative ^ second.positive ^ second.negative);
    sum.negative = (first.positive ^ first.negative ^ second.positive ^ second.negative) & ~((first.negative << 1) | (second.negative << 1));
    carry.positive = (first.positive & second.positive) | ((first.positive ^ second.positive) & ~(first.negative | second.negative) & ~((first.negative << 1) | (second.negative << 1)));
    carry.negative = (((first.negative << 1) | (second.negative << 1)) & ~(first.positive | second.positive) & (first.negative ^ second.negative)) | (first.negative & second.negative);

    // for (uint8_t i = 0; i < 16; i++) {
    //     int8_t firstVal = ((first.positive >> i) & 1) - ((first.negative >> i) & 1);
    //     int8_t secondVal = ((second.positive >> i) & 1) - ((second.negative >> i) & 1);
    //     int8_t val = firstVal + secondVal;

    //     //printf("%d\n", (int)val);

    //     switch (val) {
    //         case -2: 
    //             carry.negative |= (1 << i);
    //             nextLowestNeitherNegative = false;
    //             break;
    //         case -1:
    //             if (nextLowestNeitherNegative) {
    //                 sum.negative |= (1 << i);
    //             } else {
    //                 carry.negative |= (1 << i);
    //                 sum.positive |= (1 << i);
    //             }
    //             nextLowestNeitherNegative = false;
    //             break;
    //         case 0:
    //             if (firstVal < 0 || secondVal < 0) {
    //                 nextLowestNeitherNegative = false;
    //             } else {
    //                 nextLowestNeitherNegative = true;
    //             }
    //             break;
    //         case 1:
    //             if (nextLowestNeitherNegative) {
    //                 carry.positive |= (1 << i);
    //                 sum.negative |= (1 << i);
    //             } else {
    //                 sum.positive |= (1 << i);
    //             }
    //             nextLowestNeitherNegative = true;
    //             break;
    //         case 2:
    //             carry.positive |= (1 << i);
    //             nextLowestNeitherNegative = true;
    //             break;
    //     }
    // }

    // printf("sum positive: ");
    // printBitString(sum.positive);
    // printf("\n");
    // printf("sum negative: ");
    // printBitString(sum.negative);
    // printf("\n");
    // printf("carry positive: ");
    // printBitString(carry.positive);
    // printf("\n");
    // printf("carry negative: ");
    // printBitString(carry.negative);
    // printf("\n\n");

    RB result;

    result.positive = (sum.positive | (carry.positive << 1)) & ~(sum.negative) & ~(carry.negative << 1);// & (carry.negative << 1);
    result.negative = (sum.negative | (carry.negative << 1)) & ~(sum.positive) & ~(carry.positive << 1);// & (carry.positive << 1);

    return result;
}

int main(int argc, char** argv) {

    if (argc != 3) {
        printf("Usage: ./build-c-example [multiplicand] [multiplier]\n");
        return 0;
    }

    int8_t num1 = atoi(argv[1]);
    uint8_t num2 = atoi(argv[2]);

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
        partials_round2[i] = add(partials[i * 2], partials[(i * 2) + 1]);
    }

    printf("partials round 2\n");
    printPartials(partials_round2, 4);
    printf("\n");

    RB partials_round3[2];

    for (uint8_t i = 0; i < 2; i++) {
        partials_round3[i] = add(partials_round2[i * 2], partials_round2[(i * 2) + 1]);
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