/***********************************************************************
Filename   : rv64bits.h
Description: 64 bit module header
************************************************************************
      Copyright (c) 2001,2002 RADVISION Inc. and RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Inc. and RADVISION Ltd.. No part of this document may be
reproduced in any form whatsoever without written prior approval by
RADVISION Inc. or RADVISION Ltd..

RADVISION Inc. and RADVISION Ltd. reserve the right to revise this
publication and make changes without obligation to notify any person of
such revisions or changes.
***********************************************************************/
#ifndef RV_64BITS_H
#define RV_64BITS_H

#if !defined(RV_64BITS_TYPE) || \
    ((RV_64BITS_TYPE != RV_64BITS_STANDARD) && (RV_64BITS_TYPE != RV_64BITS_MANUAL))
#error RV_64BITS_TYPE not set properly
#endif

#if defined(__cplusplus)
extern "C" {
#endif 

/* Prototypes and macros */

#if (RV_64BITS_TYPE == RV_64BITS_STANDARD)
/* Compiler supports 64 bit math so just macro out everything */

/* Basic Assignments */
/* Not used in STANDARD mode */

/* Mathematical Operations */
#define Rv2Uint64ShiftLeft(_val, _numbits) ((_val) << (_numbits))
#define Rv2Int64ShiftLeft(_val, _numbits) ((_val) << (_numbits))
#define Rv2Uint64ShiftRight(_val, _numbits) ((_val) >> (_numbits))
#define Rv2Int64ShiftRight(_val, _numbits) ((_val) >> (_numbits))
#define Rv2Uint64Not(_val) (~(_val))
#define Rv2Int64Not(_val) (~(_val))
#define Rv2Uint64And(_val1, _val2) ((_val1) & (_val2))
#define Rv2Int64And(_val1, _val2) ((_val1) & (_val2))
#define Rv2Uint64Or(_val1, _val2) ((_val1) | (_val2))
#define Rv2Int64Or(_val1, _val2) ((_val1) | (_val2))
#define Rv2Uint64Xor(_val1, _val2) ((_val1) ^ (_val2))
#define Rv2Int64Xor(_val1, _val2) ((_val1) ^ (_val2))
#define Rv2Uint64Add(_val1, _val2) ((_val1) + (_val2))
#define Rv2Int64Add(_val1, _val2) ((_val1) + (_val2))
#define Rv2Uint64Sub(_val1, _val2) ((_val1) - (_val2))
#define Rv2Int64Sub(_val1, _val2) ((_val1) - (_val2))
#define Rv2Uint64Mul(_val1, _val2) ((_val1) * (_val2))
#define Rv2Int64Mul(_val1, _val2) ((_val1) * (_val2))
#define Rv2Uint64Div(_val1, _val2) ((_val1) / (_val2))
#define Rv2Int64Div(_val1, _val2) ((_val1) / (_val2))
#define Rv2Uint64Mod(_val1, _val2) ((_val1) % (_val2))
#define Rv2Int64Mod(_val1, _val2) ((_val1) % (_val2))

/* Cast from RvInt64 to RvUint64 and back */
#define Rv2Uint64FromRvInt64(_val) ((RvUint64)(_val))
#define Rv2Int64FromRvUint64(_val) ((RvInt64)(_val))

/* Cast from base types to RvInt64 and RvUint64 */
#define Rv2Uint64FromRvUint32(_val) ((RvUint64)(_val))
#define Rv2Uint64FromRvInt32(_val) ((RvUint64)(_val))
#define Rv2Int64FromRvUint32(_val) ((RvInt64)(_val))
#define Rv2Int64FromRvInt32(_val) ((RvInt64)(_val))
#define Rv2Uint64FromRvUint16(_val) ((RvUint64)(_val))
#define Rv2Uint64FromRvInt16(_val) ((RvUint64)(_val))
#define Rv2Int64FromRvUint16(_val) ((RvInt64)(_val))
#define Rv2Int64FromRvInt16(_val) ((RvInt64)(_val))
#define Rv2Uint64FromRvUint8(_val) ((RvUint64)(_val))
#define Rv2Uint64FromRvInt8(_val) ((RvUint64)(_val))
#define Rv2Int64FromRvUint8(_val) ((RvInt64)(_val))
#define Rv2Int64FromRvInt8(_val) ((RvInt64)(_val))
#define Rv2Uint64FromRvUint(_val) ((RvUint64)(_val))
#define Rv2Uint64FromRvInt(_val) ((RvUint64)(_val))
#define Rv2Int64FromRvUint(_val) ((RvInt64)(_val))
#define Rv2Int64FromRvInt(_val) ((RvInt64)(_val))
#define Rv2Uint64FromRvSize_t(_val) ((RvUint64)(_val))
#define Rv2Int64FromRvSize_t(_val) ((RvInt64)(_val))
#define Rv2Uint64FromRvChar(_val) ((RvUint64)(_val))
#define Rv2Int64FromRvChar(_val) ((RvInt64)(_val))

/* Cast to base types from RvInt64 and RvUint64 */
#define Rv2Uint64ToRvUint32(_val) ((RvUint32)(_val))
#define Rv2Uint64ToRvInt32(_val) ((RvInt32)(_val))
#define Rv2Int64ToRvUint32(_val) ((RvUint32)(_val))
#define Rv2Int64ToRvInt32(_val) ((RvInt32)(_val))
#define Rv2Uint64ToRvUint16(_val) ((RvUint16)(_val))
#define Rv2Uint64ToRvInt16(_val) ((RvInt16)(_val))
#define Rv2Int64ToRvUint16(_val) ((RvUint16)(_val))
#define Rv2Int64ToRvInt16(_val) ((RvInt16)(_val))
#define Rv2Uint64ToRvUint8(_val) ((RvUint8)(_val))
#define Rv2Uint64ToRvInt8(_val) ((RvInt8)(_val))
#define Rv2Int64ToRvUint8(_val) ((RvUint8)(_val))
#define Rv2Int64ToRvInt8(_val) ((RvInt8)(_val))
#define Rv2Uint64ToRvUint(_val) ((RvUint)(_val))
#define Rv2Uint64ToRvInt(_val) ((RvInt)(_val))
#define Rv2Int64ToRvUint(_val) ((RvUint)(_val))
#define Rv2Int64ToRvInt(_val) ((RvInt)(_val))
#define Rv2Uint64ToRvSize_t(_val) ((RvSize_t)(_val))
#define Rv2Int64ToRvSize_t(_val) ((RvSize_t)(_val))
#define Rv2Uint64ToRvChar(_val) ((RvChar)(_val))
#define Rv2Int64ToRvChar(_val) ((RvChar)(_val))

/* Logical Operations */
#define Rv2Uint64IsEqual(_val1, _val2) ((_val1) == (_val2))
#define Rv2Int64IsEqual(_val1, _val2) ((_val1) == (_val2))
#define RvUint64IsNotEqual(_val1, _val2) ((_val1) != (_val2))
#define RvInt64IsNotEqual(_val1, _val2) ((_val1) != (_val2))
#define Rv2Uint64IsLessThan(_val1, _val2) ((_val1) < (_val2))
#define Rv2Int64IsLessThan(_val1, _val2) ((_val1) < (_val2))
#define Rv2Uint64IsGreaterThan(_val1, _val2) ((_val1) > (_val2))
#define Rv2Int64IsGreaterThan(_val1, _val2) ((_val1) > (_val2))
#define Rv2Uint64IsLessThanOrEqual(_val1, _val2) ((_val1) <= (_val2))
#define Rv2Int64IsLessThanOrEqual(_val1, _val2) ((_val1) <= (_val2))
#define Rv2Uint64IsGreaterThanOrEqual(_val1, _val2) ((_val1) >= (_val2))
#define Rv2Int64IsGreaterThanOrEqual(_val1, _val2) ((_val1) >= (_val2))

/* Min/Max Operations */
#define RvUint64Min(val1, val2) RvMin((val1), (val2))
#define RvInt64Min(val1, val2)  RvMin((val1), (val2))
#define RvUint64Max(val1, val2) RvMax((val1), (val2))
#define RvInt64Max(val1, val2)  RvMax((val1), (val2))

#else
/* Use this module to do 64 bit math */

/* Basic Assignments */
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64Assign(RvUint32 ms32, RvUint32 ls32);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64Assign(RvInt sign, RvUint32 ms32, RvUint32 ls32);
RVCOREAPI RvInt64 RVCALLCONV Rv2Int64ShortAssign(RvInt32 ls32);

/* Mathematical Operations */
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64ShiftLeft(RvUint64 val, RvInt numbits);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64ShiftLeft(RvInt64 val, RvInt numbits);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64ShiftRight(RvUint64 val, RvInt numbits);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64ShiftRight(RvInt64 val, RvInt numbits);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64Not(RvUint64 val);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64Not(RvInt64 val);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64And(RvUint64 val1, RvUint64 val2);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64And(RvInt64 val1, RvInt64 val2);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64Or(RvUint64 val1, RvUint64 val2);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64Or(RvInt64 val1, RvInt64 val2);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64Xor(RvUint64 val1, RvUint64 val2);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64Xor(RvInt64 val1, RvInt64 val2);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64Add(RvUint64 val1, RvUint64 val2);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64Add(RvInt64 val1, RvInt64 val2);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64Sub(RvUint64 val1, RvUint64 val2);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64Sub(RvInt64 val1, RvInt64 val2);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64Mul(RvUint64 val1, RvUint64 val2);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64Mul(RvInt64 val1, RvInt64 val2);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64Div(RvUint64 val1, RvUint64 val2);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64Div(RvInt64 val1, RvInt64 val2);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64Mod(RvUint64 val1, RvUint64 val2);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64Mod(RvInt64 val1, RvInt64 val2);

/* Cast from RvInt64 to RvUint64 and back */
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64FromRvInt64(RvInt64 val);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64FromRvUint64(RvUint64 val);

/* Cast from base types to RvInt64 and RvUint64 */
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64FromRvUint32(RvUint32 val);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64FromRvInt32(RvInt32 val);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64FromRvUint32(RvUint32 val);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64FromRvInt32(RvInt32 val);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64FromRvUint16(RvUint16 val);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64FromRvInt16(RvInt16 val);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64FromRvUint16(RvUint16 val);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64FromRvInt16(RvInt16 val);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64FromRvUint8(RvUint8 val);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64FromRvInt8(RvInt8 val);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64FromRvUint8(RvUint8 val);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64FromRvInt8(RvInt8 val);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64FromRvUint(RvUint val);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64FromRvInt(RvInt val);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64FromRvUint(RvUint val);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64FromRvInt(RvInt val);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64FromRvSize_t(RvSize_t val);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64FromRvSize_t(RvSize_t val);
RVCOREAPI RvUint64 RVCALLCONV Rv2Uint64FromRvChar(RvChar val);
RVCOREAPI RvInt64  RVCALLCONV Rv2Int64FromRvChar(RvChar val);

/* Cast to base types from RvInt64 and RvUint64 */
RVCOREAPI RvUint32 RVCALLCONV Rv2Uint64ToRvUint32(RvUint64 val);
RVCOREAPI RvInt32  RVCALLCONV Rv2Uint64ToRvInt32(RvUint64 val);
RVCOREAPI RvUint32 RVCALLCONV Rv2Int64ToRvUint32(RvInt64 val);
RVCOREAPI RvInt32  RVCALLCONV Rv2Int64ToRvInt32(RvInt64 val);
RVCOREAPI RvUint16 RVCALLCONV Rv2Uint64ToRvUint16(RvUint64 val);
RVCOREAPI RvInt16  RVCALLCONV Rv2Uint64ToRvInt16(RvUint64 val);
RVCOREAPI RvUint16 RVCALLCONV Rv2Int64ToRvUint16(RvInt64 val);
RVCOREAPI RvInt16  RVCALLCONV Rv2Int64ToRvInt16(RvInt64 val);
RVCOREAPI RvUint8  RVCALLCONV Rv2Uint64ToRvUint8(RvUint64 val);
RVCOREAPI RvInt8   RVCALLCONV Rv2Uint64ToRvInt8(RvUint64 val);
RVCOREAPI RvUint8  RVCALLCONV Rv2Int64ToRvUint8(RvInt64 val);
RVCOREAPI RvInt8   RVCALLCONV Rv2Int64ToRvInt8(RvInt64 val);
RVCOREAPI RvUint   RVCALLCONV Rv2Uint64ToRvUint(RvUint64 val);
RVCOREAPI RvInt    RVCALLCONV Rv2Uint64ToRvInt(RvUint64 val);
RVCOREAPI RvUint   RVCALLCONV Rv2Int64ToRvUint(RvInt64 val);
RVCOREAPI RvInt    RVCALLCONV Rv2Int64ToRvInt(RvInt64 val);
RVCOREAPI RvSize_t RVCALLCONV Rv2Uint64ToRvSize_t(RvUint64 val);
RVCOREAPI RvSize_t RVCALLCONV Rv2Int64ToRvSize_t(RvInt64 val);
RVCOREAPI RvChar   RVCALLCONV Rv2Uint64ToRvChar(RvUint64 val);
RVCOREAPI RvChar   RVCALLCONV Rv2Int64ToRvChar(RvInt64 val);

/* Logical Operations */
RVCOREAPI RvBool RVCALLCONV Rv2Uint64IsEqual(RvUint64 val1, RvUint64 val2);
RVCOREAPI RvBool RVCALLCONV Rv2Int64IsEqual(RvInt64 val1, RvInt64 val2);
#define                     RvUint64IsNotEqual(val1, val2) (!Rv2Uint64IsEqual(val1, val2))
#define                     RvInt64IsNotEqual(val1, val2) (!Rv2Int64IsEqual(val1, val2))
RVCOREAPI RvBool RVCALLCONV Rv2Uint64IsLessThan(RvUint64 val1, RvUint64 val2);
RVCOREAPI RvBool RVCALLCONV Rv2Int64IsLessThan(RvInt64 val1, RvInt64 val2);
RVCOREAPI RvBool RVCALLCONV Rv2Uint64IsGreaterThan(RvUint64 val1, RvUint64 val2);
RVCOREAPI RvBool RVCALLCONV Rv2Int64IsGreaterThan(RvInt64 val1, RvInt64 val2);
RVCOREAPI RvBool RVCALLCONV Rv2Uint64IsLessThanOrEqual(RvUint64 val1, RvUint64 val2);
RVCOREAPI RvBool RVCALLCONV Rv2Int64IsLessThanOrEqual(RvInt64 val1, RvInt64 val2);
RVCOREAPI RvBool RVCALLCONV Rv2Uint64IsGreaterThanOrEqual(RvUint64 val1, RvUint64 val2);
RVCOREAPI RvBool RVCALLCONV Rv2Int64IsGreaterThanOrEqual(RvInt64 val1, RvInt64 val2);

/* Min/Max Operations */
#define RvUint64Min(val1, val2) ((Rv2Uint64IsLessThan   ((val1), (val2))) ? (val1) : (val2))
#define RvInt64Min(val1, val2)  ((Rv2Int64IsLessThan    ((val1), (val2))) ? (val1) : (val2))
#define RvUint64Max(val1, val2) ((Rv2Uint64IsGreaterThan((val1), (val2))) ? (val1) : (val2))
#define RvInt64Max(val1, val2)  ((Rv2Int64IsGreaterThan ((val1), (val2))) ? (val1) : (val2))

#endif /* RV_64BITS_TYPE == RV_64BITS_STANDARD */

#if defined(__cplusplus)
}
#endif 

#endif /* RV_64BITS_H */
