#ifndef INC_CENG_MACRO_H
#define INC_CENG_MACRO_H


// define a join to use, ripped from boost
#define CENG_Join( X, Y ) CENG_DO_JOIN( X, Y )
#define CENG_DO_JOIN( X, Y ) CENG_DO_JOIN2(X,Y)
#define CENG_DO_JOIN2( X, Y ) X##Y

#endif
