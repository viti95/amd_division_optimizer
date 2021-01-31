/* 
* Program to determine algorithm, multiplier, and shift factor to be   
* used to accomplish signed division by a constant divisor. 
* Compile with GCC.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t log2(uint32_t i)
{
    uint32_t t = 0;
    i = i >> 1;
    while (i)
    {
        i = i >> 1;
        t++;
    }
    return (t);
}

long e;
uint32_t res1, res2;
uint32_t oa, os, om;
uint32_t d, l, s, m, a, r, t;
uint64_t m_low, m_high, j, k;

int main(void)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Signed division by constant\n");
    fprintf(stderr, "===========================\n\n");
    fprintf(stderr, "enter divisor: ");
    scanf("%ld", &d);
    fprintf(stderr, "\n");
    e = d;
    d = labs(d);
    if (d == 0)
        goto printed_code;
    if (e == (-1))
    {
        printf("; dividend: register or memory location\n");
        printf("\n");
        printf("NEG    dividend\n");
        printf("\n");
        printf("; quotient replaced dividend\n");
        goto printed_code;
    }
    if (d == 2)
    {
        printf("; dividend expected in EAX\n");
        printf("\n");
        printf("CMP    EAX, 080000000h\n");
        printf("SBB    EAX, -1\n");
        printf("SAR    EAX, 1\n");
        if (e < 0)
            printf("NEG    EAX\n");
        printf("\n");
        printf("; quotient now in EAX\n");
        goto printed_code;
    }
    if (!(d & (d - 1)))
    {
        printf("; dividend expected in EAX\n");
        printf("\n");
        printf("CDQ\n");
        printf("AND    EDX, 0%08lXh\n", (d - 1));
        printf("ADD    EAX, EDX\n");
        if (log2(d))
            printf("SAR    EAX, %d\n", log2(d));
        if (e < 0)
            printf("NEG    EAX\n");
        printf("\n");
        printf("; quotient now in EAX\n");
        goto printed_code;
    }

    /* Determine algorithm (a), multiplier (m), and shift factor (s) for 32-bit      
    * signed integer division. Based on: Granlund, T.; Montgomery, P.L.:       
    * "Division by Invariant Integers using Multiplication". 
    * SIGPLAN Notices, Vol. 29, June 1994, page 61.   
    */

    l = log2(d);
    j = (((uint64_t)(0x80000000)) % ((uint64_t)(d)));
    k = (((uint64_t)(1)) << (32 + l)) / ((uint64_t)(0x80000000 - j));
    m_low = (((uint64_t)(1)) << (32 + l)) / d;
    m_high = ((((uint64_t)(1)) << (32 + l)) + k) / d;

    while (((m_low >> 1) < (m_high >> 1)) && (l > 0))
    {
        m_low = m_low >> 1;
        m_high = m_high >> 1;
        l = l - 1;
    }
    m = ((uint32_t)(m_high));
    s = l;
    a = (m_high >> 31) ? 1 : 0;
    if (a)
    {
        printf("; dividend: memory location or register other than EAX or EDX\n");
        printf("\n");
        printf("MOV    EAX, 0%08LXh\n", m);
        printf("IMUL   dividend\n");
        printf("MOV    EAX, dividend\n");
        printf("ADD    EDX, EAX\n");
        if (s)
            printf("SAR    EDX, %d\n", s);
        printf("SHR    EAX, 31\n");
        printf("ADD    EDX, EAX\n");
        if (e < 0)
            printf("NEG    EDX\n");
        printf("\n");
        printf("; quotient now in EDX\n");
    }
    else
    {
        printf("; dividend: memory location of register other than EAX or EDX\n");
        printf("\n");
        printf("MOV    EAX, 0%08LXh\n", m);
        printf("IMUL   dividend\n");
        printf("MOV    EAX, dividend\n");
        if (s)
            printf("SAR    EDX, %d\n", s);
        printf("SHR    EAX, 31\n");
        printf("ADD    EDX, EAX\n");
        if (e < 0)
            printf("NEG    EDX\n");
        printf("\n");
        printf("; quotient now in EDX\n");
    }
printed_code:
    fprintf(stderr, "\n");
    exit(0);
}