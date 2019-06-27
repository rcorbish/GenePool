#!/usr/bin/env python

import sys
from gene_pool import Chromosone

for _ in range(5) :
    d = Chromosone.random( 10 ) 
    print( int(d) )

a = Chromosone( b'111000' )
b = Chromosone( b'101111' )
print( "a & b", a & b )
print( "a | b", a | b )
print( "a ^ b", a ^ b )
print( "~a   ", ~a, int(~a) )
print( "a    ", bool(a) )

# c = Chromosone( b'10110011000000000000011110' )
c = Chromosone( 0xffffffff )
print( c, len(c) )

c.mutate( .3 ) 
oc = c
c=Chromosone(c)
print( " C ", c, c.capacity )
print( "OC ", oc, oc.capacity )

c.mutate() 
print( " C'", c, c.capacity )

a = Chromosone( b"1111111100000000" )
b = Chromosone( "0000000011111111" )

x = Chromosone.from_parents( a, b )
print( " A ", a ) 
print( " B ", b ) 
print( " X ", x ) 
x.mutate( 0.3 )
print( " X'", x ) 


a = Chromosone( b"0000000011111111000000001111111100000000111111110000000011111111" )
print( int(a) )
print( a.countOnes(), a.countZeros() )

a.mutate( .2 )
print( a.countOnes(), a.countZeros() )

print( float(a), a )

