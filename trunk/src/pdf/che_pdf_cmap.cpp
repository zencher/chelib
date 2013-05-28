#include "../../include/pdf/che_pdf_cmap.h"
#include "../../include/pdf/che_pdf_cmap_res_cns.h"
#include "../../include/pdf/che_pdf_cmap_res_gb.h"
#include "../../include/pdf/che_pdf_cmap_res_japan.h"
#include "../../include/pdf/che_pdf_cmap_res_korea.h"

#include <string.h>

#define pdf_range_high(r) ((r)->low + ((r)->extent_flags >> 2))
#define pdf_range_flags(r) ((r)->extent_flags & 3)
#define pdf_range_set_high(r, h) ((r)->extent_flags = (((r)->extent_flags & 3) | ((h - (r)->low) << 2)))
#define pdf_range_set_flags(r, f) ((r)->extent_flags = (((r)->extent_flags & ~3) | f))

const struct { HE_CHAR * name; PDF_CMAP * cmap; } cmap_table[] =
{
	{"78-EUC-H",&CMap_78_EUC_H},
	{"78-EUC-V",&CMap_78_EUC_V},
	{"78-H",&CMap_78_H},
	{"78-RKSJ-H",&CMap_78_RKSJ_H},
	{"78-RKSJ-V",&CMap_78_RKSJ_V},
	{"78-V",&CMap_78_V},
	{"78ms-RKSJ-H",&CMap_78ms_RKSJ_H},
	{"78ms-RKSJ-V",&CMap_78ms_RKSJ_V},
	{"83pv-RKSJ-H",&CMap_83pv_RKSJ_H},
	{"90ms-RKSJ-H",&CMap_90ms_RKSJ_H},
	{"90ms-RKSJ-V",&CMap_90ms_RKSJ_V},
	{"90msp-RKSJ-H",&CMap_90msp_RKSJ_H},
	{"90msp-RKSJ-V",&CMap_90msp_RKSJ_V},
	{"90pv-RKSJ-H",&CMap_90pv_RKSJ_H},
	{"90pv-RKSJ-V",&CMap_90pv_RKSJ_V},
	{"Add-H",&CMap_Add_H},
	{"Add-RKSJ-H",&CMap_Add_RKSJ_H},
	{"Add-RKSJ-V",&CMap_Add_RKSJ_V},
	{"Add-V",&CMap_Add_V},
	{"Adobe-CNS1-0",&CMap_Adobe_CNS1_0},
	{"Adobe-CNS1-1",&CMap_Adobe_CNS1_1},
	{"Adobe-CNS1-2",&CMap_Adobe_CNS1_2},
	{"Adobe-CNS1-3",&CMap_Adobe_CNS1_3},
	{"Adobe-CNS1-4",&CMap_Adobe_CNS1_4},
	{"Adobe-CNS1-5",&CMap_Adobe_CNS1_5},
	{"Adobe-CNS1-6",&CMap_Adobe_CNS1_6},
	{"Adobe-CNS1-UCS2",&CMap_Adobe_CNS1_UCS2},
	{"Adobe-GB1-0",&CMap_Adobe_GB1_0},
	{"Adobe-GB1-1",&CMap_Adobe_GB1_1},
	{"Adobe-GB1-2",&CMap_Adobe_GB1_2},
	{"Adobe-GB1-3",&CMap_Adobe_GB1_3},
	{"Adobe-GB1-4",&CMap_Adobe_GB1_4},
	{"Adobe-GB1-5",&CMap_Adobe_GB1_5},
	{"Adobe-GB1-UCS2",&CMap_Adobe_GB1_UCS2},
	{"Adobe-Japan1-0",&CMap_Adobe_Japan1_0},
	{"Adobe-Japan1-1",&CMap_Adobe_Japan1_1},
	{"Adobe-Japan1-2",&CMap_Adobe_Japan1_2},
	{"Adobe-Japan1-3",&CMap_Adobe_Japan1_3},
	{"Adobe-Japan1-4",&CMap_Adobe_Japan1_4},
	{"Adobe-Japan1-5",&CMap_Adobe_Japan1_5},
	{"Adobe-Japan1-6",&CMap_Adobe_Japan1_6},
	{"Adobe-Japan1-UCS2",&CMap_Adobe_Japan1_UCS2},
	{"Adobe-Japan2-0",&CMap_Adobe_Japan2_0},
	{"Adobe-Korea1-0",&CMap_Adobe_Korea1_0},
	{"Adobe-Korea1-1",&CMap_Adobe_Korea1_1},
	{"Adobe-Korea1-2",&CMap_Adobe_Korea1_2},
	{"Adobe-Korea1-UCS2",&CMap_Adobe_Korea1_UCS2},
	{"B5-H",&CMap_B5_H},
	{"B5-V",&CMap_B5_V},
	{"B5pc-H",&CMap_B5pc_H},
	{"B5pc-V",&CMap_B5pc_V},
	{"CNS-EUC-H",&CMap_CNS_EUC_H},
	{"CNS-EUC-V",&CMap_CNS_EUC_V},
	{"CNS1-H",&CMap_CNS1_H},
	{"CNS1-V",&CMap_CNS1_V},
	{"CNS2-H",&CMap_CNS2_H},
	{"CNS2-V",&CMap_CNS2_V},
	{"ETHK-B5-H",&CMap_ETHK_B5_H},
	{"ETHK-B5-V",&CMap_ETHK_B5_V},
	{"ETen-B5-H",&CMap_ETen_B5_H},
	{"ETen-B5-V",&CMap_ETen_B5_V},
	{"ETenms-B5-H",&CMap_ETenms_B5_H},
	{"ETenms-B5-V",&CMap_ETenms_B5_V},
	{"EUC-H",&CMap_EUC_H},
	{"EUC-V",&CMap_EUC_V},
	{"Ext-H",&CMap_Ext_H},
	{"Ext-RKSJ-H",&CMap_Ext_RKSJ_H},
	{"Ext-RKSJ-V",&CMap_Ext_RKSJ_V},
	{"Ext-V",&CMap_Ext_V},
	{"GB-EUC-H",&CMap_GB_EUC_H},
	{"GB-EUC-V",&CMap_GB_EUC_V},
	{"GB-H",&CMap_GB_H},
	{"GB-V",&CMap_GB_V},
	{"GBK-EUC-H",&CMap_GBK_EUC_H},
	{"GBK-EUC-V",&CMap_GBK_EUC_V},
	{"GBK2K-H",&CMap_GBK2K_H},
	{"GBK2K-V",&CMap_GBK2K_V},
	{"GBKp-EUC-H",&CMap_GBKp_EUC_H},
	{"GBKp-EUC-V",&CMap_GBKp_EUC_V},
	{"GBT-EUC-H",&CMap_GBT_EUC_H},
	{"GBT-EUC-V",&CMap_GBT_EUC_V},
	{"GBT-H",&CMap_GBT_H},
	{"GBT-V",&CMap_GBT_V},
	{"GBTpc-EUC-H",&CMap_GBTpc_EUC_H},
	{"GBTpc-EUC-V",&CMap_GBTpc_EUC_V},
	{"GBpc-EUC-H",&CMap_GBpc_EUC_H},
	{"GBpc-EUC-V",&CMap_GBpc_EUC_V},
	{"H",&CMap_H},
	{"HKdla-B5-H",&CMap_HKdla_B5_H},
	{"HKdla-B5-V",&CMap_HKdla_B5_V},
	{"HKdlb-B5-H",&CMap_HKdlb_B5_H},
	{"HKdlb-B5-V",&CMap_HKdlb_B5_V},
	{"HKgccs-B5-H",&CMap_HKgccs_B5_H},
	{"HKgccs-B5-V",&CMap_HKgccs_B5_V},
	{"HKm314-B5-H",&CMap_HKm314_B5_H},
	{"HKm314-B5-V",&CMap_HKm314_B5_V},
	{"HKm471-B5-H",&CMap_HKm471_B5_H},
	{"HKm471-B5-V",&CMap_HKm471_B5_V},
	{"HKscs-B5-H",&CMap_HKscs_B5_H},
	{"HKscs-B5-V",&CMap_HKscs_B5_V},
	{"Hankaku",&CMap_Hankaku},
	{"Hiragana",&CMap_Hiragana},
	{"Hojo-EUC-H",&CMap_Hojo_EUC_H},
	{"Hojo-EUC-V",&CMap_Hojo_EUC_V},
	{"Hojo-H",&CMap_Hojo_H},
	{"Hojo-V",&CMap_Hojo_V},
	{"KSC-EUC-H",&CMap_KSC_EUC_H},
	{"KSC-EUC-V",&CMap_KSC_EUC_V},
	{"KSC-H",&CMap_KSC_H},
	{"KSC-Johab-H",&CMap_KSC_Johab_H},
	{"KSC-Johab-V",&CMap_KSC_Johab_V},
	{"KSC-V",&CMap_KSC_V},
	{"KSCms-UHC-H",&CMap_KSCms_UHC_H},
	{"KSCms-UHC-HW-H",&CMap_KSCms_UHC_HW_H},
	{"KSCms-UHC-HW-V",&CMap_KSCms_UHC_HW_V},
	{"KSCms-UHC-V",&CMap_KSCms_UHC_V},
	{"KSCpc-EUC-H",&CMap_KSCpc_EUC_H},
	{"KSCpc-EUC-V",&CMap_KSCpc_EUC_V},
	{"Katakana",&CMap_Katakana},
	{"NWP-H",&CMap_NWP_H},
	{"NWP-V",&CMap_NWP_V},
	{"RKSJ-H",&CMap_RKSJ_H},
	{"RKSJ-V",&CMap_RKSJ_V},
	{"Roman",&CMap_Roman},
	{"UniCNS-UCS2-H",&CMap_UniCNS_UCS2_H},
	{"UniCNS-UCS2-V",&CMap_UniCNS_UCS2_V},
	{"UniCNS-UTF16-H",&CMap_UniCNS_UTF16_H},
	{"UniCNS-UTF16-V",&CMap_UniCNS_UTF16_V},
	{"UniGB-UCS2-H",&CMap_UniGB_UCS2_H},
	{"UniGB-UCS2-V",&CMap_UniGB_UCS2_V},
	{"UniGB-UTF16-H",&CMap_UniGB_UTF16_H},
	{"UniGB-UTF16-V",&CMap_UniGB_UTF16_V},
	{"UniHojo-UCS2-H",&CMap_UniHojo_UCS2_H},
	{"UniHojo-UCS2-V",&CMap_UniHojo_UCS2_V},
	{"UniHojo-UTF16-H",&CMap_UniHojo_UTF16_H},
	{"UniHojo-UTF16-V",&CMap_UniHojo_UTF16_V},
	{"UniJIS-UCS2-H",&CMap_UniJIS_UCS2_H},
	{"UniJIS-UCS2-HW-H",&CMap_UniJIS_UCS2_HW_H},
	{"UniJIS-UCS2-HW-V",&CMap_UniJIS_UCS2_HW_V},
	{"UniJIS-UCS2-V",&CMap_UniJIS_UCS2_V},
	{"UniJIS-UTF16-H",&CMap_UniJIS_UTF16_H},
	{"UniJIS-UTF16-V",&CMap_UniJIS_UTF16_V},
	{"UniJISPro-UCS2-HW-V",&CMap_UniJISPro_UCS2_HW_V},
	{"UniJISPro-UCS2-V",&CMap_UniJISPro_UCS2_V},
	{"UniKS-UCS2-H",&CMap_UniKS_UCS2_H},
	{"UniKS-UCS2-V",&CMap_UniKS_UCS2_V},
	{"UniKS-UTF16-H",&CMap_UniKS_UTF16_H},
	{"UniKS-UTF16-V",&CMap_UniKS_UTF16_V},
	{"V",&CMap_V},
	{"WP-Symbol",&CMap_WP_Symbol},
};

