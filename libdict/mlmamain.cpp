/******************************************************************************

    libmorphrus - dictiorary-based morphological analyser for Russian.
    Copyright (C) 1994-2016 Andrew Kovalenko aka Keva

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    Contacts:
      email: keva@meta.ua, keva@rambler.ru
      Skype: big_keva
      Phone: +7(495)648-4058, +7(926)513-2991

******************************************************************************/
# include <namespace.h>
# include "../include/mlma1049.h"
# include "mlmadefs.h"
# include <xmorph/scandict.h>
# include <xmorph/capsheme.h>
# include <xmorph/scanlist.h>
# include <xmorph/wildscan.h>
# include <xmorph/lemmatiz.h>
# include <string.h>

#if defined(_MSC_VER) 
#   if !defined( strncasecmp )
#     define strncasecmp memicmp
#   endif  // strncasecmp
#   if !defined( strcasecmp )
#     define strcasecmp  strcmpi
#   endif  // strcasecmp
# endif

# if !defined( _WIN32_WCE )
  # define  CATCH_ALL         try {
  # define  ON_ERRORS( code ) } catch ( ... ) { return (code); }
# else
  # define  CATCH_ALL
  # define  ON_ERRORS( code )
# endif  // ! _WIN32_WCE

namespace LIBMORPH_NAMESPACE
{
  //
  // the new api - IMlma interface class
  //
  struct  CMlmaMb: public IMlmaMb
  {
    virtual int MLMAPROC  Attach();
    virtual int MLMAPROC  Detach();

    virtual int MLMAPROC  CheckWord( const char*      pszstr, size_t    cchstr,
                                     unsigned         dwsets );
    virtual int MLMAPROC  Lemmatize( const char*      pszstr, size_t    cchstr,
                                     SLemmInfoA*      output, size_t    cchout,
                                     char*            plemma, size_t    clemma,
                                     SGramInfo*       pgrams, size_t    ngrams,
                                     unsigned         dwsets );
    virtual int MLMAPROC  BuildForm( char*            output, size_t    cchout,
                                     lexeme_t         nlexid, formid_t  idform );
    virtual int MLMAPROC  FindForms( char*            output, size_t    cchout,
                                     const char*      pszstr, size_t    cchstr,
                                     formid_t         idform );
    virtual int MLMAPROC  CheckHelp( char*            output, size_t    cchout,
                                     const char*      pszstr, size_t    cchstr );
    virtual int MLMAPROC  GetWdInfo( unsigned char*   pwindo, lexeme_t  nlexid );
  };
  CMlmaMb mlmaMbInstance;

  // CMlmaMb implementation

  int   CMlmaMb::Attach()
  {
    return 0;
  }

  int   CMlmaMb::Detach()
  {
    return 0;
  }

  int   CMlmaMb::CheckWord( const char* pszstr, size_t  cchstr, unsigned  dwsets )
  {
    CATCH_ALL
      byte_t    locase[256];
      char        cpsstr[256];
      doCheckWord scheck( locase, dwsets );
	  listLookup<doCheckWord, steminfo> lookup( scheck );

    // check string length
      if ( cchstr == (size_t)-1 )
        cchstr = strlen( pszstr );

    // check for overflow
      if ( cchstr >= sizeof(locase) )
        return WORDBUFF_FAILED;

    if ( (cchstr = client2win(cpsstr, sizeof(cpsstr), pszstr, cchstr )) != (size_t)-1 ) pszstr = cpsstr;
	else  return WORDBUFF_FAILED;

    // get capitalization scheme
      scheck.scheme = GetCapScheme( locase, sizeof(locase), pszstr, cchstr ) & 0x0000ffff;

      // fill scheck structure
      return LinearScanDict<byte_t, int>( lookup, stemtree, locase, cchstr );

    ON_ERRORS( -1 )
  }

  int   CMlmaMb::Lemmatize( const char* pszstr, size_t  cchstr,
                            SLemmInfoA* output, size_t  cchout,
                            char*       plemma, size_t  clemma,
                            SGramInfo*  pgrams, size_t  ngrams, unsigned  dwsets )
  {
    CATCH_ALL
      byte_t    locase[256];
      char        cpsstr[256];
      doLemmatize lemact( locase, dwsets, 0 );
      listLookup<doLemmatize, steminfo> lookup( lemact );

    // check string length
      if ( cchstr == (size_t)-1 )
        cchstr = strlen( pszstr );

    // check for overflow
      if ( cchstr >= sizeof(locase) )
        return WORDBUFF_FAILED;

    if ( (cchstr = client2win(cpsstr, sizeof(cpsstr), pszstr, cchstr )) != (size_t)-1 ) pszstr = cpsstr;
    else  return WORDBUFF_FAILED;

    // get capitalization scheme
      lemact.scheme = GetCapScheme( locase, sizeof(locase), pszstr, cchstr ) & 0x0000ffff;

    // fill other fields
      lemact.elemma = (lemact.plemma = output) + cchout;
      lemact.eforms = (lemact.pforms = plemma) + clemma;
      lemact.egrams = (lemact.pgrams = pgrams) + ngrams;

    // call dictionary scanner
      return LinearScanDict<byte_t, int>( lookup, stemtree, locase, cchstr ) < 0 ?
        lemact.nerror : (int)(lemact.plemma - output);
    ON_ERRORS( -1 )
  }

