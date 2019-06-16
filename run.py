#!/usr/bin/env python

import random
from gene_pool import Chromosone

POPULATION=500
SURVIVOR_RATIO=0.5
EPOCHS=1200
MUTATION_RATE=0.03
START = int(POPULATION * SURVIVOR_RATIO)

class Jobby(Chromosone) :
    def __init__( self, sz ) :
        Chromosone.__init__( self, sz ) 
    
    def error( self ) :
        da = ( float(self) - 13.0 )
        return da*da  



def sort( a ) :
    return a.error()


def main() :

    genes = []
    for i in range( POPULATION ) :
        genes.append( Jobby.random( 64 ) )

    # for g in genes :
    #     print( float(g) ) 

    for generation in range(EPOCHS) :
        genes = sorted( genes, key=sort, reverse=False )
        for i in range( START, len(genes) ) :
            p1 = random.randint(0,START)
            p2 = random.randint(0,START)
            c = Jobby.from_parents(  genes[p1], genes[p2] )
            c.mutate( MUTATION_RATE ) 
            genes[i] = c

    print( float( genes[0] ), float( genes[49] ) )

if __name__=="__main__" :
    main()