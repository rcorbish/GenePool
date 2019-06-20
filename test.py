#!/usr/bin/env python

import sys
from gene_pool import Chromosone

for _ in range(20) :
    d = Chromosone.random( 10 ) 
    print( int(d) )

sys.exit()

# c = Chromosone( b'10110011000000000000011110' )
c = Chromosone( -1 )

c.mutate( .3 ) 
oc = c
c=Chromosone(c)
print( " C ", c, c.capacity )
print( "OC ", oc, oc.capacity )

c.mutate() 
print( " C'", c, c.capacity )

a = Chromosone( b"11111111", b"00000000" )
b = Chromosone( b"00000000", b"11111111" )

x = Chromosone( a, b )
print( " A ", a ) 
print( " B ", b ) 
print( " X ", x ) 
x.mutate( 0.3 )
print( " X'", x ) 

print(  a & b   )
print( a | b )
print(  ~(a ^ b) )

print( int(a) )
a = Chromosone( b"00000000", b"11111111", b"00000000", b"11111111", 
                b"00000000", b"11111111", b"00000000", b"11111111" )
print( a.countOnes(), a.countZeros() )
a.mutate( .2 )
print( a.countOnes(), a.countZeros() )
print( float(a), a )

