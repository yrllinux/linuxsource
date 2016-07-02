/***********************************************************************
Copyright (c) 2003 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..
RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

/****************************************************************************

  ci.h  --  Configuration Interface Header

****************************************************************************/

#ifndef __CI_H
#define __CI_H


#ifdef __cplusplus
extern "C" {
#endif


#include "rverror.h"

RV_DECLARE_HANDLE(HCFG);

typedef enum
{
     ERR_CI_NOERROR          = 0,     /* no error */

     ERR_CI_GENERALERROR     = -1,    /* general error */
     ERR_CI_BUFFERTOOSMALL   = -2,    /* return buffer was too small */
     ERR_CI_NOTFOUND         = -3,    /* the value was not found */
     ERR_CI_INTERFACEUNKNOWN = -4,    /* can't ID input/output source */
     ERR_CI_ALLOCERROR       = -5,    /* couldn't allocate memory */

     ERR_CI_REGOPENKEY       = -10,   /* couldn't open registry key */
     ERR_CI_REGCREATE        = -11,   /* couldn't create registry key */

     ERR_CI_FILEOPEN         = -20,   /* couldn't open file */
     ERR_CI_FILEREAD         = -21,   /* error reading from file */
     ERR_CI_FILEWRITE        = -22    /* error writing to file */
} ci_errors;

typedef enum
{
    ci_str_not               = 0,     /* not a string */
    ci_str_regular           = 1,     /* a regular string */
    ci_str_bit               = 2      /* a bit string */
} ci_str_type;



/* Size of a virtual file source, as given by the application to
   ciCreateVirtualFileSouze(). */
#if (RV_ARCH_BITS == RV_ARCH_BITS_32)
#define CI_VIRTUAL_FILE_SOURCE_SIZE (RvRoundToSize(40, RV_ALIGN_SIZE))
#elif (RV_ARCH_BITS == RV_ARCH_BITS_64)
#define CI_VIRTUAL_FILE_SOURCE_SIZE (RvRoundToSize(56, RV_ALIGN_SIZE))
#else
#error RV_ARCH_BITS not defined properly
#endif



/******************************************************************************
 * ciVirtualFileOpenEv
 * ----------------------------------------------------------------------------
 * General: Function given by the application to open the virtual file when
 *          we want to read from its beginning.
 *          It should be implemented like the fopen() function.
 *          This function might be called more than once from ciConstruct().
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  context      - Context to use for this call.
 * Output: None.
 *****************************************************************************/
typedef RvStatus (RVCALLCONV *ciVirtualFileOpenEv) (
    IN void* context);


/******************************************************************************
 * ciVirtualFileCloseEv
 * ----------------------------------------------------------------------------
 * General: Function given by the application to close the virtual file when
 *          we don't want to read from it.
 *          It should be implemented like the fclose() function.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  context      - Context to use for this call.
 * Output: None.
 *****************************************************************************/
typedef RvStatus (RVCALLCONV *ciVirtualFileCloseEv) (
    IN void* context);


/******************************************************************************
 * ciVirtualFileReadLineEv
 * ----------------------------------------------------------------------------
 * General: Function given by the application to read a line from a virtual
 *          file and move the file's pointer to the next position in the file.
 *          It should be implemented like the fgets() function.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure or if we reached the end of the virtual file.
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  context      - Context to use for this call.
 *         buffer       - Buffer to fill in with the read line. This line must
 *                        be filled with a NULL terminated string.
 *         bufferSize   - Size of the buffer to fill.
 * Output: None.
 *****************************************************************************/
typedef RvStatus (RVCALLCONV *ciVirtualFileReadLineEv) (
    IN void*    context,
    IN RvChar*  buffer,
    IN RvSize_t bufferSize);



/******************************************************************************
 * ciCreateVirtualFileSource
 * ----------------------------------------------------------------------------
 * General: Create a configuration source for a virtual file.
 *          This kind of a configuration source can be used when you have a
 *          configuration file that is stored in something other than a regular
 *          file system. It actually allows reading these files from memory
 *          buffers, flash or over the network.
 *          The configuration source itself is only needed for the ciConstruct()
 *          function, and after calling ciConstruct(), or your Stack's
 *          initialization function, you can free any resources used by your
 *          virtual file.
 *
 * Return Value: RV_OK  - if successful.
 *               Other on failure
 * ----------------------------------------------------------------------------
 * Arguments:
 * Input:  sourceResult - Source result string. This buffer is given by the
 *                        user and filled by this function with the relevant
 *                        information. The size of this string must be
 *                        at least CI_VIRTUAL_FILE_SOURCE_SIZE.
 *         context      - Context to use on calls to the virtual file's
 *                        functions.
 *         openFunc     - Open virtual file function. Called when CI wants
 *                        to open the virtual file. Might be called more than
 *                        once during ciConstruct(), but only on a "closed"
 *                        file.
 *         closeFunc    - Close virtual file function. Called when CI wants
 *                        to close the virtual file. Might be called more than
 *                        once during ciConstruct().
 *         readLineEv   - Request to read a line from the virtual file. Works
 *                        similar to the fgets() function.
 * Output: None.
 *****************************************************************************/
RVAPI RvStatus RVCALLCONV ciCreateVirtualFileSource(
    IN RvChar*                  sourceResult,
    IN void*                    context,
    IN ciVirtualFileOpenEv      openFunc,
    IN ciVirtualFileCloseEv     closeFunc,
    IN ciVirtualFileReadLineEv  readLineEv);



RVAPI
HCFG RVCALLCONV ciConstruct(
        IN  const char * source      /* configuration source identifier */
        );


RVAPI
HCFG RVCALLCONV ciConstructEx(
        IN  const char * source,     /* configuration source identifier */
        IN  int          extraData,  /* maximum amount of data growth */
        IN  int          extraNodes  /* maximum number of nodes growth */
        );

RVAPI
ci_errors RVCALLCONV ciDestruct(
        IN  HCFG hCfg
        );


RVAPI
ci_errors RVCALLCONV ciSave(
        IN  HCFG hCfg,
        IN  char *target
        );


RVAPI
ci_errors RVCALLCONV ciGetValue(
        IN   HCFG         hCfg,
        IN   const char * path,      /* full path to node, i.e. "a.b.c" */
        OUT  RvBool *       isString,
        OUT  RvInt32 *    value      /* data for ints, length for strings */
        );


RVAPI
ci_errors RVCALLCONV ciGetValueExt(
        IN   HCFG         hCfg,
        IN   const char  *path,      /* full path to node, i.e. "a.b.c" */
        OUT  ci_str_type *strType,
        OUT  RvInt32 *    value      /* data for ints, length for strings */
        );


RVAPI
ci_errors RVCALLCONV ciGetString(
        IN  HCFG         hCfg,
        IN  const char * path,       /* full path to node, i.e. "a.b.c" */
        OUT char *       str,
        IN  RvUint32     maxStrLen   /* length of output string buffer */
        );


RVAPI
ci_errors RVCALLCONV ciGetBitString(
        IN  HCFG         hCfg,
        IN  const char * path,       /* full path to node, i.e. "a.b.c" */
        OUT char *       str,
        IN  RvUint32     maxStrLen,  /* length of output string buffer */
        OUT RvUint32 *   bits
        );


RVAPI      /* returns ERR_CI_NOTFOUND when there is no 'next' value */
ci_errors RVCALLCONV ciNext(
        IN  HCFG         hCfg,
        IN  const char * path,       /* full path to node, or NULL to start
                                        at first node */
        OUT char *       nextPath,   /* buffer for next path IN cfg */
        IN  RvUint32     maxPathLen  /* length of output buffer */
        );


RVAPI
ci_errors RVCALLCONV ciSetValue(
        IN  HCFG         hCfg,
        IN  const char * path,       /* full path to node */
        IN  RvBool       isString,
        IN  RvInt32      value,      /* data for ints, length for strings */
        IN  const char * str         /* null for ints, data for strings */
        );


RVAPI
ci_errors RVCALLCONV ciSetBitString(
        IN  HCFG         hCfg,
        IN  const char * path,       /* full path to node */
        IN  RvInt32      bits,       /* number of bits in the string */
        IN  const char * str         /* null for ints, data for strings */
        );


RVAPI
ci_errors RVCALLCONV ciDeleteValue(
        IN  HCFG hCfg,
        IN  const char * path
        );



#ifdef __cplusplus
}
#endif


#endif /* __CI_H */

