/** @brief NotesLoaderCHART - loads notes from .chart files, custon guitar hero files, only used in guitar mode */

#ifndef NotesLoaderCHART_H
#define NotesLoaderCHART_H

class Song;
class Steps;

namespace CHARTLoader
{
   void GetApplicableFiles( const std::string &sPath, std::vector<std::string> &out );
   bool LoadFromDir( const std::string &sDir, Song &out );
   bool LoadNoteDataFromSimfile( const std::string & cachePath, Steps &out );
}

#endif /* NotesLoaderCHART_H */