#ifndef _CHE_PDF_CMAP_RES_H_
#define _CHE_PDF_CMAP_RES_H_

struct PDF_CMAP_RANGE
{
	uint16 low;
	/* Next, we pack 2 fields into the same unsigned short. Top 14 bits
	 * are the extent, bottom 2 bits are flags: single, range, table,
	 * multi */
	uint16 extent_flags;
	uint16 offset;	/* range-delta or table-index */
};

struct PDF_CMAP
{
	int32 refs;
	char cmap_name[32];

	char usecmap_name[32];
	PDF_CMAP *usecmap;

	int32 wmode;

	int32 codespace_len;
	struct
	{
		uint16 n;
		uint16 low;
		uint16 high;
	} codespace[40];

	int32 rlen, rcap;
	PDF_CMAP_RANGE *ranges;

	int32 tlen, tcap;
	uint16 *table;
};

enum PDF_CMAP_TYPE
{
	CMAP_SINGLE,
	CMAP_RANGE,
	CMAP_TABLE,
	CMAP_MULTI
};

#endif