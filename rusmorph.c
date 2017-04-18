#include <postgres.h>
#include <tsearch/ts_public.h>
#include <parser/parse_func.h>
#include <mb/pg_wchar.h>
#include "mlma1049.h"
#include "typedefs.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(rusmorph_init);
PG_FUNCTION_INFO_V1(rusmorph_lexize);

size_t client2win(char* dst, size_t dst_size, char const* src, size_t src_size)
{
    PG_TRY();
    {    	
		char* cnv = (char*)pg_do_encoding_conversion((unsigned char*)src, src_size, pg_get_client_encoding(), PG_WIN1251);
		strncpy(dst, cnv, dst_size);
		if (cnv == src && src_size < dst_size) { 
			dst[src_size] = '\0';
			return src_size;
		}
		return strlen(dst);
	}
	PG_CATCH();
    {
		return (size_t)-1;
	}
    PG_END_TRY();
}

size_t win2client(char* dst, size_t dst_size, char const* src, size_t src_size)
{
    PG_TRY();
    {    	
		char* cnv = (char*)pg_do_encoding_conversion((unsigned char*)src, src_size, PG_WIN1251, pg_get_client_encoding());
		strncpy(dst, cnv, dst_size);
		if (cnv == src && src_size < dst_size) { 
			dst[src_size] = '\0';
			return src_size;
		}
		return strlen(dst);
	}
	PG_CATCH();
    {
		return (size_t)-1;
	}
    PG_END_TRY();
}

Datum
rusmorph_init(PG_FUNCTION_ARGS)
{
	PG_RETURN_POINTER(palloc(1));
}

Datum
rusmorph_lexize(PG_FUNCTION_ARGS)
{
	IMlmaMb* pmorph;
	char* in = (char *) PG_GETARG_POINTER(1);
	int			length = PG_GETARG_INT32(2);
	SLemmInfoA  lemmas[0x20];
	char        normal[0x100];
	SGramInfo   agrams[0x40];
	int         i, nlemma;
	TSLexeme   *res;

	if (!length)
        PG_RETURN_POINTER(NULL);  

	mlmaruLoadMbAPI(&pmorph);

	nlemma = pmorph->vtbl->Lemmatize(pmorph, in, length, lemmas, 0x20, normal, 0x100, agrams, 0x40, sfIgnoreCapitals);
	if (nlemma == 0) { 
		PG_RETURN_POINTER(NULL);
	}
	res = palloc0(sizeof(TSLexeme) * (nlemma+1));

	for (i = 0; i < nlemma; i++) 
	{
		res[i].lexeme = pstrdup(lemmas[i].plemma);
	}
	res[i].lexeme = NULL;
	PG_RETURN_POINTER(res);
}
