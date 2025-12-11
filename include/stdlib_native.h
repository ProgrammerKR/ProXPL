#ifndef PROX_STDLIB_H
#define PROX_STDLIB_H

#include "value.h"

// Math Functions (15)
Value nativeSqrt(int argCount, Value *args);
Value nativeAbs(int argCount, Value *args);
Value nativePow(int argCount, Value *args);
Value nativeSin(int argCount, Value *args);
Value nativeCos(int argCount, Value *args);
Value nativeTan(int argCount, Value *args);
Value nativeCeil(int argCount, Value *args);
Value nativeFloor(int argCount, Value *args);
Value nativeLog(int argCount, Value *args);
Value nativeExp(int argCount, Value *args);
Value nativeRandom(int argCount, Value *args);
Value nativeRandint(int argCount, Value *args);
Value nativeMax(int argCount, Value *args);
Value nativeMin(int argCount, Value *args);
Value nativeRound(int argCount, Value *args);

// String Functions (15)
Value nativeLen(int argCount, Value *args);
Value nativeUpper(int argCount, Value *args);
Value nativeLower(int argCount, Value *args);
Value nativeTrim(int argCount, Value *args);
Value nativeSplit(int argCount, Value *args);
Value nativeJoin(int argCount, Value *args);
Value nativeReplace(int argCount, Value *args);
Value nativeStartswith(int argCount, Value *args);
Value nativeEndswith(int argCount, Value *args);
Value nativeContains(int argCount, Value *args);
Value nativeIndexOf(int argCount, Value *args);
Value nativeSubstring(int argCount, Value *args);
Value nativeCharAt(int argCount, Value *args);
Value nativeFormat(int argCount, Value *args);
Value nativeToBytes(int argCount, Value *args);

// Type Conversion (10)
Value nativeToInt(int argCount, Value *args);
Value nativeToFloat(int argCount, Value *args);
Value nativeToString(int argCount, Value *args);
Value nativeToBool(int argCount, Value *args);
Value nativeToList(int argCount, Value *args);
Value nativeToDict(int argCount, Value *args);
Value nativeToHex(int argCount, Value *args);
Value nativeToBin(int argCount, Value *args);
Value nativeParseJSON(int argCount, Value *args);
Value nativeStringifyJSON(int argCount, Value *args);

// I/O Functions (5)
Value nativePrint(int argCount, Value *args);
Value nativeInput(int argCount, Value *args);
Value nativeReadFile(int argCount, Value *args);
Value nativeWriteFile(int argCount, Value *args);
Value nativeAppendFile(int argCount, Value *args);

// System Functions (5)
Value nativeClock(int argCount, Value *args);
Value nativeExit(int argCount, Value *args);
Value nativePlatform(int argCount, Value *args);
Value nativeVersion(int argCount, Value *args);
Value nativeExec(int argCount, Value *args);

// DateTime Functions (5)
Value nativeNow(int argCount, Value *args);
Value nativeTimestamp(int argCount, Value *args);
Value nativeSleep(int argCount, Value *args);
Value nativeFormatDate(int argCount, Value *args);
Value nativeParseDate(int argCount, Value *args);

// Runtime Functions (5)
Value nativeType(int argCount, Value *args);
Value nativeAssert(int argCount, Value *args);
Value nativeId(int argCount, Value *args);
Value nativeHash(int argCount, Value *args);
Value nativeIsInstance(int argCount, Value *args);

// Collections Functions (15)
Value nativeRange(int argCount, Value *args);
Value nativePush(int argCount, Value *args);
Value nativePop(int argCount, Value *args);
Value nativeInsert(int argCount, Value *args);
Value nativeRemove(int argCount, Value *args);
Value nativeSort(int argCount, Value *args);
Value nativeReverse(int argCount, Value *args);
Value nativeKeys(int argCount, Value *args);
Value nativeValues(int argCount, Value *args);
Value nativeEntries(int argCount, Value *args);
Value nativeContainsKey(int argCount, Value *args);
Value nativeMerge(int argCount, Value *args);
Value nativeClone(int argCount, Value *args);
Value nativeDeepClone(int argCount, Value *args);
Value nativeClear(int argCount, Value *args);

void registerStdlib();

#endif