  int   CMlmaMb::BuildForm( char* output, size_t  cchout, lexeme_t  nlexid, formid_t  idform )
  {
    CATCH_ALL
      byte_t        lidkey[0x10];
      const byte_t* ofsptr;
      auto          getofs = []( const byte_t* thedic, const byte_t*, size_t cchstr ) {  return cchstr == 0 ? thedic : nullptr;  };

    // Оригинальная форма слова не задана, следует применять модификацию алгоритма, "прыгающую"
    // по словарю идентификаторов лексем сразу в нужную точку на странице.
      if ( (ofsptr = LinearScanDict<word16_t, const byte_t*>( getofs, lidstree, lidkey, lexkeylen( lidkey, nlexid ) )) != nullptr )
      {
        const byte_t* dicpos = stemtree + getserial( ofsptr );
        byte_t        szstem[0x80];
        doBuildForm     abuild( szstem, 0, 0 );

        abuild.outend = (abuild.output = output) + cchout;
        abuild.grinfo = 0;
        abuild.bflags = 0;
        abuild.idform = idform;

        return RecursGetTrack<byte_t, int>( listTracer<doBuildForm, steminfo>( abuild, szstem, dicpos ),
          stemtree, szstem, 0, dicpos ) >= 0 ? abuild.rcount : abuild.nerror;
      }
      return 0;
    ON_ERRORS( -1 )
  }

  int   CMlmaMb::FindForms( char* output, size_t  cchout, const char* pszstr, size_t  cchstr, formid_t idform )
  {
    CATCH_ALL
      byte_t    locase[256];
      char        cpsstr[256];
      doBuildForm abuild( locase, 0, 0 );
      listLookup<doBuildForm, steminfo> lookup( abuild );

    // check string length
      if ( cchstr == (size_t)-1 )
        cchstr = strlen( pszstr );

    // check for overflow
      if ( cchstr >= sizeof(locase) )
        return WORDBUFF_FAILED;

    // modify the codepage
	  if ( (cchstr = client2win(cpsstr, sizeof(cpsstr), pszstr, cchstr )) != (size_t)-1 ) pszstr = cpsstr;
	  else  return WORDBUFF_FAILED;

      abuild.scheme = GetCapScheme( locase, sizeof(locase), pszstr, cchstr ) & 0x0000ffff;
      abuild.outend = (abuild.output = output) + cchout;
      abuild.grinfo = 0;
      abuild.bflags = 0;
      abuild.idform = idform;

      return LinearScanDict<byte_t, int>( lookup, stemtree, locase, cchstr ) < 0 ? abuild.nerror : abuild.rcount;
    ON_ERRORS( -1 )
  }

  int   CMlmaMb::CheckHelp( char* output, size_t  cchout, const char* pszstr, size_t  cchstr )
  {
    CATCH_ALL
      byte_t  locase[256];
      char      cpsstr[256];
      int       nchars;

    // check string length
      if ( cchstr == (size_t)-1 )
        cchstr = strlen( pszstr );

    // check for overflow
      if ( cchstr >= sizeof(locase) )
        return WORDBUFF_FAILED;

    // modify the codepage
	  if ( (cchstr = client2win(cpsstr, sizeof(cpsstr), pszstr, cchstr )) != (size_t)-1 ) pszstr = cpsstr;
	  else  return WORDBUFF_FAILED;

    // change the word to the lower case
      memcpy( locase, pszstr, cchstr );
        locase[cchstr] = '\0';
      SetLowerCase( locase );

    // scan the dictionary
      if ( (nchars = (int)WildScan( (byte_t*)cpsstr, cchout, locase, cchstr )) <= 0 )
        return nchars;

      return (int)win2client(output, cchout, cpsstr, nchars);
    ON_ERRORS( -1 )
  }

  int   CMlmaMb::GetWdInfo( unsigned char* pwinfo, lexeme_t lexkey )
  {
    CATCH_ALL
      byte_t        lidkey[0x10];
      const byte_t* ofsptr;
      auto          getofs = []( const byte_t* thedic, const byte_t*, size_t cchstr ){  return cchstr == 0 ? thedic : nullptr;  };


    // Оригинальная форма слова не задана, следует применять модификацию алгоритма, "прыгающую"
    // по словарю идентификаторов лексем сразу в нужную точку на странице.
      if ( (ofsptr = LinearScanDict<word16_t, const byte_t*>( getofs, lidstree, lidkey, lexkeylen( lidkey, lexkey ) )) != nullptr )
      {
        const byte_t* dicpos = stemtree + getserial( ofsptr );
        lexeme_t        nlexid = getserial( dicpos );
        word16_t        oclass = getword16( dicpos );
        steminfo        stinfo;

        if ( nlexid != lexkey )
          return -2;

        *pwinfo = stinfo.Load( classmap + (oclass & 0x7fff) ).wdinfo & 0x3f;
          return 1;
      }
      return 0;
    ON_ERRORS( -1 )
  }
}

extern "C" { 

int   MLMAPROC        mlmaruLoadMbAPI( IMlmaMb**  ptrAPI )
{
  if ( ptrAPI == NULL )
    return -1;
  *ptrAPI = (IMlmaMb*)&LIBMORPH_NAMESPACE::mlmaMbInstance;
    return 0;
}


}


