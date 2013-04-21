#ifndef _CHE_PDF_CMAP_RES_H_
#define _CHE_PDF_CMAP_RES_H_

struct PDF_CMAP_RANGE
{
	HE_WORD low;
	/* Next, we pack 2 fields into the same unsigned short. Top 14 bits
	 * are the extent, bottom 2 bits are flags: single, range, table,
	 * multi */
	HE_WORD extent_flags;
	HE_WORD offset;	/* range-delta or table-index */
};

struct PDF_CMAP
{
	HE_INT32 refs;
	HE_CHAR cmap_name[32];

	HE_CHAR usecmap_name[32];
	PDF_CMAP *usecmap;

	HE_INT32 wmode;

	HE_INT32 codespace_len;
	struct
	{
		HE_WORD n;
		HE_WORD low;
		HE_WORD high;
	} codespace[40];

	HE_INT32 rlen, rcap;
	PDF_CMAP_RANGE *ranges;

	HE_INT32 tlen, tcap;
	HE_WORD *table;
};

enum PDF_CMAP_TYPE
{
	CMAP_SINGLE,
	CMAP_RANGE,
	CMAP_TABLE,
	CMAP_MULTI
};

#endif