PDF_CMAP * Get_Builtin_CMap( HE_CHAR * cmap_name )
{
	HE_INT32 l = 0;
	HE_INT32 r = sizeof( cmap_table ) / sizeof( (cmap_table)[0] ) - 1;
	while (l <= r)
	{
		HE_INT32 m = (l + r) >> 1;
		HE_INT32 c = strcmp( cmap_name, cmap_table[m].name );
		if ( c < 0 )
			r = m - 1;
		else if ( c > 0 )
			l = m + 1;
		else
			return cmap_table[m].cmap;
	}
	return NULL;
}


HE_BOOL LookUp_CMap( PDF_CMAP * cmap, HE_DWORD cpt, HE_DWORD & codeRet )
{
	HE_INT32 l = 0;
	HE_INT32 r = cmap->rlen - 1;
	HE_INT32 m = 0;

	while ( l <= r )
	{
		m = (l + r) >> 1;
		if ( cpt < cmap->ranges[m].low )
			r = m - 1;
		else if ( cpt > pdf_range_high( &cmap->ranges[m] ) )
			l = m + 1;
		else
		{
			HE_DWORD i = cpt - cmap->ranges[m].low + cmap->ranges[m].offset;
			if ( pdf_range_flags( &cmap->ranges[m] ) == CMAP_TABLE )
			{
				codeRet = cmap->table[i];
				return TRUE;
			}
			if ( pdf_range_flags( &cmap->ranges[m] ) == CMAP_MULTI )
			{
				return FALSE; /* should use lookup_cmap_full */
			}
			codeRet = i;
			return TRUE;
		}
	}

	if ( cmap->usecmap )
		return LookUp_CMap( cmap->usecmap, cpt, codeRet );

	return FALSE;
}

