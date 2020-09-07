#ifndef STRUCT_IO_H_
#define STRUCT_IO_H_

#include "ByteIO.h"
#include "GliderStructs.h"
#include "Prefs.h"

// The following functions all return nonzero on success and zero on failure.

int ReadPrefsInfo (byteio *reader, prefsInfo *data);
int WritePrefsInfo (byteio *writer, const prefsInfo *data);
int ReadScoresType (byteio *reader, scoresType *data);
int WriteScoresType (byteio *writer, const scoresType *data);
int ReadGame2Type (byteio *reader, game2Type *data);
int WriteGame2Type (byteio *writer, const game2Type *data);
int ReadHouseType (byteio *reader, houseType *data);
int WriteHouseType (byteio *writer, const houseType *data);
int ReadBoundsType (byteio *reader, boundsType *data);
int WriteBoundsType (byteio *writer, const boundsType *data);
int ReadDemoType (byteio *reader, demoType *data);
int WriteDemoType (byteio *writer, const demoType *data);

#endif
