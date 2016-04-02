/** @brief NotesLoaderCHART - loads notes from .chart files, custon guitar hero files, only used in guitar mode */

#ifndef NotesLoaderCHART_H
#define NotesLoaderCHART_H

class Song;
class Steps;

namespace CHARTLoader
{
   void GetApplicableFiles( const RString &sPath, vector<RString> &out );
   bool LoadFromDir( const RString &sDir, Song &out );
   bool LoadNoteDataFromSimfile( const RString & cachePath, Steps &out );
}

#endif /* NotesLoaderCHART_H */
