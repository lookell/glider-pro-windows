#ifndef STRUCT_IO_H_
#define STRUCT_IO_H_

#include "ByteIO.h"
#include "GliderStructs.h"
#include "Prefs.h"
#include "WinAPI.h"

HRESULT ReadPrefsInfo (byteio *reader, prefsInfo *data);
HRESULT WritePrefsInfo (byteio *writer, const prefsInfo *data);
HRESULT ReadScoresType (byteio *reader, scoresType *data);
HRESULT WriteScoresType (byteio *writer, const scoresType *data);
HRESULT ReadGame2Type (byteio *reader, game2Type *data);
HRESULT WriteGame2Type (byteio *writer, const game2Type *data);
HRESULT ReadHouseType (byteio *reader, houseType *data);
HRESULT WriteHouseType (byteio *writer, const houseType *data);
HRESULT ReadBoundsType (byteio *reader, boundsType *data);
HRESULT WriteBoundsType (byteio *writer, const boundsType *data);
HRESULT ReadDemoType (byteio *reader, demoType *data);
HRESULT WriteDemoType (byteio *writer, const demoType *data);

#endif