HE_INT32 LookUp_CMap_Full( PDF_CMAP *cmap, HE_INT32 cpt, HE_INT32 *out )
{
	HE_INT32 i, k, n;
	HE_INT32 l = 0;
	HE_INT32 r = cmap->rlen - 1;
	HE_INT32 m = 0;

	while ( l <= r )
	{
		m = ( l + r ) >> 1;
		if ( cpt < cmap->ranges[m].low )
			r = m - 1;
		else if ( cpt > pdf_range_high( &cmap->ranges[m] ) )
			l = m + 1;
		else
		{
			k = cpt - cmap->ranges[m].low + cmap->ranges[m].offset;
			if ( pdf_range_flags( &cmap->ranges[m] ) == CMAP_TABLE )
			{
				out[0] = cmap->table[k];
				return 1;
			}
			else if  (pdf_range_flags( &cmap->ranges[m] ) == CMAP_MULTI )
			{
				n = cmap->ranges[m].offset;
				for ( i = 0; i < cmap->table[n]; i++ )
					out[i] = cmap->table[n + i + 1];
				return cmap->table[n];
			}
			else
			{
				out[0] = k;
				return 1;
			}
		}
	}

	if ( cmap->usecmap )
		return LookUp_CMap_Full( cmap->usecmap, cpt, out );

	return 0;
}

CHE_PDF_CMap * CHE_PDF_CMap::LoadBuildinCMap( const CHE_ByteString & cmapName,  CHE_Allocator * pAllocator )
{
	PDF_CMAP * pRet = Get_Builtin_CMap( (HE_CHAR *)( cmapName.GetData() ) );
	if ( !pRet )
	{
		return NULL;
	}
	if ( pAllocator )
	{
		return pAllocator->New<CHE_PDF_CMap>( pRet, false, pAllocator );
	}
	return new CHE_PDF_CMap( pRet, false, NULL ); 
}

CHE_PDF_CMap::~CHE_PDF_CMap()
{
	if ( mbNeedClear )
	{
		GetAllocator()->Delete( mpCMap );
	}
}

HE_BOOL CHE_PDF_CMap::LookupCode( HE_DWORD code, HE_DWORD & codeRet ) const
{
	if ( mpCMap )
	{
		return LookUp_CMap( mpCMap, code, codeRet );
	}
	return FALSE;
}