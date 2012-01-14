#ifndef _CHE_PDF_GRAPHICS_STATE_H_
#define _CHE_PDF_GRAPHICS_STATE_H_

enum PDF_GSTATE_LINECAP
{
	LineCap_Butt = 0,
	LineCap_Round = 1,
	LineCap_Square = 2
};

enum PDF_GSTATE_LINEJOIN
{
	LineJoin_Miter = 0,
	LineJoin_Round = 1,
	LineJoin_Bevel = 2
};

struct PDF_GSTATE_DASHPATTERN
{
	PDF_GSTATE_DASHPATTERN()
	{
		dashPhase = 0;
	}
	float dashPhase;
	std::vector<float> dashArray;
};

enum PDF_GSTATE_RENDERINTENTS
{
	RI_AbsoluteColorimetric = 0,
	RI_RelativeColorimetric = 1,
	RI_Saturation = 2,
	RI_Perceptual = 3
};

enum PDF_GSTATE_TEXTRENDERMODE
{
	TextRenderMode_Fill = 0,
	TextRenderMode_Stroke = 1,
	TextRenderMode_FillStroke = 2,
	TextRenderMode_Invisible = 3,
	TextRenderMode_FillClip = 4,
	TextRenderMode_StrokeClip = 5,
	TextRenderMode_FillStrokeClip = 6,
	TextRenderMode_Clip = 7
};

#endif