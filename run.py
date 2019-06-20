#!/usr/bin/env python

import random
from gene_pool import Chromosone

POPULATION=200
SURVIVOR_RATIO=0.3
EPOCHS=1000
MUTATION_RATE=0.01
START = int(POPULATION * SURVIVOR_RATIO)

class Genome() :
    def __init__( self, sz=10, n=3 ) :
        self.chromosones = []
        for _ in range(n) :
            self.chromosones.append( Chromosone.random(sz) )


    @classmethod
    def from_parents( cls, p1, p2, mutation_rate=MUTATION_RATE ) :
        cc = []
        for i in range( len(p1.chromosones) ) :
            c = Chromosone.from_parents( p1.chromosones[i],p2.chromosones[i]) 
            c.mutate( mutation_rate )
            cc.append( c )
        g = Genome()
        g.chromosones = cc
        return g


    def __repr__( self ) :
        s = str( int(self.chromosones[0]) ) + " " + \
            str( int(self.chromosones[1]) ) + " " + \
            str( int(self.chromosones[2]) ) + " " + \
            str( self.error() )
        return s 


    def error( self ) :
        fa = int(self.chromosones[0]) 
        fb = int(self.chromosones[1]) 
        fc = int(self.chromosones[2]) 
        ft = ( fa*fb - fc ) - 50.0
        a = (fa + fb + fc) / 3.0
        da = fa - a
        db = fb - a
        dc = fc - a
        return ft*ft + ( da*da + db*db + dc*dc ) / 50.0 



def sort( a ) :
    return a.error() 


def main() :
    g1 = Genome()
    g2 = Genome()
    g3 = Genome.from_parents( g1, g2 )

    genes = []
    for i in range( POPULATION ) :
        genes.append( Genome( 32, 3 ) )

    # for g in genes :
    #     print( float(g) ) 

    last_error = 0
    for epoch in range(EPOCHS) :
        genes = sorted( genes, key=sort, reverse=False )
        for i in range( START, len(genes) ) :
            p1 = random.randint(0,START)
            p2 = random.randint(0,START)
            c = Genome.from_parents( genes[p1], genes[p2] )
            # c.mutate( MUTATION_RATE ) 
            genes[i] = c
        er = genes[0].error()
        if( er != last_error ) :
            print( epoch, er )
            last_error = er

    print( genes[0] )

if __name__=="__main__" :
    main()