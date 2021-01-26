/*  
* Program to determine algorithm, multiplier, and shift factor to be   
* used to accomplish unsigned division by a constant divisor. 
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

uint32_t res1, res2;
uint32_t d, l, s, m, a, r, n, t;
uint64_t m_low, m_high, j, k;

int main(void)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Unsigned division by constant\n");
    fprintf(stderr, "=============================\n\n");
    fprintf(stderr, "enter divisor: ");
    scanf("%lu", &d);
    printf("\n");
    if (d == 0)
        goto printed_code;
    if (d >= 0x80000000UL)
    {
        printf("; dividend: register or memory location\n");
        printf("\n");
        printf("CMP    dividend, 0%08lXh\n", d);
        printf("MOV    EDX, 0\n");
        printf("SBB    EDX, -1\n");
        printf("\n");
        printf("; quotient now in EDX\n");
        goto printed_code;
    } /* Reduce divisor until it becomes odd */
    n = 0;
    t = d;
    while (!(t & 1))
    {
        t >>= 1;
        n++;
    }
    if (t == 1)
    {
        if (n == 0)
        {
            printf("; dividend: register or memory location\n");
            printf("\n");
            printf("MOV    EDX, dividend\n", n);
            printf("\n");
            printf("; quotient now in EDX\n");
        }
        else
        {
            printf("; dividend: register or memory location\n");
            printf("\n");
            printf("SHR    dividend, %d\n", n);
            printf("\n");
            printf("; quotient replaced dividend\n");
        }
        goto printed_code;
    } 
    
    /* Generate m, s for algorithm 0. Based on: Granlund, T.; Montgomery,
    * P.L.: "Division by Invariant Integers using Multiplication". 
    * SIGPLAN Notices, Vol. 29, June 1994, page 61.   
    */
   
    l = log2(t) + 1;
    j = (((uint64_t)(0xffffffff)) % ((uint64_t)(t)));
    k = (((uint64_t)(1)) << (32 + l)) / ((uint64_t)(0xffffffff - j));
    m_low = (((uint64_t)(1)) << (32 + l)) / t;
    m_high = ((((uint64_t)(1)) << (32 + l)) + k) / t;
    while (((m_low >> 1) < (m_high >> 1)) && (l > 0))
    {
        m_low = m_low >> 1;
        m_high = m_high >> 1;
        l = l - 1;
    }
    if ((m_high >> 32) == 0)
    {
        m = ((uint32_t)(m_high));
        s = l;
        a = 0;
    } /* Generate m, s for algorithm 1. Based on: Magenheimer, D.J.; et al:      "Integer Multiplication and Division on the HP Precision Architecture".      IEEE Transactions on Computers, Vol 37, No. 8, August 1988, page 980.   */
    else
    {
        s = log2(t);
        m_low = (((uint64_t)(1)) << (32 + s)) / ((uint64_t)(t));
        r = ((uint32_t)((((uint64_t)(1)) << (32 + s)) % ((uint64_t)(t))));
        m = (r < ((t >> 1) + 1)) ? ((uint32_t)(m_low)) : ((uint32_t)(m_low)) + 1;
        a = 1;
    } /* Reduce multiplier for either algorithm to smallest possible */
    while (!(m & 1))
    {
        m = m >> 1;
        s--;
    } /* Adjust multiplier for reduction of even divisors */
    s += n;
    if (a)
    {
        printf("; dividend: register other than EAX or memory location\n");
        printf("\n");
        printf("MOV    EAX, 0%08lXh\n", m);
        printf("MUL    dividend\n");
        printf("ADD    EAX, 0%08lXh\n", m);
        printf("ADC    EDX, 0\n");
        if (s)
            printf("SHR    EDX, %d\n", s);
        printf("\n");
        printf("; quotient now in EDX\n");
    }
    else
    {
        printf("; dividend: register other than EAX or memory location\n");
        printf("\n");
        printf("MOV    EAX, 0%08lXh\n", m);
        printf("MUL    dividend\n");
        if (s)
            printf("SHR    EDX, %d\n", s);
        printf("\n");
        printf("; quotient now in EDX\n");
    }
printed_code:
    fprintf(stderr, "\n");
    exit(0);

    return (0);
}