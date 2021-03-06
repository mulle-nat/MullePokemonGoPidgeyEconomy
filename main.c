//
//  main.c
//  MullePokemonGoPidgeyEconomy
//
//  Created by Nat! on 30.08.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>


enum
{
   keep_one,
   sell_first
};


#define STRATEGY     keep_one
#define VERBOSE      0
#define DOUBLE_EGG   0


struct pokemongo
{
   int   pidgey;        // Taubsi
   int   pidgeotto;     // Tauboga
   int   candy;
   int   ep_100;
   int   catches;
   int   transforms;
   int   steps;
};


static double  pokemongo_get_average_per_catch( struct pokemongo *p)
{
   if( ! p->catches)
      return( 0.0);
   return( p->ep_100 * 100.0 / p->catches);
}


#if VERBOSE
static double  pokemongo_get_average_per_step( struct pokemongo *p)
{
   if( ! p->steps)
      return( 0.0);
   return( p->ep_100 * 100.0 / p->steps);
}
#endif


#if VERBOSE
static double  pokemongo_get_average_per_transform( struct pokemongo *p)
{
   if( ! p->transforms)
      return( 0.0);
   return( p->ep_100 * 100.0 / p->transforms);
}
#endif



static void   pokemongo_step( struct pokemongo *p, char *op)
{
   ++p->steps;
#if VERBOSE
   fprintf( stderr, "%d: %s %d %d %d %.2f\n",
               p->steps,
               op,
               p->pidgey + p->pidgeotto,
               p->candy,
               p->ep_100 * 100,
               pokemongo_get_average_per_catch( p));
#endif
}


static void   pokemongo_catch( struct pokemongo *p)
{
   p->catches += 1;
   p->pidgey  += 1;
   p->candy   += 3;
   p->ep_100  += 1;

   pokemongo_step( p, "catch");
}


static void   pokemongo_sell( struct pokemongo *p)
{
   assert( p->pidgey > 0 || p->pidgeotto > 0);

   if( p->pidgeotto)
      p->pidgeotto -= 1;
   else
      p->pidgey  -= 1;

   p->candy += 1;

   pokemongo_step( p, "sell");
}


static void   pokemongo_transform( struct pokemongo *p)
{
   assert( p->pidgey >= 1);
   assert( p->candy >= 12);

#if DOUBLE_EGG
   p->ep_100     += 10;
#else
   p->ep_100     += 5;
#endif
   p->candy      -= 12;
   p->pidgey     -= 1;
   p->pidgeotto  += 1;
   p->candy      += 1;
   p->transforms += 1;

   pokemongo_step( p, "transform");
}


static int   pokemongo_can_sell( struct pokemongo *p)
{
   if( p->pidgeotto > 0)
      return( 1);
   if( STRATEGY == sell_first)
      return( p->pidgey > 0);
   return( p->pidgey > 1);
}


static int   pokemongo_can_transform( struct pokemongo *p)
{
   return( p->pidgey > 0 && p->candy >= 12);
}


int main( int argc, const char * argv[])
{
   struct pokemongo   game;

   memset( &game, 0, sizeof( game));
   for(;;)
   {
      if( game.ep_100 >= 50000 / 100)  // 1 mio EP
      {
         printf( "ep            : %ld\n", (long) game.ep_100 * 100);
#if VERBOSE
         printf( "transforms    : %d\n", game.transforms);
         printf( "rate          : %.2f\n", pokemongo_get_average_per_step( &game));
#endif
         printf( "avg per catch : %.2f\n", pokemongo_get_average_per_catch( &game));
         break;
      }

      if( pokemongo_can_transform( &game))
      {
         pokemongo_transform( &game);
         continue;
      }

      if( pokemongo_can_sell( &game))
      {
         pokemongo_sell( &game);
         continue;
      }

      pokemongo_catch( &game);
   }
}
