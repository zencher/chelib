#include "../../include/che_datastructure.h"
#include "../../include/pdf/che_pdf_font.h"
#include "../../include/pdf/che_pdf_parser.h"
#include "../../include/pdf/che_pdf_contentobjs.h"

#include "../../extlib/freetype/include/ft2build.h"
#include "../../extlib/freetype/include/freetype/freetype.h"
#include "../../extlib/freetype/include/freetype/ftoutln.h"

FT_Library HE_GetFTLibrary();
HE_VOID HE_InitFTLibrary();
HE_VOID HE_DestroyFTLibrary();

FT_Library gFTLibrary = NULL;

IHE_SystemFontMgr * gpSystemFontMgr = NULL;


#define _notdef NULL


const char * const pdf_standard[256] = { _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	"space", "exclam", "quotedbl", "numbersign", "dollar", "percent",
	"ampersand", "quoteright", "parenleft", "parenright", "asterisk",
	"plus", "comma", "hyphen", "period", "slash", "zero", "one", "two",
	"three", "four", "five", "six", "seven", "eight", "nine", "colon",
	"semicolon", "less", "equal", "greater", "question", "at", "A",
	"B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N",
	"O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
	"bracketleft", "backslash", "bracketright", "asciicircum", "underscore",
	"quoteleft", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
	"l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
	"y", "z", "braceleft", "bar", "braceright", "asciitilde", _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, "exclamdown", "cent", "sterling",
	"fraction", "yen", "florin", "section", "currency", "quotesingle",
	"quotedblleft", "guillemotleft", "guilsinglleft", "guilsinglright",
	"fi", "fl", _notdef, "endash", "dagger", "daggerdbl", "periodcentered",
	_notdef, "paragraph", "bullet", "quotesinglbase", "quotedblbase",
	"quotedblright", "guillemotright", "ellipsis", "perthousand",
	_notdef, "questiondown", _notdef, "grave", "acute", "circumflex",
	"tilde", "macron", "breve", "dotaccent", "dieresis", _notdef,
	"ring", "cedilla", _notdef, "hungarumlaut", "ogonek", "caron",
	"emdash", _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, "AE",
	_notdef, "ordfeminine", _notdef, _notdef, _notdef, _notdef,
	"Lslash", "Oslash", "OE", "ordmasculine", _notdef, _notdef,
	_notdef, _notdef, _notdef, "ae", _notdef, _notdef,
	_notdef, "dotlessi", _notdef, _notdef, "lslash", "oslash",
	"oe", "germandbls", _notdef, _notdef, _notdef, _notdef
};

const char * const pdf_mac_roman[256] = { _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	"space", "exclam", "quotedbl", "numbersign", "dollar", "percent",
	"ampersand", "quotesingle", "parenleft", "parenright", "asterisk",
	"plus", "comma", "hyphen", "period", "slash", "zero", "one", "two",
	"three", "four", "five", "six", "seven", "eight", "nine", "colon",
	"semicolon", "less", "equal", "greater", "question", "at", "A",
	"B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N",
	"O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
	"bracketleft", "backslash", "bracketright", "asciicircum", "underscore",
	"grave", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
	"l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
	"y", "z", "braceleft", "bar", "braceright", "asciitilde", _notdef,
	"Adieresis", "Aring", "Ccedilla", "Eacute", "Ntilde", "Odieresis",
	"Udieresis", "aacute", "agrave", "acircumflex", "adieresis", "atilde",
	"aring", "ccedilla", "eacute", "egrave", "ecircumflex", "edieresis",
	"iacute", "igrave", "icircumflex", "idieresis", "ntilde", "oacute",
	"ograve", "ocircumflex", "odieresis", "otilde", "uacute", "ugrave",
	"ucircumflex", "udieresis", "dagger", "degree", "cent", "sterling",
	"section", "bullet", "paragraph", "germandbls", "registered",
	"copyright", "trademark", "acute", "dieresis", _notdef, "AE",
	"Oslash", _notdef, "plusminus", _notdef, _notdef, "yen", "mu",
	_notdef, _notdef, _notdef, _notdef, _notdef, "ordfeminine",
	"ordmasculine", _notdef, "ae", "oslash", "questiondown", "exclamdown",
	"logicalnot", _notdef, "florin", _notdef, _notdef, "guillemotleft",
	"guillemotright", "ellipsis", "space", "Agrave", "Atilde", "Otilde",
	"OE", "oe", "endash", "emdash", "quotedblleft", "quotedblright",
	"quoteleft", "quoteright", "divide", _notdef, "ydieresis",
	"Ydieresis", "fraction", "currency", "guilsinglleft", "guilsinglright",
	"fi", "fl", "daggerdbl", "periodcentered", "quotesinglbase",
	"quotedblbase", "perthousand", "Acircumflex", "Ecircumflex", "Aacute",
	"Edieresis", "Egrave", "Iacute", "Icircumflex", "Idieresis", "Igrave",
	"Oacute", "Ocircumflex", _notdef, "Ograve", "Uacute", "Ucircumflex",
	"Ugrave", "dotlessi", "circumflex", "tilde", "macron", "breve",
	"dotaccent", "ring", "cedilla", "hungarumlaut", "ogonek", "caron"
};

const char * const pdf_mac_expert[256] = { _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	"space", "exclamsmall", "Hungarumlautsmall", "centoldstyle",
	"dollaroldstyle", "dollarsuperior", "ampersandsmall", "Acutesmall",
	"parenleftsuperior", "parenrightsuperior", "twodotenleader",
	"onedotenleader", "comma", "hyphen", "period", "fraction",
	"zerooldstyle", "oneoldstyle", "twooldstyle", "threeoldstyle",
	"fouroldstyle", "fiveoldstyle", "sixoldstyle", "sevenoldstyle",
	"eightoldstyle", "nineoldstyle", "colon", "semicolon", _notdef,
	"threequartersemdash", _notdef, "questionsmall", _notdef,
	_notdef, _notdef, _notdef, "Ethsmall", _notdef, _notdef,
	"onequarter", "onehalf", "threequarters", "oneeighth", "threeeighths",
	"fiveeighths", "seveneighths", "onethird", "twothirds", _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, "ff", "fi",
	"fl", "ffi", "ffl", "parenleftinferior", _notdef, "parenrightinferior",
	"Circumflexsmall", "hypheninferior", "Gravesmall", "Asmall", "Bsmall",
	"Csmall", "Dsmall", "Esmall", "Fsmall", "Gsmall", "Hsmall", "Ismall",
	"Jsmall", "Ksmall", "Lsmall", "Msmall", "Nsmall", "Osmall", "Psmall",
	"Qsmall", "Rsmall", "Ssmall", "Tsmall", "Usmall", "Vsmall", "Wsmall",
	"Xsmall", "Ysmall", "Zsmall", "colonmonetary", "onefitted", "rupiah",
	"Tildesmall", _notdef, _notdef, "asuperior", "centsuperior",
	_notdef, _notdef, _notdef, _notdef, "Aacutesmall",
	"Agravesmall", "Acircumflexsmall", "Adieresissmall", "Atildesmall",
	"Aringsmall", "Ccedillasmall", "Eacutesmall", "Egravesmall",
	"Ecircumflexsmall", "Edieresissmall", "Iacutesmall", "Igravesmall",
	"Icircumflexsmall", "Idieresissmall", "Ntildesmall", "Oacutesmall",
	"Ogravesmall", "Ocircumflexsmall", "Odieresissmall", "Otildesmall",
	"Uacutesmall", "Ugravesmall", "Ucircumflexsmall", "Udieresissmall",
	_notdef, "eightsuperior", "fourinferior", "threeinferior",
	"sixinferior", "eightinferior", "seveninferior", "Scaronsmall",
	_notdef, "centinferior", "twoinferior", _notdef, "Dieresissmall",
	_notdef, "Caronsmall", "osuperior", "fiveinferior", _notdef,
	"commainferior", "periodinferior", "Yacutesmall", _notdef,
	"dollarinferior", _notdef, _notdef, "Thornsmall", _notdef,
	"nineinferior", "zeroinferior", "Zcaronsmall", "AEsmall", "Oslashsmall",
	"questiondownsmall", "oneinferior", "Lslashsmall", _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, "Cedillasmall",
	_notdef, _notdef, _notdef, _notdef, _notdef, "OEsmall",
	"figuredash", "hyphensuperior", _notdef, _notdef, _notdef,
	_notdef, "exclamdownsmall", _notdef, "Ydieresissmall", _notdef,
	"onesuperior", "twosuperior", "threesuperior", "foursuperior",
	"fivesuperior", "sixsuperior", "sevensuperior", "ninesuperior",
	"zerosuperior", _notdef, "esuperior", "rsuperior", "tsuperior",
	_notdef, _notdef, "isuperior", "ssuperior", "dsuperior",
	_notdef, _notdef, _notdef, _notdef, _notdef, "lsuperior",
	"Ogoneksmall", "Brevesmall", "Macronsmall", "bsuperior", "nsuperior",
	"msuperior", "commasuperior", "periodsuperior", "Dotaccentsmall",
	"Ringsmall", _notdef, _notdef, _notdef, _notdef };

const char * const pdf_win_ansi[256] = { _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, _notdef,
	_notdef, _notdef, _notdef, _notdef, _notdef, "space",
	"exclam", "quotedbl", "numbersign", "dollar", "percent", "ampersand",
	"quotesingle", "parenleft", "parenright", "asterisk", "plus",
	"comma", "hyphen", "period", "slash", "zero", "one", "two", "three",
	"four", "five", "six", "seven", "eight", "nine", "colon", "semicolon",
	"less", "equal", "greater", "question", "at", "A", "B", "C", "D",
	"E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q",
	"R", "S", "T", "U", "V", "W", "X", "Y", "Z", "bracketleft",
	"backslash", "bracketright", "asciicircum", "underscore", "grave",
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
	"n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
	"braceleft", "bar", "braceright", "asciitilde", "bullet", "Euro",
	"bullet", "quotesinglbase", "florin", "quotedblbase", "ellipsis",
	"dagger", "daggerdbl", "circumflex", "perthousand", "Scaron",
	"guilsinglleft", "OE", "bullet", "Zcaron", "bullet", "bullet",
	"quoteleft", "quoteright", "quotedblleft", "quotedblright", "bullet",
	"endash", "emdash", "tilde", "trademark", "scaron", "guilsinglright",
	"oe", "bullet", "zcaron", "Ydieresis", "space", "exclamdown", "cent",
	"sterling", "currency", "yen", "brokenbar", "section", "dieresis",
	"copyright", "ordfeminine", "guillemotleft", "logicalnot", "hyphen",
	"registered", "macron", "degree", "plusminus", "twosuperior",
	"threesuperior", "acute", "mu", "paragraph", "periodcentered",
	"cedilla", "onesuperior", "ordmasculine", "guillemotright",
	"onequarter", "onehalf", "threequarters", "questiondown", "Agrave",
	"Aacute", "Acircumflex", "Atilde", "Adieresis", "Aring", "AE",
	"Ccedilla", "Egrave", "Eacute", "Ecircumflex", "Edieresis", "Igrave",
	"Iacute", "Icircumflex", "Idieresis", "Eth", "Ntilde", "Ograve",
	"Oacute", "Ocircumflex", "Otilde", "Odieresis", "multiply", "Oslash",
	"Ugrave", "Uacute", "Ucircumflex", "Udieresis", "Yacute", "Thorn",
	"germandbls", "agrave", "aacute", "acircumflex", "atilde", "adieresis",
	"aring", "ae", "ccedilla", "egrave", "eacute", "ecircumflex",
	"edieresis", "igrave", "iacute", "icircumflex", "idieresis", "eth",
	"ntilde", "ograve", "oacute", "ocircumflex", "otilde", "odieresis",
	"divide", "oslash", "ugrave", "uacute", "ucircumflex", "udieresis",
	"yacute", "thorn", "ydieresis"
};


static const char *agl_name_list[] = {
	"A","AE","AEacute","AEmacron","AEsmall","Aacute","Aacutesmall","Abreve",
	"Abreveacute","Abrevecyrillic","Abrevedotbelow","Abrevegrave",
	"Abrevehookabove","Abrevetilde","Acaron","Acircle","Acircumflex",
	"Acircumflexacute","Acircumflexdotbelow","Acircumflexgrave",
	"Acircumflexhookabove","Acircumflexsmall","Acircumflextilde","Acute",
	"Acutesmall","Acyrillic","Adblgrave","Adieresis","Adieresiscyrillic",
	"Adieresismacron","Adieresissmall","Adotbelow","Adotmacron","Agrave",
	"Agravesmall","Ahookabove","Aiecyrillic","Ainvertedbreve","Alpha",
	"Alphatonos","Amacron","Amonospace","Aogonek","Aring","Aringacute",
	"Aringbelow","Aringsmall","Asmall","Atilde","Atildesmall","Aybarmenian","B",
	"Bcircle","Bdotaccent","Bdotbelow","Becyrillic","Benarmenian","Beta","Bhook",
	"Blinebelow","Bmonospace","Brevesmall","Bsmall","Btopbar","C","Caarmenian",
	"Cacute","Caron","Caronsmall","Ccaron","Ccedilla","Ccedillaacute",
	"Ccedillasmall","Ccircle","Ccircumflex","Cdot","Cdotaccent","Cedillasmall",
	"Chaarmenian","Cheabkhasiancyrillic","Checyrillic",
	"Chedescenderabkhasiancyrillic","Chedescendercyrillic","Chedieresiscyrillic",
	"Cheharmenian","Chekhakassiancyrillic","Cheverticalstrokecyrillic","Chi",
	"Chook","Circumflexsmall","Cmonospace","Coarmenian","Csmall","D","DZ",
	"DZcaron","Daarmenian","Dafrican","Dcaron","Dcedilla","Dcircle",
	"Dcircumflexbelow","Dcroat","Ddotaccent","Ddotbelow","Decyrillic","Deicoptic",
	"Delta","Deltagreek","Dhook","Dieresis","DieresisAcute","DieresisGrave",
	"Dieresissmall","Digammagreek","Djecyrillic","Dlinebelow","Dmonospace",
	"Dotaccentsmall","Dslash","Dsmall","Dtopbar","Dz","Dzcaron",
	"Dzeabkhasiancyrillic","Dzecyrillic","Dzhecyrillic","E","Eacute",
	"Eacutesmall","Ebreve","Ecaron","Ecedillabreve","Echarmenian","Ecircle",
	"Ecircumflex","Ecircumflexacute","Ecircumflexbelow","Ecircumflexdotbelow",
	"Ecircumflexgrave","Ecircumflexhookabove","Ecircumflexsmall",
	"Ecircumflextilde","Ecyrillic","Edblgrave","Edieresis","Edieresissmall",
	"Edot","Edotaccent","Edotbelow","Efcyrillic","Egrave","Egravesmall",
	"Eharmenian","Ehookabove","Eightroman","Einvertedbreve","Eiotifiedcyrillic",
	"Elcyrillic","Elevenroman","Emacron","Emacronacute","Emacrongrave",
	"Emcyrillic","Emonospace","Encyrillic","Endescendercyrillic","Eng",
	"Enghecyrillic","Enhookcyrillic","Eogonek","Eopen","Epsilon","Epsilontonos",
	"Ercyrillic","Ereversed","Ereversedcyrillic","Escyrillic",
	"Esdescendercyrillic","Esh","Esmall","Eta","Etarmenian","Etatonos","Eth",
	"Ethsmall","Etilde","Etildebelow","Euro","Ezh","Ezhcaron","Ezhreversed","F",
	"Fcircle","Fdotaccent","Feharmenian","Feicoptic","Fhook","Fitacyrillic",
	"Fiveroman","Fmonospace","Fourroman","Fsmall","G","GBsquare","Gacute","Gamma",
	"Gammaafrican","Gangiacoptic","Gbreve","Gcaron","Gcedilla","Gcircle",
	"Gcircumflex","Gcommaaccent","Gdot","Gdotaccent","Gecyrillic","Ghadarmenian",
	"Ghemiddlehookcyrillic","Ghestrokecyrillic","Gheupturncyrillic","Ghook",
	"Gimarmenian","Gjecyrillic","Gmacron","Gmonospace","Grave","Gravesmall",
	"Gsmall","Gsmallhook","Gstroke","H","H18533","H18543","H18551","H22073",
	"HPsquare","Haabkhasiancyrillic","Hadescendercyrillic","Hardsigncyrillic",
	"Hbar","Hbrevebelow","Hcedilla","Hcircle","Hcircumflex","Hdieresis",
	"Hdotaccent","Hdotbelow","Hmonospace","Hoarmenian","Horicoptic","Hsmall",
	"Hungarumlaut","Hungarumlautsmall","Hzsquare","I","IAcyrillic","IJ",
	"IUcyrillic","Iacute","Iacutesmall","Ibreve","Icaron","Icircle","Icircumflex",
	"Icircumflexsmall","Icyrillic","Idblgrave","Idieresis","Idieresisacute",
	"Idieresiscyrillic","Idieresissmall","Idot","Idotaccent","Idotbelow",
	"Iebrevecyrillic","Iecyrillic","Ifraktur","Igrave","Igravesmall","Ihookabove",
	"Iicyrillic","Iinvertedbreve","Iishortcyrillic","Imacron","Imacroncyrillic",
	"Imonospace","Iniarmenian","Iocyrillic","Iogonek","Iota","Iotaafrican",
	"Iotadieresis","Iotatonos","Ismall","Istroke","Itilde","Itildebelow",
	"Izhitsacyrillic","Izhitsadblgravecyrillic","J","Jaarmenian","Jcircle",
	"Jcircumflex","Jecyrillic","Jheharmenian","Jmonospace","Jsmall","K",
	"KBsquare","KKsquare","Kabashkircyrillic","Kacute","Kacyrillic",
	"Kadescendercyrillic","Kahookcyrillic","Kappa","Kastrokecyrillic",
	"Kaverticalstrokecyrillic","Kcaron","Kcedilla","Kcircle","Kcommaaccent",
	"Kdotbelow","Keharmenian","Kenarmenian","Khacyrillic","Kheicoptic","Khook",
	"Kjecyrillic","Klinebelow","Kmonospace","Koppacyrillic","Koppagreek",
	"Ksicyrillic","Ksmall","L","LJ","LL","Lacute","Lambda","Lcaron","Lcedilla",
	"Lcircle","Lcircumflexbelow","Lcommaaccent","Ldot","Ldotaccent","Ldotbelow",
	"Ldotbelowmacron","Liwnarmenian","Lj","Ljecyrillic","Llinebelow","Lmonospace",
	"Lslash","Lslashsmall","Lsmall","M","MBsquare","Macron","Macronsmall",
	"Macute","Mcircle","Mdotaccent","Mdotbelow","Menarmenian","Mmonospace",
	"Msmall","Mturned","Mu","N","NJ","Nacute","Ncaron","Ncedilla","Ncircle",
	"Ncircumflexbelow","Ncommaaccent","Ndotaccent","Ndotbelow","Nhookleft",
	"Nineroman","Nj","Njecyrillic","Nlinebelow","Nmonospace","Nowarmenian",
	"Nsmall","Ntilde","Ntildesmall","Nu","O","OE","OEsmall","Oacute",
	"Oacutesmall","Obarredcyrillic","Obarreddieresiscyrillic","Obreve","Ocaron",
	"Ocenteredtilde","Ocircle","Ocircumflex","Ocircumflexacute",
	"Ocircumflexdotbelow","Ocircumflexgrave","Ocircumflexhookabove",
	"Ocircumflexsmall","Ocircumflextilde","Ocyrillic","Odblacute","Odblgrave",
	"Odieresis","Odieresiscyrillic","Odieresissmall","Odotbelow","Ogoneksmall",
	"Ograve","Ogravesmall","Oharmenian","Ohm","Ohookabove","Ohorn","Ohornacute",
	"Ohorndotbelow","Ohorngrave","Ohornhookabove","Ohorntilde","Ohungarumlaut",
	"Oi","Oinvertedbreve","Omacron","Omacronacute","Omacrongrave","Omega",
	"Omegacyrillic","Omegagreek","Omegaroundcyrillic","Omegatitlocyrillic",
	"Omegatonos","Omicron","Omicrontonos","Omonospace","Oneroman","Oogonek",
	"Oogonekmacron","Oopen","Oslash","Oslashacute","Oslashsmall","Osmall",
	"Ostrokeacute","Otcyrillic","Otilde","Otildeacute","Otildedieresis",
	"Otildesmall","P","Pacute","Pcircle","Pdotaccent","Pecyrillic","Peharmenian",
	"Pemiddlehookcyrillic","Phi","Phook","Pi","Piwrarmenian","Pmonospace","Psi",
	"Psicyrillic","Psmall","Q","Qcircle","Qmonospace","Qsmall","R","Raarmenian",
	"Racute","Rcaron","Rcedilla","Rcircle","Rcommaaccent","Rdblgrave",
	"Rdotaccent","Rdotbelow","Rdotbelowmacron","Reharmenian","Rfraktur","Rho",
	"Ringsmall","Rinvertedbreve","Rlinebelow","Rmonospace","Rsmall",
	"Rsmallinverted","Rsmallinvertedsuperior","S","SF010000","SF020000",
	"SF030000","SF040000","SF050000","SF060000","SF070000","SF080000","SF090000",
	"SF100000","SF110000","SF190000","SF200000","SF210000","SF220000","SF230000",
	"SF240000","SF250000","SF260000","SF270000","SF280000","SF360000","SF370000",
	"SF380000","SF390000","SF400000","SF410000","SF420000","SF430000","SF440000",
	"SF450000","SF460000","SF470000","SF480000","SF490000","SF500000","SF510000",
	"SF520000","SF530000","SF540000","Sacute","Sacutedotaccent","Sampigreek",
	"Scaron","Scarondotaccent","Scaronsmall","Scedilla","Schwa","Schwacyrillic",
	"Schwadieresiscyrillic","Scircle","Scircumflex","Scommaaccent","Sdotaccent",
	"Sdotbelow","Sdotbelowdotaccent","Seharmenian","Sevenroman","Shaarmenian",
	"Shacyrillic","Shchacyrillic","Sheicoptic","Shhacyrillic","Shimacoptic",
	"Sigma","Sixroman","Smonospace","Softsigncyrillic","Ssmall","Stigmagreek","T",
	"Tau","Tbar","Tcaron","Tcedilla","Tcircle","Tcircumflexbelow","Tcommaaccent",
	"Tdotaccent","Tdotbelow","Tecyrillic","Tedescendercyrillic","Tenroman",
	"Tetsecyrillic","Theta","Thook","Thorn","Thornsmall","Threeroman",
	"Tildesmall","Tiwnarmenian","Tlinebelow","Tmonospace","Toarmenian","Tonefive",
	"Tonesix","Tonetwo","Tretroflexhook","Tsecyrillic","Tshecyrillic","Tsmall",
	"Twelveroman","Tworoman","U","Uacute","Uacutesmall","Ubreve","Ucaron",
	"Ucircle","Ucircumflex","Ucircumflexbelow","Ucircumflexsmall","Ucyrillic",
	"Udblacute","Udblgrave","Udieresis","Udieresisacute","Udieresisbelow",
	"Udieresiscaron","Udieresiscyrillic","Udieresisgrave","Udieresismacron",
	"Udieresissmall","Udotbelow","Ugrave","Ugravesmall","Uhookabove","Uhorn",
	"Uhornacute","Uhorndotbelow","Uhorngrave","Uhornhookabove","Uhorntilde",
	"Uhungarumlaut","Uhungarumlautcyrillic","Uinvertedbreve","Ukcyrillic",
	"Umacron","Umacroncyrillic","Umacrondieresis","Umonospace","Uogonek",
	"Upsilon","Upsilon1","Upsilonacutehooksymbolgreek","Upsilonafrican",
	"Upsilondieresis","Upsilondieresishooksymbolgreek","Upsilonhooksymbol",
	"Upsilontonos","Uring","Ushortcyrillic","Usmall","Ustraightcyrillic",
	"Ustraightstrokecyrillic","Utilde","Utildeacute","Utildebelow","V","Vcircle",
	"Vdotbelow","Vecyrillic","Vewarmenian","Vhook","Vmonospace","Voarmenian",
	"Vsmall","Vtilde","W","Wacute","Wcircle","Wcircumflex","Wdieresis",
	"Wdotaccent","Wdotbelow","Wgrave","Wmonospace","Wsmall","X","Xcircle",
	"Xdieresis","Xdotaccent","Xeharmenian","Xi","Xmonospace","Xsmall","Y",
	"Yacute","Yacutesmall","Yatcyrillic","Ycircle","Ycircumflex","Ydieresis",
	"Ydieresissmall","Ydotaccent","Ydotbelow","Yericyrillic",
	"Yerudieresiscyrillic","Ygrave","Yhook","Yhookabove","Yiarmenian",
	"Yicyrillic","Yiwnarmenian","Ymonospace","Ysmall","Ytilde","Yusbigcyrillic",
	"Yusbigiotifiedcyrillic","Yuslittlecyrillic","Yuslittleiotifiedcyrillic","Z",
	"Zaarmenian","Zacute","Zcaron","Zcaronsmall","Zcircle","Zcircumflex","Zdot",
	"Zdotaccent","Zdotbelow","Zecyrillic","Zedescendercyrillic",
	"Zedieresiscyrillic","Zeta","Zhearmenian","Zhebrevecyrillic","Zhecyrillic",
	"Zhedescendercyrillic","Zhedieresiscyrillic","Zlinebelow","Zmonospace",
	"Zsmall","Zstroke","a","aabengali","aacute","aadeva","aagujarati",
	"aagurmukhi","aamatragurmukhi","aarusquare","aavowelsignbengali",
	"aavowelsigndeva","aavowelsigngujarati","abbreviationmarkarmenian",
	"abbreviationsigndeva","abengali","abopomofo","abreve","abreveacute",
	"abrevecyrillic","abrevedotbelow","abrevegrave","abrevehookabove",
	"abrevetilde","acaron","acircle","acircumflex","acircumflexacute",
	"acircumflexdotbelow","acircumflexgrave","acircumflexhookabove",
	"acircumflextilde","acute","acutebelowcmb","acutecmb","acutecomb","acutedeva",
	"acutelowmod","acutetonecmb","acyrillic","adblgrave","addakgurmukhi","adeva",
	"adieresis","adieresiscyrillic","adieresismacron","adotbelow","adotmacron",
	"ae","aeacute","aekorean","aemacron","afii00208","afii08941","afii10017",
	"afii10018","afii10019","afii10020","afii10021","afii10022","afii10023",
	"afii10024","afii10025","afii10026","afii10027","afii10028","afii10029",
	"afii10030","afii10031","afii10032","afii10033","afii10034","afii10035",
	"afii10036","afii10037","afii10038","afii10039","afii10040","afii10041",
	"afii10042","afii10043","afii10044","afii10045","afii10046","afii10047",
	"afii10048","afii10049","afii10050","afii10051","afii10052","afii10053",
	"afii10054","afii10055","afii10056","afii10057","afii10058","afii10059",
	"afii10060","afii10061","afii10062","afii10063","afii10064","afii10065",
	"afii10066","afii10067","afii10068","afii10069","afii10070","afii10071",
	"afii10072","afii10073","afii10074","afii10075","afii10076","afii10077",
	"afii10078","afii10079","afii10080","afii10081","afii10082","afii10083",
	"afii10084","afii10085","afii10086","afii10087","afii10088","afii10089",
	"afii10090","afii10091","afii10092","afii10093","afii10094","afii10095",
	"afii10096","afii10097","afii10098","afii10099","afii10100","afii10101",
	"afii10102","afii10103","afii10104","afii10105","afii10106","afii10107",
	"afii10108","afii10109","afii10110","afii10145","afii10146","afii10147",
	"afii10148","afii10192","afii10193","afii10194","afii10195","afii10196",
	"afii10831","afii10832","afii10846","afii299","afii300","afii301","afii57381",
	"afii57388","afii57392","afii57393","afii57394","afii57395","afii57396",
	"afii57397","afii57398","afii57399","afii57400","afii57401","afii57403",
	"afii57407","afii57409","afii57410","afii57411","afii57412","afii57413",
	"afii57414","afii57415","afii57416","afii57417","afii57418","afii57419",
	"afii57420","afii57421","afii57422","afii57423","afii57424","afii57425",
	"afii57426","afii57427","afii57428","afii57429","afii57430","afii57431",
	"afii57432","afii57433","afii57434","afii57440","afii57441","afii57442",
	"afii57443","afii57444","afii57445","afii57446","afii57448","afii57449",
	"afii57450","afii57451","afii57452","afii57453","afii57454","afii57455",
	"afii57456","afii57457","afii57458","afii57470","afii57505","afii57506",
	"afii57507","afii57508","afii57509","afii57511","afii57512","afii57513",
	"afii57514","afii57519","afii57534","afii57636","afii57645","afii57658",
	"afii57664","afii57665","afii57666","afii57667","afii57668","afii57669",
	"afii57670","afii57671","afii57672","afii57673","afii57674","afii57675",
	"afii57676","afii57677","afii57678","afii57679","afii57680","afii57681",
	"afii57682","afii57683","afii57684","afii57685","afii57686","afii57687",
	"afii57688","afii57689","afii57690","afii57694","afii57695","afii57700",
	"afii57705","afii57716","afii57717","afii57718","afii57723","afii57793",
	"afii57794","afii57795","afii57796","afii57797","afii57798","afii57799",
	"afii57800","afii57801","afii57802","afii57803","afii57804","afii57806",
	"afii57807","afii57839","afii57841","afii57842","afii57929","afii61248",
	"afii61289","afii61352","afii61573","afii61574","afii61575","afii61664",
	"afii63167","afii64937","agrave","agujarati","agurmukhi","ahiragana",
	"ahookabove","aibengali","aibopomofo","aideva","aiecyrillic","aigujarati",
	"aigurmukhi","aimatragurmukhi","ainarabic","ainfinalarabic",
	"aininitialarabic","ainmedialarabic","ainvertedbreve","aivowelsignbengali",
	"aivowelsigndeva","aivowelsigngujarati","akatakana","akatakanahalfwidth",
	"akorean","alef","alefarabic","alefdageshhebrew","aleffinalarabic",
	"alefhamzaabovearabic","alefhamzaabovefinalarabic","alefhamzabelowarabic",
	"alefhamzabelowfinalarabic","alefhebrew","aleflamedhebrew",
	"alefmaddaabovearabic","alefmaddaabovefinalarabic","alefmaksuraarabic",
	"alefmaksurafinalarabic","alefmaksurainitialarabic","alefmaksuramedialarabic",
	"alefpatahhebrew","alefqamatshebrew","aleph","allequal","alpha","alphatonos",
	"amacron","amonospace","ampersand","ampersandmonospace","ampersandsmall",
	"amsquare","anbopomofo","angbopomofo","angkhankhuthai","angle",
	"anglebracketleft","anglebracketleftvertical","anglebracketright",
	"anglebracketrightvertical","angleleft","angleright","angstrom","anoteleia",
	"anudattadeva","anusvarabengali","anusvaradeva","anusvaragujarati","aogonek",
	"apaatosquare","aparen","apostrophearmenian","apostrophemod","apple",
	"approaches","approxequal","approxequalorimage","approximatelyequal",
	"araeaekorean","araeakorean","arc","arighthalfring","aring","aringacute",
	"aringbelow","arrowboth","arrowdashdown","arrowdashleft","arrowdashright",
	"arrowdashup","arrowdblboth","arrowdbldown","arrowdblleft","arrowdblright",
	"arrowdblup","arrowdown","arrowdownleft","arrowdownright","arrowdownwhite",
	"arrowheaddownmod","arrowheadleftmod","arrowheadrightmod","arrowheadupmod",
	"arrowhorizex","arrowleft","arrowleftdbl","arrowleftdblstroke",
	"arrowleftoverright","arrowleftwhite","arrowright","arrowrightdblstroke",
	"arrowrightheavy","arrowrightoverleft","arrowrightwhite","arrowtableft",
	"arrowtabright","arrowup","arrowupdn","arrowupdnbse","arrowupdownbase",
	"arrowupleft","arrowupleftofdown","arrowupright","arrowupwhite","arrowvertex",
	"asciicircum","asciicircummonospace","asciitilde","asciitildemonospace",
	"ascript","ascriptturned","asmallhiragana","asmallkatakana",
	"asmallkatakanahalfwidth","asterisk","asteriskaltonearabic","asteriskarabic",
	"asteriskmath","asteriskmonospace","asterisksmall","asterism","asuperior",
	"asymptoticallyequal","at","atilde","atmonospace","atsmall","aturned",
	"aubengali","aubopomofo","audeva","augujarati","augurmukhi",
	"aulengthmarkbengali","aumatragurmukhi","auvowelsignbengali",
	"auvowelsigndeva","auvowelsigngujarati","avagrahadeva","aybarmenian","ayin",
	"ayinaltonehebrew","ayinhebrew","b","babengali","backslash",
	"backslashmonospace","badeva","bagujarati","bagurmukhi","bahiragana",
	"bahtthai","bakatakana","bar","barmonospace","bbopomofo","bcircle",
	"bdotaccent","bdotbelow","beamedsixteenthnotes","because","becyrillic",
	"beharabic","behfinalarabic","behinitialarabic","behiragana",
	"behmedialarabic","behmeeminitialarabic","behmeemisolatedarabic",
	"behnoonfinalarabic","bekatakana","benarmenian","bet","beta",
	"betasymbolgreek","betdagesh","betdageshhebrew","bethebrew","betrafehebrew",
	"bhabengali","bhadeva","bhagujarati","bhagurmukhi","bhook","bihiragana",
	"bikatakana","bilabialclick","bindigurmukhi","birusquare","blackcircle",
	"blackdiamond","blackdownpointingtriangle","blackleftpointingpointer",
	"blackleftpointingtriangle","blacklenticularbracketleft",
	"blacklenticularbracketleftvertical","blacklenticularbracketright",
	"blacklenticularbracketrightvertical","blacklowerlefttriangle",
	"blacklowerrighttriangle","blackrectangle","blackrightpointingpointer",
	"blackrightpointingtriangle","blacksmallsquare","blacksmilingface",
	"blacksquare","blackstar","blackupperlefttriangle","blackupperrighttriangle",
	"blackuppointingsmalltriangle","blackuppointingtriangle","blank","blinebelow",
	"block","bmonospace","bobaimaithai","bohiragana","bokatakana","bparen",
	"bqsquare","braceex","braceleft","braceleftbt","braceleftmid",
	"braceleftmonospace","braceleftsmall","bracelefttp","braceleftvertical",
	"braceright","bracerightbt","bracerightmid","bracerightmonospace",
	"bracerightsmall","bracerighttp","bracerightvertical","bracketleft",
	"bracketleftbt","bracketleftex","bracketleftmonospace","bracketlefttp",
	"bracketright","bracketrightbt","bracketrightex","bracketrightmonospace",
	"bracketrighttp","breve","brevebelowcmb","brevecmb","breveinvertedbelowcmb",
	"breveinvertedcmb","breveinverteddoublecmb","bridgebelowcmb",
	"bridgeinvertedbelowcmb","brokenbar","bstroke","bsuperior","btopbar",
	"buhiragana","bukatakana","bullet","bulletinverse","bulletoperator",
	"bullseye","c","caarmenian","cabengali","cacute","cadeva","cagujarati",
	"cagurmukhi","calsquare","candrabindubengali","candrabinducmb",
	"candrabindudeva","candrabindugujarati","capslock","careof","caron",
	"caronbelowcmb","caroncmb","carriagereturn","cbopomofo","ccaron","ccedilla",
	"ccedillaacute","ccircle","ccircumflex","ccurl","cdot","cdotaccent",
	"cdsquare","cedilla","cedillacmb","cent","centigrade","centinferior",
	"centmonospace","centoldstyle","centsuperior","chaarmenian","chabengali",
	"chadeva","chagujarati","chagurmukhi","chbopomofo","cheabkhasiancyrillic",
	"checkmark","checyrillic","chedescenderabkhasiancyrillic",
	"chedescendercyrillic","chedieresiscyrillic","cheharmenian",
	"chekhakassiancyrillic","cheverticalstrokecyrillic","chi",
	"chieuchacirclekorean","chieuchaparenkorean","chieuchcirclekorean",
	"chieuchkorean","chieuchparenkorean","chochangthai","chochanthai",
	"chochingthai","chochoethai","chook","cieucacirclekorean","cieucaparenkorean",
	"cieuccirclekorean","cieuckorean","cieucparenkorean","cieucuparenkorean",
	"circle","circlemultiply","circleot","circleplus","circlepostalmark",
	"circlewithlefthalfblack","circlewithrighthalfblack","circumflex",
	"circumflexbelowcmb","circumflexcmb","clear","clickalveolar","clickdental",
	"clicklateral","clickretroflex","club","clubsuitblack","clubsuitwhite",
	"cmcubedsquare","cmonospace","cmsquaredsquare","coarmenian","colon",
	"colonmonetary","colonmonospace","colonsign","colonsmall",
	"colontriangularhalfmod","colontriangularmod","comma","commaabovecmb",
	"commaaboverightcmb","commaaccent","commaarabic","commaarmenian",
	"commainferior","commamonospace","commareversedabovecmb","commareversedmod",
	"commasmall","commasuperior","commaturnedabovecmb","commaturnedmod","compass",
	"congruent","contourintegral","control","controlACK","controlBEL","controlBS",
	"controlCAN","controlCR","controlDC1","controlDC2","controlDC3","controlDC4",
	"controlDEL","controlDLE","controlEM","controlENQ","controlEOT","controlESC",
	"controlETB","controlETX","controlFF","controlFS","controlGS","controlHT",
	"controlLF","controlNAK","controlRS","controlSI","controlSO","controlSOT",
	"controlSTX","controlSUB","controlSYN","controlUS","controlVT","copyright",
	"copyrightsans","copyrightserif","cornerbracketleft",
	"cornerbracketlefthalfwidth","cornerbracketleftvertical","cornerbracketright",
	"cornerbracketrighthalfwidth","cornerbracketrightvertical",
	"corporationsquare","cosquare","coverkgsquare","cparen","cruzeiro",
	"cstretched","curlyand","curlyor","currency","cyrBreve","cyrFlex","cyrbreve",
	"cyrflex","d","daarmenian","dabengali","dadarabic","dadeva","dadfinalarabic",
	"dadinitialarabic","dadmedialarabic","dagesh","dageshhebrew","dagger",
	"daggerdbl","dagujarati","dagurmukhi","dahiragana","dakatakana","dalarabic",
	"dalet","daletdagesh","daletdageshhebrew","dalethatafpatah",
	"dalethatafpatahhebrew","dalethatafsegol","dalethatafsegolhebrew",
	"dalethebrew","dalethiriq","dalethiriqhebrew","daletholam","daletholamhebrew",
	"daletpatah","daletpatahhebrew","daletqamats","daletqamatshebrew",
	"daletqubuts","daletqubutshebrew","daletsegol","daletsegolhebrew",
	"daletsheva","daletshevahebrew","dalettsere","dalettserehebrew",
	"dalfinalarabic","dammaarabic","dammalowarabic","dammatanaltonearabic",
	"dammatanarabic","danda","dargahebrew","dargalefthebrew",
	"dasiapneumatacyrilliccmb","dblGrave","dblanglebracketleft",
	"dblanglebracketleftvertical","dblanglebracketright",
	"dblanglebracketrightvertical","dblarchinvertedbelowcmb","dblarrowleft",
	"dblarrowright","dbldanda","dblgrave","dblgravecmb","dblintegral",
	"dbllowline","dbllowlinecmb","dbloverlinecmb","dblprimemod","dblverticalbar",
	"dblverticallineabovecmb","dbopomofo","dbsquare","dcaron","dcedilla",
	"dcircle","dcircumflexbelow","dcroat","ddabengali","ddadeva","ddagujarati",
	"ddagurmukhi","ddalarabic","ddalfinalarabic","dddhadeva","ddhabengali",
	"ddhadeva","ddhagujarati","ddhagurmukhi","ddotaccent","ddotbelow",
	"decimalseparatorarabic","decimalseparatorpersian","decyrillic","degree",
	"dehihebrew","dehiragana","deicoptic","dekatakana","deleteleft","deleteright",
	"delta","deltaturned","denominatorminusonenumeratorbengali","dezh",
	"dhabengali","dhadeva","dhagujarati","dhagurmukhi","dhook","dialytikatonos",
	"dialytikatonoscmb","diamond","diamondsuitwhite","dieresis","dieresisacute",
	"dieresisbelowcmb","dieresiscmb","dieresisgrave","dieresistonos","dihiragana",
	"dikatakana","dittomark","divide","divides","divisionslash","djecyrillic",
	"dkshade","dlinebelow","dlsquare","dmacron","dmonospace","dnblock",
	"dochadathai","dodekthai","dohiragana","dokatakana","dollar","dollarinferior",
	"dollarmonospace","dollaroldstyle","dollarsmall","dollarsuperior","dong",
	"dorusquare","dotaccent","dotaccentcmb","dotbelowcmb","dotbelowcomb",
	"dotkatakana","dotlessi","dotlessj","dotlessjstrokehook","dotmath",
	"dottedcircle","doubleyodpatah","doubleyodpatahhebrew","downtackbelowcmb",
	"downtackmod","dparen","dsuperior","dtail","dtopbar","duhiragana",
	"dukatakana","dz","dzaltone","dzcaron","dzcurl","dzeabkhasiancyrillic",
	"dzecyrillic","dzhecyrillic","e","eacute","earth","ebengali","ebopomofo",
	"ebreve","ecandradeva","ecandragujarati","ecandravowelsigndeva",
	"ecandravowelsigngujarati","ecaron","ecedillabreve","echarmenian",
	"echyiwnarmenian","ecircle","ecircumflex","ecircumflexacute",
	"ecircumflexbelow","ecircumflexdotbelow","ecircumflexgrave",
	"ecircumflexhookabove","ecircumflextilde","ecyrillic","edblgrave","edeva",
	"edieresis","edot","edotaccent","edotbelow","eegurmukhi","eematragurmukhi",
	"efcyrillic","egrave","egujarati","eharmenian","ehbopomofo","ehiragana",
	"ehookabove","eibopomofo","eight","eightarabic","eightbengali","eightcircle",
	"eightcircleinversesansserif","eightdeva","eighteencircle","eighteenparen",
	"eighteenperiod","eightgujarati","eightgurmukhi","eighthackarabic",
	"eighthangzhou","eighthnotebeamed","eightideographicparen","eightinferior",
	"eightmonospace","eightoldstyle","eightparen","eightperiod","eightpersian",
	"eightroman","eightsuperior","eightthai","einvertedbreve","eiotifiedcyrillic",
	"ekatakana","ekatakanahalfwidth","ekonkargurmukhi","ekorean","elcyrillic",
	"element","elevencircle","elevenparen","elevenperiod","elevenroman",
	"ellipsis","ellipsisvertical","emacron","emacronacute","emacrongrave",
	"emcyrillic","emdash","emdashvertical","emonospace","emphasismarkarmenian",
	"emptyset","enbopomofo","encyrillic","endash","endashvertical",
	"endescendercyrillic","eng","engbopomofo","enghecyrillic","enhookcyrillic",
	"enspace","eogonek","eokorean","eopen","eopenclosed","eopenreversed",
	"eopenreversedclosed","eopenreversedhook","eparen","epsilon","epsilontonos",
	"equal","equalmonospace","equalsmall","equalsuperior","equivalence",
	"erbopomofo","ercyrillic","ereversed","ereversedcyrillic","escyrillic",
	"esdescendercyrillic","esh","eshcurl","eshortdeva","eshortvowelsigndeva",
	"eshreversedloop","eshsquatreversed","esmallhiragana","esmallkatakana",
	"esmallkatakanahalfwidth","estimated","esuperior","eta","etarmenian",
	"etatonos","eth","etilde","etildebelow","etnahtafoukhhebrew",
	"etnahtafoukhlefthebrew","etnahtahebrew","etnahtalefthebrew","eturned",
	"eukorean","euro","evowelsignbengali","evowelsigndeva","evowelsigngujarati",
	"exclam","exclamarmenian","exclamdbl","exclamdown","exclamdownsmall",
	"exclammonospace","exclamsmall","existential","ezh","ezhcaron","ezhcurl",
	"ezhreversed","ezhtail","f","fadeva","fagurmukhi","fahrenheit","fathaarabic",
	"fathalowarabic","fathatanarabic","fbopomofo","fcircle","fdotaccent",
	"feharabic","feharmenian","fehfinalarabic","fehinitialarabic",
	"fehmedialarabic","feicoptic","female","ff","ffi","ffl","fi","fifteencircle",
	"fifteenparen","fifteenperiod","figuredash","filledbox","filledrect",
	"finalkaf","finalkafdagesh","finalkafdageshhebrew","finalkafhebrew",
	"finalkafqamats","finalkafqamatshebrew","finalkafsheva","finalkafshevahebrew",
	"finalmem","finalmemhebrew","finalnun","finalnunhebrew","finalpe",
	"finalpehebrew","finaltsadi","finaltsadihebrew","firsttonechinese","fisheye",
	"fitacyrillic","five","fivearabic","fivebengali","fivecircle",
	"fivecircleinversesansserif","fivedeva","fiveeighths","fivegujarati",
	"fivegurmukhi","fivehackarabic","fivehangzhou","fiveideographicparen",
	"fiveinferior","fivemonospace","fiveoldstyle","fiveparen","fiveperiod",
	"fivepersian","fiveroman","fivesuperior","fivethai","fl","florin",
	"fmonospace","fmsquare","fofanthai","fofathai","fongmanthai","forall","four",
	"fourarabic","fourbengali","fourcircle","fourcircleinversesansserif",
	"fourdeva","fourgujarati","fourgurmukhi","fourhackarabic","fourhangzhou",
	"fourideographicparen","fourinferior","fourmonospace","fournumeratorbengali",
	"fouroldstyle","fourparen","fourperiod","fourpersian","fourroman",
	"foursuperior","fourteencircle","fourteenparen","fourteenperiod","fourthai",
	"fourthtonechinese","fparen","fraction","franc","g","gabengali","gacute",
	"gadeva","gafarabic","gaffinalarabic","gafinitialarabic","gafmedialarabic",
	"gagujarati","gagurmukhi","gahiragana","gakatakana","gamma","gammalatinsmall",
	"gammasuperior","gangiacoptic","gbopomofo","gbreve","gcaron","gcedilla",
	"gcircle","gcircumflex","gcommaaccent","gdot","gdotaccent","gecyrillic",
	"gehiragana","gekatakana","geometricallyequal","gereshaccenthebrew",
	"gereshhebrew","gereshmuqdamhebrew","germandbls","gershayimaccenthebrew",
	"gershayimhebrew","getamark","ghabengali","ghadarmenian","ghadeva",
	"ghagujarati","ghagurmukhi","ghainarabic","ghainfinalarabic",
	"ghaininitialarabic","ghainmedialarabic","ghemiddlehookcyrillic",
	"ghestrokecyrillic","gheupturncyrillic","ghhadeva","ghhagurmukhi","ghook",
	"ghzsquare","gihiragana","gikatakana","gimarmenian","gimel","gimeldagesh",
	"gimeldageshhebrew","gimelhebrew","gjecyrillic","glottalinvertedstroke",
	"glottalstop","glottalstopinverted","glottalstopmod","glottalstopreversed",
	"glottalstopreversedmod","glottalstopreversedsuperior","glottalstopstroke",
	"glottalstopstrokereversed","gmacron","gmonospace","gohiragana","gokatakana",
	"gparen","gpasquare","gradient","grave","gravebelowcmb","gravecmb",
	"gravecomb","gravedeva","gravelowmod","gravemonospace","gravetonecmb",
	"greater","greaterequal","greaterequalorless","greatermonospace",
	"greaterorequivalent","greaterorless","greateroverequal","greatersmall",
	"gscript","gstroke","guhiragana","guillemotleft","guillemotright",
	"guilsinglleft","guilsinglright","gukatakana","guramusquare","gysquare","h",
	"haabkhasiancyrillic","haaltonearabic","habengali","hadescendercyrillic",
	"hadeva","hagujarati","hagurmukhi","haharabic","hahfinalarabic",
	"hahinitialarabic","hahiragana","hahmedialarabic","haitusquare","hakatakana",
	"hakatakanahalfwidth","halantgurmukhi","hamzaarabic","hamzadammaarabic",
	"hamzadammatanarabic","hamzafathaarabic","hamzafathatanarabic",
	"hamzalowarabic","hamzalowkasraarabic","hamzalowkasratanarabic",
	"hamzasukunarabic","hangulfiller","hardsigncyrillic","harpoonleftbarbup",
	"harpoonrightbarbup","hasquare","hatafpatah","hatafpatah16","hatafpatah23",
	"hatafpatah2f","hatafpatahhebrew","hatafpatahnarrowhebrew",
	"hatafpatahquarterhebrew","hatafpatahwidehebrew","hatafqamats",
	"hatafqamats1b","hatafqamats28","hatafqamats34","hatafqamatshebrew",
	"hatafqamatsnarrowhebrew","hatafqamatsquarterhebrew","hatafqamatswidehebrew",
	"hatafsegol","hatafsegol17","hatafsegol24","hatafsegol30","hatafsegolhebrew",
	"hatafsegolnarrowhebrew","hatafsegolquarterhebrew","hatafsegolwidehebrew",
	"hbar","hbopomofo","hbrevebelow","hcedilla","hcircle","hcircumflex",
	"hdieresis","hdotaccent","hdotbelow","he","heart","heartsuitblack",
	"heartsuitwhite","hedagesh","hedageshhebrew","hehaltonearabic","heharabic",
	"hehebrew","hehfinalaltonearabic","hehfinalalttwoarabic","hehfinalarabic",
	"hehhamzaabovefinalarabic","hehhamzaaboveisolatedarabic",
	"hehinitialaltonearabic","hehinitialarabic","hehiragana",
	"hehmedialaltonearabic","hehmedialarabic","heiseierasquare","hekatakana",
	"hekatakanahalfwidth","hekutaarusquare","henghook","herutusquare","het",
	"hethebrew","hhook","hhooksuperior","hieuhacirclekorean","hieuhaparenkorean",
	"hieuhcirclekorean","hieuhkorean","hieuhparenkorean","hihiragana",
	"hikatakana","hikatakanahalfwidth","hiriq","hiriq14","hiriq21","hiriq2d",
	"hiriqhebrew","hiriqnarrowhebrew","hiriqquarterhebrew","hiriqwidehebrew",
	"hlinebelow","hmonospace","hoarmenian","hohipthai","hohiragana","hokatakana",
	"hokatakanahalfwidth","holam","holam19","holam26","holam32","holamhebrew",
	"holamnarrowhebrew","holamquarterhebrew","holamwidehebrew","honokhukthai",
	"hookabovecomb","hookcmb","hookpalatalizedbelowcmb","hookretroflexbelowcmb",
	"hoonsquare","horicoptic","horizontalbar","horncmb","hotsprings","house",
	"hparen","hsuperior","hturned","huhiragana","huiitosquare","hukatakana",
	"hukatakanahalfwidth","hungarumlaut","hungarumlautcmb","hv","hyphen",
	"hypheninferior","hyphenmonospace","hyphensmall","hyphensuperior","hyphentwo",
	"i","iacute","iacyrillic","ibengali","ibopomofo","ibreve","icaron","icircle",
	"icircumflex","icyrillic","idblgrave","ideographearthcircle",
	"ideographfirecircle","ideographicallianceparen","ideographiccallparen",
	"ideographiccentrecircle","ideographicclose","ideographiccomma",
	"ideographiccommaleft","ideographiccongratulationparen",
	"ideographiccorrectcircle","ideographicearthparen",
	"ideographicenterpriseparen","ideographicexcellentcircle",
	"ideographicfestivalparen","ideographicfinancialcircle",
	"ideographicfinancialparen","ideographicfireparen","ideographichaveparen",
	"ideographichighcircle","ideographiciterationmark","ideographiclaborcircle",
	"ideographiclaborparen","ideographicleftcircle","ideographiclowcircle",
	"ideographicmedicinecircle","ideographicmetalparen","ideographicmoonparen",
	"ideographicnameparen","ideographicperiod","ideographicprintcircle",
	"ideographicreachparen","ideographicrepresentparen",
	"ideographicresourceparen","ideographicrightcircle","ideographicsecretcircle",
	"ideographicselfparen","ideographicsocietyparen","ideographicspace",
	"ideographicspecialparen","ideographicstockparen","ideographicstudyparen",
	"ideographicsunparen","ideographicsuperviseparen","ideographicwaterparen",
	"ideographicwoodparen","ideographiczero","ideographmetalcircle",
	"ideographmooncircle","ideographnamecircle","ideographsuncircle",
	"ideographwatercircle","ideographwoodcircle","ideva","idieresis",
	"idieresisacute","idieresiscyrillic","idotbelow","iebrevecyrillic",
	"iecyrillic","ieungacirclekorean","ieungaparenkorean","ieungcirclekorean",
	"ieungkorean","ieungparenkorean","igrave","igujarati","igurmukhi","ihiragana",
	"ihookabove","iibengali","iicyrillic","iideva","iigujarati","iigurmukhi",
	"iimatragurmukhi","iinvertedbreve","iishortcyrillic","iivowelsignbengali",
	"iivowelsigndeva","iivowelsigngujarati","ij","ikatakana","ikatakanahalfwidth",
	"ikorean","ilde","iluyhebrew","imacron","imacroncyrillic",
	"imageorapproximatelyequal","imatragurmukhi","imonospace","increment",
	"infinity","iniarmenian","integral","integralbottom","integralbt",
	"integralex","integraltop","integraltp","intersection","intisquare",
	"invbullet","invcircle","invsmileface","iocyrillic","iogonek","iota",
	"iotadieresis","iotadieresistonos","iotalatin","iotatonos","iparen",
	"irigurmukhi","ismallhiragana","ismallkatakana","ismallkatakanahalfwidth",
	"issharbengali","istroke","isuperior","iterationhiragana","iterationkatakana",
	"itilde","itildebelow","iubopomofo","iucyrillic","ivowelsignbengali",
	"ivowelsigndeva","ivowelsigngujarati","izhitsacyrillic",
	"izhitsadblgravecyrillic","j","jaarmenian","jabengali","jadeva","jagujarati",
	"jagurmukhi","jbopomofo","jcaron","jcircle","jcircumflex","jcrossedtail",
	"jdotlessstroke","jecyrillic","jeemarabic","jeemfinalarabic",
	"jeeminitialarabic","jeemmedialarabic","jeharabic","jehfinalarabic",
	"jhabengali","jhadeva","jhagujarati","jhagurmukhi","jheharmenian","jis",
	"jmonospace","jparen","jsuperior","k","kabashkircyrillic","kabengali",
	"kacute","kacyrillic","kadescendercyrillic","kadeva","kaf","kafarabic",
	"kafdagesh","kafdageshhebrew","kaffinalarabic","kafhebrew","kafinitialarabic",
	"kafmedialarabic","kafrafehebrew","kagujarati","kagurmukhi","kahiragana",
	"kahookcyrillic","kakatakana","kakatakanahalfwidth","kappa",
	"kappasymbolgreek","kapyeounmieumkorean","kapyeounphieuphkorean",
	"kapyeounpieupkorean","kapyeounssangpieupkorean","karoriisquare",
	"kashidaautoarabic","kashidaautonosidebearingarabic","kasmallkatakana",
	"kasquare","kasraarabic","kasratanarabic","kastrokecyrillic",
	"katahiraprolongmarkhalfwidth","kaverticalstrokecyrillic","kbopomofo",
	"kcalsquare","kcaron","kcedilla","kcircle","kcommaaccent","kdotbelow",
	"keharmenian","kehiragana","kekatakana","kekatakanahalfwidth","kenarmenian",
	"kesmallkatakana","kgreenlandic","khabengali","khacyrillic","khadeva",
	"khagujarati","khagurmukhi","khaharabic","khahfinalarabic",
	"khahinitialarabic","khahmedialarabic","kheicoptic","khhadeva","khhagurmukhi",
	"khieukhacirclekorean","khieukhaparenkorean","khieukhcirclekorean",
	"khieukhkorean","khieukhparenkorean","khokhaithai","khokhonthai",
	"khokhuatthai","khokhwaithai","khomutthai","khook","khorakhangthai",
	"khzsquare","kihiragana","kikatakana","kikatakanahalfwidth",
	"kiroguramusquare","kiromeetorusquare","kirosquare","kiyeokacirclekorean",
	"kiyeokaparenkorean","kiyeokcirclekorean","kiyeokkorean","kiyeokparenkorean",
	"kiyeoksioskorean","kjecyrillic","klinebelow","klsquare","kmcubedsquare",
	"kmonospace","kmsquaredsquare","kohiragana","kohmsquare","kokaithai",
	"kokatakana","kokatakanahalfwidth","kooposquare","koppacyrillic",
	"koreanstandardsymbol","koroniscmb","kparen","kpasquare","ksicyrillic",
	"ktsquare","kturned","kuhiragana","kukatakana","kukatakanahalfwidth",
	"kvsquare","kwsquare","l","labengali","lacute","ladeva","lagujarati",
	"lagurmukhi","lakkhangyaothai","lamaleffinalarabic",
	"lamalefhamzaabovefinalarabic","lamalefhamzaaboveisolatedarabic",
	"lamalefhamzabelowfinalarabic","lamalefhamzabelowisolatedarabic",
	"lamalefisolatedarabic","lamalefmaddaabovefinalarabic",
	"lamalefmaddaaboveisolatedarabic","lamarabic","lambda","lambdastroke","lamed",
	"lameddagesh","lameddageshhebrew","lamedhebrew","lamedholam",
	"lamedholamdagesh","lamedholamdageshhebrew","lamedholamhebrew",
	"lamfinalarabic","lamhahinitialarabic","laminitialarabic",
	"lamjeeminitialarabic","lamkhahinitialarabic","lamlamhehisolatedarabic",
	"lammedialarabic","lammeemhahinitialarabic","lammeeminitialarabic",
	"lammeemjeeminitialarabic","lammeemkhahinitialarabic","largecircle","lbar",
	"lbelt","lbopomofo","lcaron","lcedilla","lcircle","lcircumflexbelow",
	"lcommaaccent","ldot","ldotaccent","ldotbelow","ldotbelowmacron",
	"leftangleabovecmb","lefttackbelowcmb","less","lessequal",
	"lessequalorgreater","lessmonospace","lessorequivalent","lessorgreater",
	"lessoverequal","lesssmall","lezh","lfblock","lhookretroflex","lira",
	"liwnarmenian","lj","ljecyrillic","ll","lladeva","llagujarati","llinebelow",
	"llladeva","llvocalicbengali","llvocalicdeva","llvocalicvowelsignbengali",
	"llvocalicvowelsigndeva","lmiddletilde","lmonospace","lmsquare","lochulathai",
	"logicaland","logicalnot","logicalnotreversed","logicalor","lolingthai",
	"longs","lowlinecenterline","lowlinecmb","lowlinedashed","lozenge","lparen",
	"lslash","lsquare","lsuperior","ltshade","luthai","lvocalicbengali",
	"lvocalicdeva","lvocalicvowelsignbengali","lvocalicvowelsigndeva","lxsquare",
	"m","mabengali","macron","macronbelowcmb","macroncmb","macronlowmod",
	"macronmonospace","macute","madeva","magujarati","magurmukhi",
	"mahapakhhebrew","mahapakhlefthebrew","mahiragana","maichattawalowleftthai",
	"maichattawalowrightthai","maichattawathai","maichattawaupperleftthai",
	"maieklowleftthai","maieklowrightthai","maiekthai","maiekupperleftthai",
	"maihanakatleftthai","maihanakatthai","maitaikhuleftthai","maitaikhuthai",
	"maitholowleftthai","maitholowrightthai","maithothai","maithoupperleftthai",
	"maitrilowleftthai","maitrilowrightthai","maitrithai","maitriupperleftthai",
	"maiyamokthai","makatakana","makatakanahalfwidth","male","mansyonsquare",
	"maqafhebrew","mars","masoracirclehebrew","masquare","mbopomofo","mbsquare",
	"mcircle","mcubedsquare","mdotaccent","mdotbelow","meemarabic",
	"meemfinalarabic","meeminitialarabic","meemmedialarabic",
	"meemmeeminitialarabic","meemmeemisolatedarabic","meetorusquare","mehiragana",
	"meizierasquare","mekatakana","mekatakanahalfwidth","mem","memdagesh",
	"memdageshhebrew","memhebrew","menarmenian","merkhahebrew",
	"merkhakefulahebrew","merkhakefulalefthebrew","merkhalefthebrew","mhook",
	"mhzsquare","middledotkatakanahalfwidth","middot","mieumacirclekorean",
	"mieumaparenkorean","mieumcirclekorean","mieumkorean","mieumpansioskorean",
	"mieumparenkorean","mieumpieupkorean","mieumsioskorean","mihiragana",
	"mikatakana","mikatakanahalfwidth","minus","minusbelowcmb","minuscircle",
	"minusmod","minusplus","minute","miribaarusquare","mirisquare",
	"mlonglegturned","mlsquare","mmcubedsquare","mmonospace","mmsquaredsquare",
	"mohiragana","mohmsquare","mokatakana","mokatakanahalfwidth","molsquare",
	"momathai","moverssquare","moverssquaredsquare","mparen","mpasquare",
	"mssquare","msuperior","mturned","mu","mu1","muasquare","muchgreater",
	"muchless","mufsquare","mugreek","mugsquare","muhiragana","mukatakana",
	"mukatakanahalfwidth","mulsquare","multiply","mumsquare","munahhebrew",
	"munahlefthebrew","musicalnote","musicalnotedbl","musicflatsign",
	"musicsharpsign","mussquare","muvsquare","muwsquare","mvmegasquare",
	"mvsquare","mwmegasquare","mwsquare","n","nabengali","nabla","nacute",
	"nadeva","nagujarati","nagurmukhi","nahiragana","nakatakana",
	"nakatakanahalfwidth","napostrophe","nasquare","nbopomofo","nbspace","ncaron",
	"ncedilla","ncircle","ncircumflexbelow","ncommaaccent","ndotaccent",
	"ndotbelow","nehiragana","nekatakana","nekatakanahalfwidth","newsheqelsign",
	"nfsquare","ngabengali","ngadeva","ngagujarati","ngagurmukhi","ngonguthai",
	"nhiragana","nhookleft","nhookretroflex","nieunacirclekorean",
	"nieunaparenkorean","nieuncieuckorean","nieuncirclekorean","nieunhieuhkorean",
	"nieunkorean","nieunpansioskorean","nieunparenkorean","nieunsioskorean",
	"nieuntikeutkorean","nihiragana","nikatakana","nikatakanahalfwidth",
	"nikhahitleftthai","nikhahitthai","nine","ninearabic","ninebengali",
	"ninecircle","ninecircleinversesansserif","ninedeva","ninegujarati",
	"ninegurmukhi","ninehackarabic","ninehangzhou","nineideographicparen",
	"nineinferior","ninemonospace","nineoldstyle","nineparen","nineperiod",
	"ninepersian","nineroman","ninesuperior","nineteencircle","nineteenparen",
	"nineteenperiod","ninethai","nj","njecyrillic","nkatakana",
	"nkatakanahalfwidth","nlegrightlong","nlinebelow","nmonospace","nmsquare",
	"nnabengali","nnadeva","nnagujarati","nnagurmukhi","nnnadeva","nohiragana",
	"nokatakana","nokatakanahalfwidth","nonbreakingspace","nonenthai","nonuthai",
	"noonarabic","noonfinalarabic","noonghunnaarabic","noonghunnafinalarabic",
	"noonhehinitialarabic","nooninitialarabic","noonjeeminitialarabic",
	"noonjeemisolatedarabic","noonmedialarabic","noonmeeminitialarabic",
	"noonmeemisolatedarabic","noonnoonfinalarabic","notcontains","notelement",
	"notelementof","notequal","notgreater","notgreaternorequal",
	"notgreaternorless","notidentical","notless","notlessnorequal","notparallel",
	"notprecedes","notsubset","notsucceeds","notsuperset","nowarmenian","nparen",
	"nssquare","nsuperior","ntilde","nu","nuhiragana","nukatakana",
	"nukatakanahalfwidth","nuktabengali","nuktadeva","nuktagujarati",
	"nuktagurmukhi","numbersign","numbersignmonospace","numbersignsmall",
	"numeralsigngreek","numeralsignlowergreek","numero","nun","nundagesh",
	"nundageshhebrew","nunhebrew","nvsquare","nwsquare","nyabengali","nyadeva",
	"nyagujarati","nyagurmukhi","o","oacute","oangthai","obarred",
	"obarredcyrillic","obarreddieresiscyrillic","obengali","obopomofo","obreve",
	"ocandradeva","ocandragujarati","ocandravowelsigndeva",
	"ocandravowelsigngujarati","ocaron","ocircle","ocircumflex",
	"ocircumflexacute","ocircumflexdotbelow","ocircumflexgrave",
	"ocircumflexhookabove","ocircumflextilde","ocyrillic","odblacute","odblgrave",
	"odeva","odieresis","odieresiscyrillic","odotbelow","oe","oekorean","ogonek",
	"ogonekcmb","ograve","ogujarati","oharmenian","ohiragana","ohookabove",
	"ohorn","ohornacute","ohorndotbelow","ohorngrave","ohornhookabove",
	"ohorntilde","ohungarumlaut","oi","oinvertedbreve","okatakana",
	"okatakanahalfwidth","okorean","olehebrew","omacron","omacronacute",
	"omacrongrave","omdeva","omega","omega1","omegacyrillic","omegalatinclosed",
	"omegaroundcyrillic","omegatitlocyrillic","omegatonos","omgujarati","omicron",
	"omicrontonos","omonospace","one","onearabic","onebengali","onecircle",
	"onecircleinversesansserif","onedeva","onedotenleader","oneeighth",
	"onefitted","onegujarati","onegurmukhi","onehackarabic","onehalf",
	"onehangzhou","oneideographicparen","oneinferior","onemonospace",
	"onenumeratorbengali","oneoldstyle","oneparen","oneperiod","onepersian",
	"onequarter","oneroman","onesuperior","onethai","onethird","oogonek",
	"oogonekmacron","oogurmukhi","oomatragurmukhi","oopen","oparen","openbullet",
	"option","ordfeminine","ordmasculine","orthogonal","oshortdeva",
	"oshortvowelsigndeva","oslash","oslashacute","osmallhiragana",
	"osmallkatakana","osmallkatakanahalfwidth","ostrokeacute","osuperior",
	"otcyrillic","otilde","otildeacute","otildedieresis","oubopomofo","overline",
	"overlinecenterline","overlinecmb","overlinedashed","overlinedblwavy",
	"overlinewavy","overscore","ovowelsignbengali","ovowelsigndeva",
	"ovowelsigngujarati","p","paampssquare","paasentosquare","pabengali","pacute",
	"padeva","pagedown","pageup","pagujarati","pagurmukhi","pahiragana",
	"paiyannoithai","pakatakana","palatalizationcyrilliccmb","palochkacyrillic",
	"pansioskorean","paragraph","parallel","parenleft","parenleftaltonearabic",
	"parenleftbt","parenleftex","parenleftinferior","parenleftmonospace",
	"parenleftsmall","parenleftsuperior","parenlefttp","parenleftvertical",
	"parenright","parenrightaltonearabic","parenrightbt","parenrightex",
	"parenrightinferior","parenrightmonospace","parenrightsmall",
	"parenrightsuperior","parenrighttp","parenrightvertical","partialdiff",
	"paseqhebrew","pashtahebrew","pasquare","patah","patah11","patah1d","patah2a",
	"patahhebrew","patahnarrowhebrew","patahquarterhebrew","patahwidehebrew",
	"pazerhebrew","pbopomofo","pcircle","pdotaccent","pe","pecyrillic","pedagesh",
	"pedageshhebrew","peezisquare","pefinaldageshhebrew","peharabic",
	"peharmenian","pehebrew","pehfinalarabic","pehinitialarabic","pehiragana",
	"pehmedialarabic","pekatakana","pemiddlehookcyrillic","perafehebrew",
	"percent","percentarabic","percentmonospace","percentsmall","period",
	"periodarmenian","periodcentered","periodhalfwidth","periodinferior",
	"periodmonospace","periodsmall","periodsuperior","perispomenigreekcmb",
	"perpendicular","perthousand","peseta","pfsquare","phabengali","phadeva",
	"phagujarati","phagurmukhi","phi","phi1","phieuphacirclekorean",
	"phieuphaparenkorean","phieuphcirclekorean","phieuphkorean",
	"phieuphparenkorean","philatin","phinthuthai","phisymbolgreek","phook",
	"phophanthai","phophungthai","phosamphaothai","pi","pieupacirclekorean",
	"pieupaparenkorean","pieupcieuckorean","pieupcirclekorean",
	"pieupkiyeokkorean","pieupkorean","pieupparenkorean","pieupsioskiyeokkorean",
	"pieupsioskorean","pieupsiostikeutkorean","pieupthieuthkorean",
	"pieuptikeutkorean","pihiragana","pikatakana","pisymbolgreek","piwrarmenian",
	"plus","plusbelowcmb","pluscircle","plusminus","plusmod","plusmonospace",
	"plussmall","plussuperior","pmonospace","pmsquare","pohiragana",
	"pointingindexdownwhite","pointingindexleftwhite","pointingindexrightwhite",
	"pointingindexupwhite","pokatakana","poplathai","postalmark","postalmarkface",
	"pparen","precedes","prescription","primemod","primereversed","product",
	"projective","prolongedkana","propellor","propersubset","propersuperset",
	"proportion","proportional","psi","psicyrillic","psilipneumatacyrilliccmb",
	"pssquare","puhiragana","pukatakana","pvsquare","pwsquare","q","qadeva",
	"qadmahebrew","qafarabic","qaffinalarabic","qafinitialarabic",
	"qafmedialarabic","qamats","qamats10","qamats1a","qamats1c","qamats27",
	"qamats29","qamats33","qamatsde","qamatshebrew","qamatsnarrowhebrew",
	"qamatsqatanhebrew","qamatsqatannarrowhebrew","qamatsqatanquarterhebrew",
	"qamatsqatanwidehebrew","qamatsquarterhebrew","qamatswidehebrew",
	"qarneyparahebrew","qbopomofo","qcircle","qhook","qmonospace","qof",
	"qofdagesh","qofdageshhebrew","qofhatafpatah","qofhatafpatahhebrew",
	"qofhatafsegol","qofhatafsegolhebrew","qofhebrew","qofhiriq","qofhiriqhebrew",
	"qofholam","qofholamhebrew","qofpatah","qofpatahhebrew","qofqamats",
	"qofqamatshebrew","qofqubuts","qofqubutshebrew","qofsegol","qofsegolhebrew",
	"qofsheva","qofshevahebrew","qoftsere","qoftserehebrew","qparen",
	"quarternote","qubuts","qubuts18","qubuts25","qubuts31","qubutshebrew",
	"qubutsnarrowhebrew","qubutsquarterhebrew","qubutswidehebrew","question",
	"questionarabic","questionarmenian","questiondown","questiondownsmall",
	"questiongreek","questionmonospace","questionsmall","quotedbl","quotedblbase",
	"quotedblleft","quotedblmonospace","quotedblprime","quotedblprimereversed",
	"quotedblright","quoteleft","quoteleftreversed","quotereversed","quoteright",
	"quoterightn","quotesinglbase","quotesingle","quotesinglemonospace","r",
	"raarmenian","rabengali","racute","radeva","radical","radicalex",
	"radoverssquare","radoverssquaredsquare","radsquare","rafe","rafehebrew",
	"ragujarati","ragurmukhi","rahiragana","rakatakana","rakatakanahalfwidth",
	"ralowerdiagonalbengali","ramiddlediagonalbengali","ramshorn","ratio",
	"rbopomofo","rcaron","rcedilla","rcircle","rcommaaccent","rdblgrave",
	"rdotaccent","rdotbelow","rdotbelowmacron","referencemark","reflexsubset",
	"reflexsuperset","registered","registersans","registerserif","reharabic",
	"reharmenian","rehfinalarabic","rehiragana","rehyehaleflamarabic",
	"rekatakana","rekatakanahalfwidth","resh","reshdageshhebrew","reshhatafpatah",
	"reshhatafpatahhebrew","reshhatafsegol","reshhatafsegolhebrew","reshhebrew",
	"reshhiriq","reshhiriqhebrew","reshholam","reshholamhebrew","reshpatah",
	"reshpatahhebrew","reshqamats","reshqamatshebrew","reshqubuts",
	"reshqubutshebrew","reshsegol","reshsegolhebrew","reshsheva",
	"reshshevahebrew","reshtsere","reshtserehebrew","reversedtilde","reviahebrew",
	"reviamugrashhebrew","revlogicalnot","rfishhook","rfishhookreversed",
	"rhabengali","rhadeva","rho","rhook","rhookturned","rhookturnedsuperior",
	"rhosymbolgreek","rhotichookmod","rieulacirclekorean","rieulaparenkorean",
	"rieulcirclekorean","rieulhieuhkorean","rieulkiyeokkorean",
	"rieulkiyeoksioskorean","rieulkorean","rieulmieumkorean","rieulpansioskorean",
	"rieulparenkorean","rieulphieuphkorean","rieulpieupkorean",
	"rieulpieupsioskorean","rieulsioskorean","rieulthieuthkorean",
	"rieultikeutkorean","rieulyeorinhieuhkorean","rightangle","righttackbelowcmb",
	"righttriangle","rihiragana","rikatakana","rikatakanahalfwidth","ring",
	"ringbelowcmb","ringcmb","ringhalfleft","ringhalfleftarmenian",
	"ringhalfleftbelowcmb","ringhalfleftcentered","ringhalfright",
	"ringhalfrightbelowcmb","ringhalfrightcentered","rinvertedbreve",
	"rittorusquare","rlinebelow","rlongleg","rlonglegturned","rmonospace",
	"rohiragana","rokatakana","rokatakanahalfwidth","roruathai","rparen",
	"rrabengali","rradeva","rragurmukhi","rreharabic","rrehfinalarabic",
	"rrvocalicbengali","rrvocalicdeva","rrvocalicgujarati",
	"rrvocalicvowelsignbengali","rrvocalicvowelsigndeva",
	"rrvocalicvowelsigngujarati","rsuperior","rtblock","rturned",
	"rturnedsuperior","ruhiragana","rukatakana","rukatakanahalfwidth",
	"rupeemarkbengali","rupeesignbengali","rupiah","ruthai","rvocalicbengali",
	"rvocalicdeva","rvocalicgujarati","rvocalicvowelsignbengali",
	"rvocalicvowelsigndeva","rvocalicvowelsigngujarati","s","sabengali","sacute",
	"sacutedotaccent","sadarabic","sadeva","sadfinalarabic","sadinitialarabic",
	"sadmedialarabic","sagujarati","sagurmukhi","sahiragana","sakatakana",
	"sakatakanahalfwidth","sallallahoualayhewasallamarabic","samekh",
	"samekhdagesh","samekhdageshhebrew","samekhhebrew","saraaathai","saraaethai",
	"saraaimaimalaithai","saraaimaimuanthai","saraamthai","saraathai","saraethai",
	"saraiileftthai","saraiithai","saraileftthai","saraithai","saraothai",
	"saraueeleftthai","saraueethai","saraueleftthai","sarauethai","sarauthai",
	"sarauuthai","sbopomofo","scaron","scarondotaccent","scedilla","schwa",
	"schwacyrillic","schwadieresiscyrillic","schwahook","scircle","scircumflex",
	"scommaaccent","sdotaccent","sdotbelow","sdotbelowdotaccent",
	"seagullbelowcmb","second","secondtonechinese","section","seenarabic",
	"seenfinalarabic","seeninitialarabic","seenmedialarabic","segol","segol13",
	"segol1f","segol2c","segolhebrew","segolnarrowhebrew","segolquarterhebrew",
	"segoltahebrew","segolwidehebrew","seharmenian","sehiragana","sekatakana",
	"sekatakanahalfwidth","semicolon","semicolonarabic","semicolonmonospace",
	"semicolonsmall","semivoicedmarkkana","semivoicedmarkkanahalfwidth",
	"sentisquare","sentosquare","seven","sevenarabic","sevenbengali",
	"sevencircle","sevencircleinversesansserif","sevendeva","seveneighths",
	"sevengujarati","sevengurmukhi","sevenhackarabic","sevenhangzhou",
	"sevenideographicparen","seveninferior","sevenmonospace","sevenoldstyle",
	"sevenparen","sevenperiod","sevenpersian","sevenroman","sevensuperior",
	"seventeencircle","seventeenparen","seventeenperiod","seventhai","sfthyphen",
	"shaarmenian","shabengali","shacyrillic","shaddaarabic","shaddadammaarabic",
	"shaddadammatanarabic","shaddafathaarabic","shaddafathatanarabic",
	"shaddakasraarabic","shaddakasratanarabic","shade","shadedark","shadelight",
	"shademedium","shadeva","shagujarati","shagurmukhi","shalshelethebrew",
	"shbopomofo","shchacyrillic","sheenarabic","sheenfinalarabic",
	"sheeninitialarabic","sheenmedialarabic","sheicoptic","sheqel","sheqelhebrew",
	"sheva","sheva115","sheva15","sheva22","sheva2e","shevahebrew",
	"shevanarrowhebrew","shevaquarterhebrew","shevawidehebrew","shhacyrillic",
	"shimacoptic","shin","shindagesh","shindageshhebrew","shindageshshindot",
	"shindageshshindothebrew","shindageshsindot","shindageshsindothebrew",
	"shindothebrew","shinhebrew","shinshindot","shinshindothebrew","shinsindot",
	"shinsindothebrew","shook","sigma","sigma1","sigmafinal",
	"sigmalunatesymbolgreek","sihiragana","sikatakana","sikatakanahalfwidth",
	"siluqhebrew","siluqlefthebrew","similar","sindothebrew","siosacirclekorean",
	"siosaparenkorean","sioscieuckorean","sioscirclekorean","sioskiyeokkorean",
	"sioskorean","siosnieunkorean","siosparenkorean","siospieupkorean",
	"siostikeutkorean","six","sixarabic","sixbengali","sixcircle",
	"sixcircleinversesansserif","sixdeva","sixgujarati","sixgurmukhi",
	"sixhackarabic","sixhangzhou","sixideographicparen","sixinferior",
	"sixmonospace","sixoldstyle","sixparen","sixperiod","sixpersian","sixroman",
	"sixsuperior","sixteencircle","sixteencurrencydenominatorbengali",
	"sixteenparen","sixteenperiod","sixthai","slash","slashmonospace","slong",
	"slongdotaccent","smileface","smonospace","sofpasuqhebrew","softhyphen",
	"softsigncyrillic","sohiragana","sokatakana","sokatakanahalfwidth",
	"soliduslongoverlaycmb","solidusshortoverlaycmb","sorusithai","sosalathai",
	"sosothai","sosuathai","space","spacehackarabic","spade","spadesuitblack",
	"spadesuitwhite","sparen","squarebelowcmb","squarecc","squarecm",
	"squarediagonalcrosshatchfill","squarehorizontalfill","squarekg","squarekm",
	"squarekmcapital","squareln","squarelog","squaremg","squaremil","squaremm",
	"squaremsquared","squareorthogonalcrosshatchfill",
	"squareupperlefttolowerrightfill","squareupperrighttolowerleftfill",
	"squareverticalfill","squarewhitewithsmallblack","srsquare","ssabengali",
	"ssadeva","ssagujarati","ssangcieuckorean","ssanghieuhkorean",
	"ssangieungkorean","ssangkiyeokkorean","ssangnieunkorean","ssangpieupkorean",
	"ssangsioskorean","ssangtikeutkorean","ssuperior","sterling",
	"sterlingmonospace","strokelongoverlaycmb","strokeshortoverlaycmb","subset",
	"subsetnotequal","subsetorequal","succeeds","suchthat","suhiragana",
	"sukatakana","sukatakanahalfwidth","sukunarabic","summation","sun","superset",
	"supersetnotequal","supersetorequal","svsquare","syouwaerasquare","t",
	"tabengali","tackdown","tackleft","tadeva","tagujarati","tagurmukhi",
	"taharabic","tahfinalarabic","tahinitialarabic","tahiragana",
	"tahmedialarabic","taisyouerasquare","takatakana","takatakanahalfwidth",
	"tatweelarabic","tau","tav","tavdages","tavdagesh","tavdageshhebrew",
	"tavhebrew","tbar","tbopomofo","tcaron","tccurl","tcedilla","tcheharabic",
	"tchehfinalarabic","tchehinitialarabic","tchehmedialarabic",
	"tchehmeeminitialarabic","tcircle","tcircumflexbelow","tcommaaccent",
	"tdieresis","tdotaccent","tdotbelow","tecyrillic","tedescendercyrillic",
	"teharabic","tehfinalarabic","tehhahinitialarabic","tehhahisolatedarabic",
	"tehinitialarabic","tehiragana","tehjeeminitialarabic",
	"tehjeemisolatedarabic","tehmarbutaarabic","tehmarbutafinalarabic",
	"tehmedialarabic","tehmeeminitialarabic","tehmeemisolatedarabic",
	"tehnoonfinalarabic","tekatakana","tekatakanahalfwidth","telephone",
	"telephoneblack","telishagedolahebrew","telishaqetanahebrew","tencircle",
	"tenideographicparen","tenparen","tenperiod","tenroman","tesh","tet",
	"tetdagesh","tetdageshhebrew","tethebrew","tetsecyrillic","tevirhebrew",
	"tevirlefthebrew","thabengali","thadeva","thagujarati","thagurmukhi",
	"thalarabic","thalfinalarabic","thanthakhatlowleftthai",
	"thanthakhatlowrightthai","thanthakhatthai","thanthakhatupperleftthai",
	"theharabic","thehfinalarabic","thehinitialarabic","thehmedialarabic",
	"thereexists","therefore","theta","theta1","thetasymbolgreek",
	"thieuthacirclekorean","thieuthaparenkorean","thieuthcirclekorean",
	"thieuthkorean","thieuthparenkorean","thirteencircle","thirteenparen",
	"thirteenperiod","thonangmonthothai","thook","thophuthaothai","thorn",
	"thothahanthai","thothanthai","thothongthai","thothungthai",
	"thousandcyrillic","thousandsseparatorarabic","thousandsseparatorpersian",
	"three","threearabic","threebengali","threecircle",
	"threecircleinversesansserif","threedeva","threeeighths","threegujarati",
	"threegurmukhi","threehackarabic","threehangzhou","threeideographicparen",
	"threeinferior","threemonospace","threenumeratorbengali","threeoldstyle",
	"threeparen","threeperiod","threepersian","threequarters",
	"threequartersemdash","threeroman","threesuperior","threethai","thzsquare",
	"tihiragana","tikatakana","tikatakanahalfwidth","tikeutacirclekorean",
	"tikeutaparenkorean","tikeutcirclekorean","tikeutkorean","tikeutparenkorean",
	"tilde","tildebelowcmb","tildecmb","tildecomb","tildedoublecmb",
	"tildeoperator","tildeoverlaycmb","tildeverticalcmb","timescircle",
	"tipehahebrew","tipehalefthebrew","tippigurmukhi","titlocyrilliccmb",
	"tiwnarmenian","tlinebelow","tmonospace","toarmenian","tohiragana",
	"tokatakana","tokatakanahalfwidth","tonebarextrahighmod","tonebarextralowmod",
	"tonebarhighmod","tonebarlowmod","tonebarmidmod","tonefive","tonesix",
	"tonetwo","tonos","tonsquare","topatakthai","tortoiseshellbracketleft",
	"tortoiseshellbracketleftsmall","tortoiseshellbracketleftvertical",
	"tortoiseshellbracketright","tortoiseshellbracketrightsmall",
	"tortoiseshellbracketrightvertical","totaothai","tpalatalhook","tparen",
	"trademark","trademarksans","trademarkserif","tretroflexhook","triagdn",
	"triaglf","triagrt","triagup","ts","tsadi","tsadidagesh","tsadidageshhebrew",
	"tsadihebrew","tsecyrillic","tsere","tsere12","tsere1e","tsere2b",
	"tserehebrew","tserenarrowhebrew","tserequarterhebrew","tserewidehebrew",
	"tshecyrillic","tsuperior","ttabengali","ttadeva","ttagujarati","ttagurmukhi",
	"tteharabic","ttehfinalarabic","ttehinitialarabic","ttehmedialarabic",
	"tthabengali","tthadeva","tthagujarati","tthagurmukhi","tturned","tuhiragana",
	"tukatakana","tukatakanahalfwidth","tusmallhiragana","tusmallkatakana",
	"tusmallkatakanahalfwidth","twelvecircle","twelveparen","twelveperiod",
	"twelveroman","twentycircle","twentyhangzhou","twentyparen","twentyperiod",
	"two","twoarabic","twobengali","twocircle","twocircleinversesansserif",
	"twodeva","twodotenleader","twodotleader","twodotleadervertical",
	"twogujarati","twogurmukhi","twohackarabic","twohangzhou",
	"twoideographicparen","twoinferior","twomonospace","twonumeratorbengali",
	"twooldstyle","twoparen","twoperiod","twopersian","tworoman","twostroke",
	"twosuperior","twothai","twothirds","u","uacute","ubar","ubengali",
	"ubopomofo","ubreve","ucaron","ucircle","ucircumflex","ucircumflexbelow",
	"ucyrillic","udattadeva","udblacute","udblgrave","udeva","udieresis",
	"udieresisacute","udieresisbelow","udieresiscaron","udieresiscyrillic",
	"udieresisgrave","udieresismacron","udotbelow","ugrave","ugujarati",
	"ugurmukhi","uhiragana","uhookabove","uhorn","uhornacute","uhorndotbelow",
	"uhorngrave","uhornhookabove","uhorntilde","uhungarumlaut",
	"uhungarumlautcyrillic","uinvertedbreve","ukatakana","ukatakanahalfwidth",
	"ukcyrillic","ukorean","umacron","umacroncyrillic","umacrondieresis",
	"umatragurmukhi","umonospace","underscore","underscoredbl",
	"underscoremonospace","underscorevertical","underscorewavy","union",
	"universal","uogonek","uparen","upblock","upperdothebrew","upsilon",
	"upsilondieresis","upsilondieresistonos","upsilonlatin","upsilontonos",
	"uptackbelowcmb","uptackmod","uragurmukhi","uring","ushortcyrillic",
	"usmallhiragana","usmallkatakana","usmallkatakanahalfwidth",
	"ustraightcyrillic","ustraightstrokecyrillic","utilde","utildeacute",
	"utildebelow","uubengali","uudeva","uugujarati","uugurmukhi",
	"uumatragurmukhi","uuvowelsignbengali","uuvowelsigndeva",
	"uuvowelsigngujarati","uvowelsignbengali","uvowelsigndeva",
	"uvowelsigngujarati","v","vadeva","vagujarati","vagurmukhi","vakatakana",
	"vav","vavdagesh","vavdagesh65","vavdageshhebrew","vavhebrew","vavholam",
	"vavholamhebrew","vavvavhebrew","vavyodhebrew","vcircle","vdotbelow",
	"vecyrillic","veharabic","vehfinalarabic","vehinitialarabic",
	"vehmedialarabic","vekatakana","venus","verticalbar","verticallineabovecmb",
	"verticallinebelowcmb","verticallinelowmod","verticallinemod","vewarmenian",
	"vhook","vikatakana","viramabengali","viramadeva","viramagujarati",
	"visargabengali","visargadeva","visargagujarati","vmonospace","voarmenian",
	"voicediterationhiragana","voicediterationkatakana","voicedmarkkana",
	"voicedmarkkanahalfwidth","vokatakana","vparen","vtilde","vturned",
	"vuhiragana","vukatakana","w","wacute","waekorean","wahiragana","wakatakana",
	"wakatakanahalfwidth","wakorean","wasmallhiragana","wasmallkatakana",
	"wattosquare","wavedash","wavyunderscorevertical","wawarabic",
	"wawfinalarabic","wawhamzaabovearabic","wawhamzaabovefinalarabic","wbsquare",
	"wcircle","wcircumflex","wdieresis","wdotaccent","wdotbelow","wehiragana",
	"weierstrass","wekatakana","wekorean","weokorean","wgrave","whitebullet",
	"whitecircle","whitecircleinverse","whitecornerbracketleft",
	"whitecornerbracketleftvertical","whitecornerbracketright",
	"whitecornerbracketrightvertical","whitediamond",
	"whitediamondcontainingblacksmalldiamond","whitedownpointingsmalltriangle",
	"whitedownpointingtriangle","whiteleftpointingsmalltriangle",
	"whiteleftpointingtriangle","whitelenticularbracketleft",
	"whitelenticularbracketright","whiterightpointingsmalltriangle",
	"whiterightpointingtriangle","whitesmallsquare","whitesmilingface",
	"whitesquare","whitestar","whitetelephone","whitetortoiseshellbracketleft",
	"whitetortoiseshellbracketright","whiteuppointingsmalltriangle",
	"whiteuppointingtriangle","wihiragana","wikatakana","wikorean","wmonospace",
	"wohiragana","wokatakana","wokatakanahalfwidth","won","wonmonospace",
	"wowaenthai","wparen","wring","wsuperior","wturned","wynn","x","xabovecmb",
	"xbopomofo","xcircle","xdieresis","xdotaccent","xeharmenian","xi",
	"xmonospace","xparen","xsuperior","y","yaadosquare","yabengali","yacute",
	"yadeva","yaekorean","yagujarati","yagurmukhi","yahiragana","yakatakana",
	"yakatakanahalfwidth","yakorean","yamakkanthai","yasmallhiragana",
	"yasmallkatakana","yasmallkatakanahalfwidth","yatcyrillic","ycircle",
	"ycircumflex","ydieresis","ydotaccent","ydotbelow","yeharabic",
	"yehbarreearabic","yehbarreefinalarabic","yehfinalarabic",
	"yehhamzaabovearabic","yehhamzaabovefinalarabic","yehhamzaaboveinitialarabic",
	"yehhamzaabovemedialarabic","yehinitialarabic","yehmedialarabic",
	"yehmeeminitialarabic","yehmeemisolatedarabic","yehnoonfinalarabic",
	"yehthreedotsbelowarabic","yekorean","yen","yenmonospace","yeokorean",
	"yeorinhieuhkorean","yerahbenyomohebrew","yerahbenyomolefthebrew",
	"yericyrillic","yerudieresiscyrillic","yesieungkorean",
	"yesieungpansioskorean","yesieungsioskorean","yetivhebrew","ygrave","yhook",
	"yhookabove","yiarmenian","yicyrillic","yikorean","yinyang","yiwnarmenian",
	"ymonospace","yod","yoddagesh","yoddageshhebrew","yodhebrew","yodyodhebrew",
	"yodyodpatahhebrew","yohiragana","yoikorean","yokatakana",
	"yokatakanahalfwidth","yokorean","yosmallhiragana","yosmallkatakana",
	"yosmallkatakanahalfwidth","yotgreek","yoyaekorean","yoyakorean","yoyakthai",
	"yoyingthai","yparen","ypogegrammeni","ypogegrammenigreekcmb","yr","yring",
	"ysuperior","ytilde","yturned","yuhiragana","yuikorean","yukatakana",
	"yukatakanahalfwidth","yukorean","yusbigcyrillic","yusbigiotifiedcyrillic",
	"yuslittlecyrillic","yuslittleiotifiedcyrillic","yusmallhiragana",
	"yusmallkatakana","yusmallkatakanahalfwidth","yuyekorean","yuyeokorean",
	"yyabengali","yyadeva","z","zaarmenian","zacute","zadeva","zagurmukhi",
	"zaharabic","zahfinalarabic","zahinitialarabic","zahiragana",
	"zahmedialarabic","zainarabic","zainfinalarabic","zakatakana",
	"zaqefgadolhebrew","zaqefqatanhebrew","zarqahebrew","zayin","zayindagesh",
	"zayindageshhebrew","zayinhebrew","zbopomofo","zcaron","zcircle",
	"zcircumflex","zcurl","zdot","zdotaccent","zdotbelow","zecyrillic",
	"zedescendercyrillic","zedieresiscyrillic","zehiragana","zekatakana","zero",
	"zeroarabic","zerobengali","zerodeva","zerogujarati","zerogurmukhi",
	"zerohackarabic","zeroinferior","zeromonospace","zerooldstyle","zeropersian",
	"zerosuperior","zerothai","zerowidthjoiner","zerowidthnonjoiner",
	"zerowidthspace","zeta","zhbopomofo","zhearmenian","zhebrevecyrillic",
	"zhecyrillic","zhedescendercyrillic","zhedieresiscyrillic","zihiragana",
	"zikatakana","zinorhebrew","zlinebelow","zmonospace","zohiragana",
	"zokatakana","zparen","zretroflexhook","zstroke","zuhiragana","zukatakana",
};

static const unsigned short agl_code_list[] = {
	65,198,508,482,63462,193,63457,258,7854,1232,7862,7856,7858,7860,461,9398,194,
	7844,7852,7846,7848,63458,7850,63177,63412,1040,512,196,1234,478,63460,7840,
	480,192,63456,7842,1236,514,913,902,256,65313,260,197,506,7680,63461,63329,
	195,63459,1329,66,9399,7682,7684,1041,1330,914,385,7686,65314,63220,63330,386,
	67,1342,262,63178,63221,268,199,7688,63463,9400,264,266,266,63416,1353,1212,
	1063,1214,1206,1268,1347,1227,1208,935,391,63222,65315,1361,63331,68,497,452,
	1332,393,270,7696,9401,7698,272,7690,7692,1044,1006,8710,916,394,63179,63180,
	63181,63400,988,1026,7694,65316,63223,272,63332,395,498,453,1248,1029,1039,69,
	201,63465,276,282,7708,1333,9402,202,7870,7704,7878,7872,7874,63466,7876,1028,
	516,203,63467,278,278,7864,1060,200,63464,1335,7866,8551,518,1124,1051,8554,
	274,7702,7700,1052,65317,1053,1186,330,1188,1223,280,400,917,904,1056,398,
	1069,1057,1194,425,63333,919,1336,905,208,63472,7868,7706,8364,439,494,440,70,
	9403,7710,1366,996,401,1138,8548,65318,8547,63334,71,13191,500,915,404,1002,
	286,486,290,9404,284,290,288,288,1043,1346,1172,1170,1168,403,1331,1027,7712,
	65319,63182,63328,63335,667,484,72,9679,9642,9643,9633,13259,1192,1202,1066,
	294,7722,7720,9405,292,7718,7714,7716,65320,1344,1000,63336,63183,63224,13200,
	73,1071,306,1070,205,63469,300,463,9406,206,63470,1030,520,207,7726,1252,
	63471,304,304,7882,1238,1045,8465,204,63468,7880,1048,522,1049,298,1250,65321,
	1339,1025,302,921,406,938,906,63337,407,296,7724,1140,1142,74,1345,9407,308,
	1032,1355,65322,63338,75,13189,13261,1184,7728,1050,1178,1219,922,1182,1180,
	488,310,9408,310,7730,1364,1343,1061,998,408,1036,7732,65323,1152,990,1134,
	63339,76,455,63167,313,923,317,315,9409,7740,315,319,319,7734,7736,1340,456,
	1033,7738,65324,321,63225,63340,77,13190,63184,63407,7742,9410,7744,7746,1348,
	65325,63341,412,924,78,458,323,327,325,9411,7754,325,7748,7750,413,8552,459,
	1034,7752,65326,1350,63342,209,63473,925,79,338,63226,211,63475,1256,1258,334,
	465,415,9412,212,7888,7896,7890,7892,63476,7894,1054,336,524,214,1254,63478,
	7884,63227,210,63474,1365,8486,7886,416,7898,7906,7900,7902,7904,336,418,526,
	332,7762,7760,8486,1120,937,1146,1148,911,927,908,65327,8544,490,492,390,216,
	510,63480,63343,510,1150,213,7756,7758,63477,80,7764,9413,7766,1055,1354,1190,
	934,420,928,1363,65328,936,1136,63344,81,9414,65329,63345,82,1356,340,344,342,
	9415,342,528,7768,7770,7772,1360,8476,929,63228,530,7774,65330,63346,641,694,
	83,9484,9492,9488,9496,9532,9516,9524,9500,9508,9472,9474,9569,9570,9558,9557,
	9571,9553,9559,9565,9564,9563,9566,9567,9562,9556,9577,9574,9568,9552,9580,
	9575,9576,9572,9573,9561,9560,9554,9555,9579,9578,346,7780,992,352,7782,63229,
	350,399,1240,1242,9416,348,536,7776,7778,7784,1357,8550,1351,1064,1065,994,
	1210,1004,931,8549,65331,1068,63347,986,84,932,358,356,354,9417,7792,354,7786,
	7788,1058,1196,8553,1204,920,428,222,63486,8546,63230,1359,7790,65332,1337,
	444,388,423,430,1062,1035,63348,8555,8545,85,218,63482,364,467,9418,219,7798,
	63483,1059,368,532,220,471,7794,473,1264,475,469,63484,7908,217,63481,7910,
	431,7912,7920,7914,7916,7918,368,1266,534,1144,362,1262,7802,65333,370,933,
	978,979,433,939,980,978,910,366,1038,63349,1198,1200,360,7800,7796,86,9419,
	7806,1042,1358,434,65334,1352,63350,7804,87,7810,9420,372,7812,7814,7816,7808,
	65335,63351,88,9421,7820,7818,1341,926,65336,63352,89,221,63485,1122,9422,374,
	376,63487,7822,7924,1067,1272,7922,435,7926,1349,1031,1362,65337,63353,7928,
	1130,1132,1126,1128,90,1334,377,381,63231,9423,7824,379,379,7826,1047,1176,
	1246,918,1338,1217,1046,1174,1244,7828,65338,63354,437,97,2438,225,2310,2694,
	2566,2622,13059,2494,2366,2750,1375,2416,2437,12570,259,7855,1233,7863,7857,
	7859,7861,462,9424,226,7845,7853,7847,7849,7851,180,791,769,769,2388,719,833,
	1072,513,2673,2309,228,1235,479,7841,481,230,509,12624,483,8213,8356,1040,
	1041,1042,1043,1044,1045,1025,1046,1047,1048,1049,1050,1051,1052,1053,1054,
	1055,1056,1057,1058,1059,1060,1061,1062,1063,1064,1065,1066,1067,1068,1069,
	1070,1071,1168,1026,1027,1028,1029,1030,1031,1032,1033,1034,1035,1036,1038,
	63172,63173,1072,1073,1074,1075,1076,1077,1105,1078,1079,1080,1081,1082,1083,
	1084,1085,1086,1087,1088,1089,1090,1091,1092,1093,1094,1095,1096,1097,1098,
	1099,1100,1101,1102,1103,1169,1106,1107,1108,1109,1110,1111,1112,1113,1114,
	1115,1116,1118,1039,1122,1138,1140,63174,1119,1123,1139,1141,63175,63176,1241,
	8206,8207,8205,1642,1548,1632,1633,1634,1635,1636,1637,1638,1639,1640,1641,
	1563,1567,1569,1570,1571,1572,1573,1574,1575,1576,1577,1578,1579,1580,1581,
	1582,1583,1584,1585,1586,1587,1588,1589,1590,1591,1592,1593,1594,1600,1601,
	1602,1603,1604,1605,1606,1608,1609,1610,1611,1612,1613,1614,1615,1616,1617,
	1618,1607,1700,1662,1670,1688,1711,1657,1672,1681,1722,1746,1749,8362,1470,
	1475,1488,1489,1490,1491,1492,1493,1494,1495,1496,1497,1498,1499,1500,1501,
	1502,1503,1504,1505,1506,1507,1508,1509,1510,1511,1512,1513,1514,64298,64299,
	64331,64287,1520,1521,1522,64309,1460,1461,1462,1467,1464,1463,1456,1458,1457,
	1459,1474,1473,1465,1468,1469,1471,1472,700,8453,8467,8470,8236,8237,8238,
	8204,1645,701,224,2693,2565,12354,7843,2448,12574,2320,1237,2704,2576,2632,
	1593,65226,65227,65228,515,2504,2376,2760,12450,65393,12623,1488,1575,64304,
	65166,1571,65156,1573,65160,1488,64335,1570,65154,1609,65264,65267,65268,
	64302,64303,8501,8780,945,940,257,65345,38,65286,63270,13250,12578,12580,3674,
	8736,12296,65087,12297,65088,9001,9002,8491,903,2386,2434,2306,2690,261,13056,
	9372,1370,700,63743,8784,8776,8786,8773,12686,12685,8978,7834,229,507,7681,
	8596,8675,8672,8674,8673,8660,8659,8656,8658,8657,8595,8601,8600,8681,709,706,
	707,708,63719,8592,8656,8653,8646,8678,8594,8655,10142,8644,8680,8676,8677,
	8593,8597,8616,8616,8598,8645,8599,8679,63718,94,65342,126,65374,593,594,
	12353,12449,65383,42,1645,1645,8727,65290,65121,8258,63209,8771,64,227,65312,
	65131,592,2452,12576,2324,2708,2580,2519,2636,2508,2380,2764,2365,1377,1506,
	64288,1506,98,2476,92,65340,2348,2732,2604,12400,3647,12496,124,65372,12549,
	9425,7683,7685,9836,8757,1073,1576,65168,65169,12409,65170,64671,64520,64621,
	12505,1378,1489,946,976,64305,64305,1489,64332,2477,2349,2733,2605,595,12403,
	12499,664,2562,13105,9679,9670,9660,9668,9664,12304,65083,12305,65084,9699,
	9698,9644,9658,9654,9642,9787,9632,9733,9700,9701,9652,9650,9251,7687,9608,
	65346,3610,12412,12508,9373,13251,63732,123,63731,63730,65371,65115,63729,
	65079,125,63742,63741,65373,65116,63740,65080,91,63728,63727,65339,63726,93,
	63739,63738,65341,63737,728,814,774,815,785,865,810,826,166,384,63210,387,
	12406,12502,8226,9688,8729,9678,99,1390,2458,263,2330,2714,2586,13192,2433,
	784,2305,2689,8682,8453,711,812,780,8629,12568,269,231,7689,9426,265,597,267,
	267,13253,184,807,162,8451,63199,65504,63394,63200,1401,2459,2331,2715,2587,
	12564,1213,10003,1095,1215,1207,1269,1395,1228,1209,967,12919,12823,12905,
	12618,12809,3594,3592,3593,3596,392,12918,12822,12904,12616,12808,12828,9675,
	8855,8857,8853,12342,9680,9681,710,813,770,8999,450,448,449,451,9827,9827,
	9831,13220,65347,13216,1409,58,8353,65306,8353,65109,721,720,44,787,789,63171,
	1548,1373,63201,65292,788,701,65104,63202,786,699,9788,8773,8750,8963,6,7,8,
	24,13,17,18,19,20,127,16,25,5,4,27,23,3,12,28,29,9,10,21,30,15,14,2,1,26,22,
	31,11,169,63721,63193,12300,65378,65089,12301,65379,65090,13183,13255,13254,
	9374,8354,663,8911,8910,164,63185,63186,63188,63189,100,1380,2470,1590,2342,
	65214,65215,65216,1468,1468,8224,8225,2726,2598,12384,12480,1583,1491,64307,
	64307,1491,1491,1491,1491,1491,1491,1491,1491,1491,1491,1491,1491,1491,1491,
	1491,1491,1491,1491,1491,1491,1491,65194,1615,1615,1612,1612,2404,1447,1447,
	1157,63187,12298,65085,12299,65086,811,8660,8658,2405,63190,783,8748,8215,819,
	831,698,8214,782,12553,13256,271,7697,9427,7699,273,2465,2337,2721,2593,1672,
	64393,2396,2466,2338,2722,2594,7691,7693,1643,1643,1076,176,1453,12391,1007,
	12487,9003,8998,948,397,2552,676,2471,2343,2727,2599,599,901,836,9830,9826,
	168,63191,804,776,63192,901,12386,12482,12291,247,8739,8725,1106,9619,7695,
	13207,273,65348,9604,3598,3604,12393,12489,36,63203,65284,63268,65129,63204,
	8363,13094,729,775,803,803,12539,305,63166,644,8901,9676,64287,64287,798,725,
	9375,63211,598,396,12389,12485,499,675,454,677,1249,1109,1119,101,233,9793,
	2447,12572,277,2317,2701,2373,2757,283,7709,1381,1415,9428,234,7871,7705,7879,
	7873,7875,7877,1108,517,2319,235,279,279,7865,2575,2631,1092,232,2703,1383,
	12573,12360,7867,12575,56,1640,2542,9319,10129,2414,9329,9349,9369,2798,2670,
	1640,12328,9835,12839,8328,65304,63288,9339,9359,1784,8567,8312,3672,519,1125,
	12456,65396,2676,12628,1083,8712,9322,9342,9362,8570,8230,8942,275,7703,7701,
	1084,8212,65073,65349,1371,8709,12579,1085,8211,65074,1187,331,12581,1189,
	1224,8194,281,12627,603,666,604,606,605,9376,949,941,61,65309,65126,8316,8801,
	12582,1088,600,1101,1089,1195,643,646,2318,2374,426,645,12359,12455,65386,
	8494,63212,951,1384,942,240,7869,7707,1425,1425,1425,1425,477,12641,8364,2503,
	2375,2759,33,1372,8252,161,63393,65281,63265,8707,658,495,659,441,442,102,
	2398,2654,8457,1614,1614,1611,12552,9429,7711,1601,1414,65234,65235,65236,997,
	9792,64256,64259,64260,64257,9326,9346,9366,8210,9632,9644,1498,64314,64314,
	1498,1498,1498,1498,1498,1501,1501,1503,1503,1507,1507,1509,1509,713,9673,
	1139,53,1637,2539,9316,10126,2411,8541,2795,2667,1637,12325,12836,8325,65301,
	63285,9336,9356,1781,8564,8309,3669,64258,402,65350,13209,3615,3613,3663,8704,
	52,1636,2538,9315,10125,2410,2794,2666,1636,12324,12835,8324,65300,2551,63284,
	9335,9355,1780,8563,8308,9325,9345,9365,3668,715,9377,8260,8355,103,2455,501,
	2327,1711,64403,64404,64405,2711,2583,12364,12460,947,611,736,1003,12557,287,
	487,291,9430,285,291,289,289,1075,12370,12466,8785,1436,1523,1437,223,1438,
	1524,12307,2456,1394,2328,2712,2584,1594,65230,65231,65232,1173,1171,1169,
	2394,2650,608,13203,12366,12462,1379,1490,64306,64306,1490,1107,446,660,662,
	704,661,705,740,673,674,7713,65351,12372,12468,9378,13228,8711,96,790,768,768,
	2387,718,65344,832,62,8805,8923,65310,8819,8823,8807,65125,609,485,12368,171,
	187,8249,8250,12464,13080,13257,104,1193,1729,2489,1203,2361,2745,2617,1581,
	65186,65187,12399,65188,13098,12495,65418,2637,1569,1569,1569,1569,1569,1569,
	1569,1569,1569,12644,1098,8636,8640,13258,1458,1458,1458,1458,1458,1458,1458,
	1458,1459,1459,1459,1459,1459,1459,1459,1459,1457,1457,1457,1457,1457,1457,
	1457,1457,295,12559,7723,7721,9431,293,7719,7715,7717,1492,9829,9829,9825,
	64308,64308,1729,1607,1492,64423,65258,65258,64421,64420,64424,65259,12408,
	64425,65260,13179,12504,65421,13110,615,13113,1495,1495,614,689,12923,12827,
	12909,12622,12813,12402,12498,65419,1460,1460,1460,1460,1460,1460,1460,1460,
	7830,65352,1392,3627,12411,12507,65422,1465,1465,1465,1465,1465,1465,1465,
	1465,3630,777,777,801,802,13122,1001,8213,795,9832,8962,9379,688,613,12405,
	13107,12501,65420,733,779,405,45,63205,65293,65123,63206,8208,105,237,1103,
	2439,12583,301,464,9432,238,1110,521,12943,12939,12863,12858,12965,12294,
	12289,65380,12855,12963,12847,12861,12957,12864,12950,12854,12843,12850,12964,
	12293,12952,12856,12967,12966,12969,12846,12842,12852,12290,12958,12867,12857,
	12862,12968,12953,12866,12851,12288,12853,12849,12859,12848,12860,12844,12845,
	12295,12942,12938,12948,12944,12940,12941,2311,239,7727,1253,7883,1239,1077,
	12917,12821,12903,12615,12807,236,2695,2567,12356,7881,2440,1080,2312,2696,
	2568,2624,523,1081,2496,2368,2752,307,12452,65394,12643,732,1452,299,1251,
	8787,2623,65353,8710,8734,1387,8747,8993,8993,63733,8992,8992,8745,13061,9688,
	9689,9787,1105,303,953,970,912,617,943,9380,2674,12355,12451,65384,2554,616,
	63213,12445,12541,297,7725,12585,1102,2495,2367,2751,1141,1143,106,1393,2460,
	2332,2716,2588,12560,496,9433,309,669,607,1112,1580,65182,65183,65184,1688,
	64395,2461,2333,2717,2589,1403,12292,65354,9381,690,107,1185,2453,7729,1082,
	1179,2325,1499,1603,64315,64315,65242,1499,65243,65244,64333,2709,2581,12363,
	1220,12459,65398,954,1008,12657,12676,12664,12665,13069,1600,1600,12533,13188,
	1616,1613,1183,65392,1181,12558,13193,489,311,9434,311,7731,1412,12369,12465,
	65401,1391,12534,312,2454,1093,2326,2710,2582,1582,65190,65191,65192,999,2393,
	2649,12920,12824,12906,12619,12810,3586,3589,3587,3588,3675,409,3590,13201,
	12365,12461,65399,13077,13078,13076,12910,12814,12896,12593,12800,12595,1116,
	7733,13208,13222,65355,13218,12371,13248,3585,12467,65402,13086,1153,12927,
	835,9382,13226,1135,13263,670,12367,12463,65400,13240,13246,108,2482,314,2354,
	2738,2610,3653,65276,65272,65271,65274,65273,65275,65270,65269,1604,955,411,
	1500,64316,64316,1500,1500,1500,1500,1500,65246,64714,65247,64713,64715,65010,
	65248,64904,64716,65247,65247,9711,410,620,12556,318,316,9435,7741,316,320,
	320,7735,7737,794,792,60,8804,8922,65308,8818,8822,8806,65124,622,9612,621,
	8356,1388,457,1113,63168,2355,2739,7739,2356,2529,2401,2531,2403,619,65356,
	13264,3628,8743,172,8976,8744,3621,383,65102,818,65101,9674,9383,322,8467,
	63214,9617,3622,2444,2316,2530,2402,13267,109,2478,175,817,772,717,65507,7743,
	2350,2734,2606,1444,1444,12414,63637,63636,3659,63635,63628,63627,3656,63626,
	63620,3633,63625,3655,63631,63630,3657,63629,63634,63633,3658,63632,3654,
	12510,65423,9794,13127,1470,9794,1455,13187,12551,13268,9436,13221,7745,7747,
	1605,65250,65251,65252,64721,64584,13133,12417,13182,12513,65426,1502,64318,
	64318,1502,1396,1445,1446,1446,1445,625,13202,65381,183,12914,12818,12900,
	12609,12656,12804,12654,12655,12415,12511,65424,8722,800,8854,727,8723,8242,
	13130,13129,624,13206,13219,65357,13215,12418,13249,12514,65427,13270,3617,
	13223,13224,9384,13227,13235,63215,623,181,181,13186,8811,8810,13196,956,
	13197,12416,12512,65425,13205,215,13211,1443,1443,9834,9835,9837,9839,13234,
	13238,13244,13241,13239,13247,13245,110,2472,8711,324,2344,2728,2600,12394,
	12490,65413,329,13185,12555,160,328,326,9437,7755,326,7749,7751,12397,12493,
	65416,8362,13195,2457,2329,2713,2585,3591,12435,626,627,12911,12815,12597,
	12897,12598,12596,12648,12801,12647,12646,12395,12491,65414,63641,3661,57,
	1641,2543,9320,10130,2415,2799,2671,1641,12329,12840,8329,65305,63289,9340,
	9360,1785,8568,8313,9330,9350,9370,3673,460,1114,12531,65437,414,7753,65358,
	13210,2467,2339,2723,2595,2345,12398,12494,65417,160,3603,3609,1606,65254,
	1722,64415,65255,65255,64722,64587,65256,64725,64590,64653,8716,8713,8713,
	8800,8815,8817,8825,8802,8814,8816,8742,8832,8836,8833,8837,1398,9385,13233,
	8319,241,957,12396,12492,65415,2492,2364,2748,2620,35,65283,65119,884,885,
	8470,1504,64320,64320,1504,13237,13243,2462,2334,2718,2590,111,243,3629,629,
	1257,1259,2451,12571,335,2321,2705,2377,2761,466,9438,244,7889,7897,7891,7893,
	7895,1086,337,525,2323,246,1255,7885,339,12634,731,808,242,2707,1413,12362,
	7887,417,7899,7907,7901,7903,7905,337,419,527,12458,65397,12631,1451,333,7763,
	7761,2384,969,982,1121,631,1147,1149,974,2768,959,972,65359,49,1633,2535,9312,
	10122,2407,8228,8539,63196,2791,2663,1633,189,12321,12832,8321,65297,2548,
	63281,9332,9352,1777,188,8560,185,3665,8531,491,493,2579,2635,596,9386,9702,
	8997,170,186,8735,2322,2378,248,511,12361,12457,65387,511,63216,1151,245,7757,
	7759,12577,8254,65098,773,65097,65100,65099,175,2507,2379,2763,112,13184,
	13099,2474,7765,2346,8671,8670,2730,2602,12401,3631,12497,1156,1216,12671,182,
	8741,40,64830,63725,63724,8333,65288,65113,8317,63723,65077,41,64831,63736,
	63735,8334,65289,65114,8318,63734,65078,8706,1472,1433,13225,1463,1463,1463,
	1463,1463,1463,1463,1463,1441,12550,9439,7767,1508,1087,64324,64324,13115,
	64323,1662,1402,1508,64343,64344,12410,64345,12506,1191,64334,37,1642,65285,
	65130,46,1417,183,65377,63207,65294,65106,63208,834,8869,8240,8359,13194,2475,
	2347,2731,2603,966,981,12922,12826,12908,12621,12812,632,3642,981,421,3614,
	3612,3616,960,12915,12819,12662,12901,12658,12610,12805,12660,12612,12661,
	12663,12659,12404,12500,982,1411,43,799,8853,177,726,65291,65122,8314,65360,
	13272,12413,9759,9756,9758,9757,12509,3611,12306,12320,9387,8826,8478,697,
	8245,8719,8965,12540,8984,8834,8835,8759,8733,968,1137,1158,13232,12407,12503,
	13236,13242,113,2392,1448,1602,65238,65239,65240,1464,1464,1464,1464,1464,
	1464,1464,1464,1464,1464,1464,1464,1464,1464,1464,1464,1439,12561,9440,672,
	65361,1511,64327,64327,1511,1511,1511,1511,1511,1511,1511,1511,1511,1511,1511,
	1511,1511,1511,1511,1511,1511,1511,1511,1511,1511,9388,9833,1467,1467,1467,
	1467,1467,1467,1467,1467,63,1567,1374,191,63423,894,65311,63295,34,8222,8220,
	65282,12318,12317,8221,8216,8219,8219,8217,329,8218,39,65287,114,1404,2480,
	341,2352,8730,63717,13230,13231,13229,1471,1471,2736,2608,12425,12521,65431,
	2545,2544,612,8758,12566,345,343,9441,343,529,7769,7771,7773,8251,8838,8839,
	174,63720,63194,1585,1408,65198,12428,1585,12524,65434,1512,64328,1512,1512,
	1512,1512,1512,1512,1512,1512,1512,1512,1512,1512,1512,1512,1512,1512,1512,
	1512,1512,1512,1512,8765,1431,1431,8976,638,639,2525,2397,961,637,635,693,
	1009,734,12913,12817,12899,12608,12602,12649,12601,12603,12652,12803,12607,
	12604,12651,12605,12606,12650,12653,8735,793,8895,12426,12522,65432,730,805,
	778,703,1369,796,723,702,825,722,531,13137,7775,636,634,65362,12429,12525,
	65435,3619,9389,2524,2353,2652,1681,64397,2528,2400,2784,2500,2372,2756,63217,
	9616,633,692,12427,12523,65433,2546,2547,63197,3620,2443,2315,2699,2499,2371,
	2755,115,2488,347,7781,1589,2360,65210,65211,65212,2744,2616,12373,12469,
	65403,65018,1505,64321,64321,1505,3634,3649,3652,3651,3635,3632,3648,63622,
	3637,63621,3636,3650,63624,3639,63623,3638,3640,3641,12569,353,7783,351,601,
	1241,1243,602,9442,349,537,7777,7779,7785,828,8243,714,167,1587,65202,65203,
	65204,1462,1462,1462,1462,1462,1462,1462,1426,1462,1405,12379,12475,65406,59,
	1563,65307,65108,12444,65439,13090,13091,55,1639,2541,9318,10128,2413,8542,
	2797,2669,1639,12327,12838,8327,65303,63287,9338,9358,1783,8566,8311,9328,
	9348,9368,3671,173,1399,2486,1096,1617,64609,64606,64608,1617,64610,64607,
	9618,9619,9617,9618,2358,2742,2614,1427,12565,1097,1588,65206,65207,65208,995,
	8362,8362,1456,1456,1456,1456,1456,1456,1456,1456,1456,1211,1005,1513,64329,
	64329,64300,64300,64301,64301,1473,1513,64298,64298,64299,64299,642,963,962,
	962,1010,12375,12471,65404,1469,1469,8764,1474,12916,12820,12670,12902,12666,
	12613,12667,12806,12669,12668,54,1638,2540,9317,10127,2412,2796,2668,1638,
	12326,12837,8326,65302,63286,9337,9357,1782,8565,8310,9327,2553,9347,9367,
	3670,47,65295,383,7835,9786,65363,1475,173,1100,12381,12477,65407,824,823,
	3625,3624,3595,3626,32,32,9824,9824,9828,9390,827,13252,13213,9641,9636,13199,
	13214,13262,13265,13266,13198,13269,13212,13217,9638,9639,9640,9637,9635,
	13275,2487,2359,2743,12617,12677,12672,12594,12645,12611,12614,12600,63218,
	163,65505,822,821,8834,8842,8838,8827,8715,12377,12473,65405,1618,8721,9788,
	8835,8843,8839,13276,13180,116,2468,8868,8867,2340,2724,2596,1591,65218,65219,
	12383,65220,13181,12479,65408,1600,964,1514,64330,64330,64330,1514,359,12554,
	357,680,355,1670,64379,64380,64381,64380,9443,7793,355,7831,7787,7789,1090,
	1197,1578,65174,64674,64524,65175,12390,64673,64523,1577,65172,65176,64676,
	64526,64627,12486,65411,8481,9742,1440,1449,9321,12841,9341,9361,8569,679,
	1496,64312,64312,1496,1205,1435,1435,2469,2341,2725,2597,1584,65196,63640,
	63639,3660,63638,1579,65178,65179,65180,8707,8756,952,977,977,12921,12825,
	12907,12620,12811,9324,9344,9364,3601,429,3602,254,3607,3600,3608,3606,1154,
	1644,1644,51,1635,2537,9314,10124,2409,8540,2793,2665,1635,12323,12834,8323,
	65299,2550,63283,9334,9354,1779,190,63198,8562,179,3667,13204,12385,12481,
	65409,12912,12816,12898,12599,12802,732,816,771,771,864,8764,820,830,8855,
	1430,1430,2672,1155,1407,7791,65364,1385,12392,12488,65412,741,745,742,744,
	743,445,389,424,900,13095,3599,12308,65117,65081,12309,65118,65082,3605,427,
	9391,8482,63722,63195,648,9660,9668,9658,9650,678,1510,64326,64326,1510,1094,
	1461,1461,1461,1461,1461,1461,1461,1461,1115,63219,2463,2335,2719,2591,1657,
	64359,64360,64361,2464,2336,2720,2592,647,12388,12484,65410,12387,12483,65391,
	9323,9343,9363,8571,9331,21316,9351,9371,50,1634,2536,9313,10123,2408,8229,
	8229,65072,2792,2664,1634,12322,12833,8322,65298,2549,63282,9333,9353,1778,
	8561,443,178,3666,8532,117,250,649,2441,12584,365,468,9444,251,7799,1091,2385,
	369,533,2313,252,472,7795,474,1265,476,470,7909,249,2697,2569,12358,7911,432,
	7913,7921,7915,7917,7919,369,1267,535,12454,65395,1145,12636,363,1263,7803,
	2625,65365,95,8215,65343,65075,65103,8746,8704,371,9392,9600,1476,965,971,944,
	650,973,797,724,2675,367,1118,12357,12453,65385,1199,1201,361,7801,7797,2442,
	2314,2698,2570,2626,2498,2370,2754,2497,2369,2753,118,2357,2741,2613,12535,
	1493,64309,64309,64309,1493,64331,64331,1520,1521,9445,7807,1074,1700,64363,
	64364,64365,12537,9792,124,781,809,716,712,1406,651,12536,2509,2381,2765,2435,
	2307,2691,65366,1400,12446,12542,12443,65438,12538,9393,7805,652,12436,12532,
	119,7811,12633,12431,12527,65436,12632,12430,12526,13143,12316,65076,1608,
	65262,1572,65158,13277,9446,373,7813,7815,7817,12433,8472,12529,12638,12637,
	7809,9702,9675,9689,12302,65091,12303,65092,9671,9672,9663,9661,9667,9665,
	12310,12311,9657,9655,9643,9786,9633,9734,9743,12312,12313,9653,9651,12432,
	12528,12639,65367,12434,12530,65382,8361,65510,3623,9394,7832,695,653,447,120,
	829,12562,9447,7821,7819,1389,958,65368,9395,739,121,13134,2479,253,2351,
	12626,2735,2607,12420,12516,65428,12625,3662,12419,12515,65388,1123,9448,375,
	255,7823,7925,1610,1746,64431,65266,1574,65162,65163,65164,65267,65268,64733,
	64600,64660,1745,12630,165,65509,12629,12678,1450,1450,1099,1273,12673,12675,
	12674,1434,7923,436,7927,1397,1111,12642,9775,1410,65369,1497,64313,64313,
	1497,1522,64287,12424,12681,12520,65430,12635,12423,12519,65390,1011,12680,
	12679,3618,3597,9396,890,837,422,7833,696,7929,654,12422,12684,12518,65429,
	12640,1131,1133,1127,1129,12421,12517,65389,12683,12682,2527,2399,122,1382,
	378,2395,2651,1592,65222,65223,12374,65224,1586,65200,12470,1429,1428,1432,
	1494,64310,64310,1494,12567,382,9449,7825,657,380,380,7827,1079,1177,1247,
	12380,12476,48,1632,2534,2406,2790,2662,1632,8320,65296,63280,1776,8304,3664,
	65279,8204,8203,950,12563,1386,1218,1078,1175,1245,12376,12472,1454,7829,
	65370,12382,12478,9397,656,438,12378,12474,
};

static const unsigned short agl_dup_offsets[] = {
	32,0,124,3,160,6,173,9,175,12,181,15,183,18,266,21,267,24,272,27,273,30,
	278,33,279,36,288,39,289,42,290,45,291,48,304,51,310,54,311,57,315,60,316,63,
	319,66,320,69,325,72,326,75,329,78,336,81,337,84,342,87,343,90,354,93,355,96,
	368,99,369,102,379,105,380,108,383,111,510,114,511,117,700,120,701,123,
	732,126,768,129,769,132,771,135,777,138,803,141,901,144,962,147,977,150,
	978,153,981,156,982,159,1025,162,1026,165,1027,168,1028,171,1029,174,1030,177,
	1031,180,1032,183,1033,186,1034,189,1035,192,1036,195,1038,198,1039,201,
	1040,204,1041,207,1042,210,1043,213,1044,216,1045,219,1046,222,1047,225,
	1048,228,1049,231,1050,234,1051,237,1052,240,1053,243,1054,246,1055,249,
	1056,252,1057,255,1058,258,1059,261,1060,264,1061,267,1062,270,1063,273,
	1064,276,1065,279,1066,282,1067,285,1068,288,1069,291,1070,294,1071,297,
	1072,300,1073,303,1074,306,1075,309,1076,312,1077,315,1078,318,1079,321,
	1080,324,1081,327,1082,330,1083,333,1084,336,1085,339,1086,342,1087,345,
	1088,348,1089,351,1090,354,1091,357,1092,360,1093,363,1094,366,1095,369,
	1096,372,1097,375,1098,378,1099,381,1100,384,1101,387,1102,390,1103,393,
	1105,396,1106,399,1107,402,1108,405,1109,408,1110,411,1111,414,1112,417,
	1113,420,1114,423,1115,426,1116,429,1118,432,1119,435,1122,438,1123,441,
	1138,444,1139,447,1140,450,1141,453,1168,456,1169,459,1241,462,1425,465,
	1430,470,1431,473,1435,476,1443,479,1444,482,1445,485,1446,488,1447,491,
	1450,494,1456,497,1457,508,1458,518,1459,528,1460,538,1461,548,1462,558,
	1463,568,1464,578,1465,596,1467,606,1468,616,1469,620,1470,624,1471,627,
	1472,631,1473,634,1474,637,1475,640,1488,643,1489,647,1490,651,1491,655,
	1492,679,1493,683,1494,687,1495,691,1496,695,1497,699,1498,703,1499,711,
	1500,715,1501,723,1502,727,1503,731,1504,735,1505,739,1506,743,1507,747,
	1508,751,1509,755,1510,759,1511,763,1512,787,1513,811,1514,815,1520,819,
	1521,822,1522,825,1548,828,1563,831,1567,834,1569,837,1570,848,1571,851,
	1572,854,1573,857,1574,860,1575,863,1576,866,1577,869,1578,872,1579,875,
	1580,878,1581,881,1582,884,1583,887,1584,890,1585,893,1586,897,1587,900,
	1588,903,1589,906,1590,909,1591,912,1592,915,1593,918,1594,921,1600,924,
	1601,929,1602,932,1603,935,1604,938,1605,941,1606,944,1607,947,1608,950,
	1609,953,1610,956,1611,959,1612,962,1613,966,1614,969,1615,973,1616,977,
	1617,980,1618,984,1632,987,1633,991,1634,995,1635,999,1636,1003,1637,1007,
	1638,1011,1639,1015,1640,1019,1641,1023,1642,1027,1643,1030,1644,1033,
	1645,1036,1657,1040,1662,1043,1670,1046,1672,1049,1681,1052,1688,1055,
	1700,1058,1711,1061,1722,1064,1729,1067,1746,1070,8204,1073,8213,1076,
	8215,1079,8219,1082,8229,1085,8353,1088,8356,1091,8362,1094,8364,1099,
	8453,1102,8467,1105,8470,1108,8486,1111,8616,1114,8656,1117,8658,1120,
	8660,1123,8704,1126,8707,1129,8710,1132,8711,1135,8713,1138,8735,1141,
	8764,1144,8773,1147,8834,1150,8835,1153,8838,1156,8839,1159,8853,1162,
	8855,1165,8976,1168,8992,1171,8993,1174,9617,1177,9618,1180,9619,1183,
	9632,1186,9633,1189,9642,1192,9643,1195,9644,1198,9650,1201,9658,1204,
	9660,1207,9668,1210,9675,1213,9679,1216,9688,1219,9689,1222,9702,1225,
	9786,1228,9787,1231,9788,1234,9792,1237,9794,1240,9824,1243,9827,1246,
	9829,1249,9835,1252,64287,1255,64298,1260,64299,1264,64300,1268,64301,1271,
	64305,1274,64306,1277,64307,1280,64308,1283,64309,1286,64310,1291,64312,1294,
	64313,1297,64314,1300,64315,1303,64316,1306,64318,1309,64320,1312,64321,1315,
	64324,1318,64326,1321,64327,1324,64329,1327,64330,1330,64331,1334,64380,1338,
	65247,1341,65255,1345,65258,1348,65267,1351,65268,1354,
};

static const char *agl_dup_names[] = {
	"space","spacehackarabic",0,"bar","verticalbar",0,"nbspace",
	"nonbreakingspace",0,"sfthyphen","softhyphen",0,"macron","overscore",0,"mu",
	"mu1",0,"middot","periodcentered",0,"Cdot","Cdotaccent",0,"cdot","cdotaccent",
	0,"Dcroat","Dslash",0,"dcroat","dmacron",0,"Edot","Edotaccent",0,"edot",
	"edotaccent",0,"Gdot","Gdotaccent",0,"gdot","gdotaccent",0,"Gcedilla",
	"Gcommaaccent",0,"gcedilla","gcommaaccent",0,"Idot","Idotaccent",0,"Kcedilla",
	"Kcommaaccent",0,"kcedilla","kcommaaccent",0,"Lcedilla","Lcommaaccent",0,
	"lcedilla","lcommaaccent",0,"Ldot","Ldotaccent",0,"ldot","ldotaccent",0,
	"Ncedilla","Ncommaaccent",0,"ncedilla","ncommaaccent",0,"napostrophe",
	"quoterightn",0,"Odblacute","Ohungarumlaut",0,"odblacute","ohungarumlaut",0,
	"Rcedilla","Rcommaaccent",0,"rcedilla","rcommaaccent",0,"Tcedilla",
	"Tcommaaccent",0,"tcedilla","tcommaaccent",0,"Udblacute","Uhungarumlaut",0,
	"udblacute","uhungarumlaut",0,"Zdot","Zdotaccent",0,"zdot","zdotaccent",0,
	"longs","slong",0,"Oslashacute","Ostrokeacute",0,"oslashacute","ostrokeacute",
	0,"afii57929","apostrophemod",0,"afii64937","commareversedmod",0,"ilde",
	"tilde",0,"gravecmb","gravecomb",0,"acutecmb","acutecomb",0,"tildecmb",
	"tildecomb",0,"hookabovecomb","hookcmb",0,"dotbelowcmb","dotbelowcomb",0,
	"dialytikatonos","dieresistonos",0,"sigma1","sigmafinal",0,"theta1",
	"thetasymbolgreek",0,"Upsilon1","Upsilonhooksymbol",0,"phi1","phisymbolgreek",
	0,"omega1","pisymbolgreek",0,"Iocyrillic","afii10023",0,"Djecyrillic",
	"afii10051",0,"Gjecyrillic","afii10052",0,"Ecyrillic","afii10053",0,
	"Dzecyrillic","afii10054",0,"Icyrillic","afii10055",0,"Yicyrillic",
	"afii10056",0,"Jecyrillic","afii10057",0,"Ljecyrillic","afii10058",0,
	"Njecyrillic","afii10059",0,"Tshecyrillic","afii10060",0,"Kjecyrillic",
	"afii10061",0,"Ushortcyrillic","afii10062",0,"Dzhecyrillic","afii10145",0,
	"Acyrillic","afii10017",0,"Becyrillic","afii10018",0,"Vecyrillic","afii10019",
	0,"Gecyrillic","afii10020",0,"Decyrillic","afii10021",0,"Iecyrillic",
	"afii10022",0,"Zhecyrillic","afii10024",0,"Zecyrillic","afii10025",0,
	"Iicyrillic","afii10026",0,"Iishortcyrillic","afii10027",0,"Kacyrillic",
	"afii10028",0,"Elcyrillic","afii10029",0,"Emcyrillic","afii10030",0,
	"Encyrillic","afii10031",0,"Ocyrillic","afii10032",0,"Pecyrillic","afii10033",
	0,"Ercyrillic","afii10034",0,"Escyrillic","afii10035",0,"Tecyrillic",
	"afii10036",0,"Ucyrillic","afii10037",0,"Efcyrillic","afii10038",0,
	"Khacyrillic","afii10039",0,"Tsecyrillic","afii10040",0,"Checyrillic",
	"afii10041",0,"Shacyrillic","afii10042",0,"Shchacyrillic","afii10043",0,
	"Hardsigncyrillic","afii10044",0,"Yericyrillic","afii10045",0,
	"Softsigncyrillic","afii10046",0,"Ereversedcyrillic","afii10047",0,
	"IUcyrillic","afii10048",0,"IAcyrillic","afii10049",0,"acyrillic","afii10065",
	0,"afii10066","becyrillic",0,"afii10067","vecyrillic",0,"afii10068",
	"gecyrillic",0,"afii10069","decyrillic",0,"afii10070","iecyrillic",0,
	"afii10072","zhecyrillic",0,"afii10073","zecyrillic",0,"afii10074",
	"iicyrillic",0,"afii10075","iishortcyrillic",0,"afii10076","kacyrillic",0,
	"afii10077","elcyrillic",0,"afii10078","emcyrillic",0,"afii10079",
	"encyrillic",0,"afii10080","ocyrillic",0,"afii10081","pecyrillic",0,
	"afii10082","ercyrillic",0,"afii10083","escyrillic",0,"afii10084",
	"tecyrillic",0,"afii10085","ucyrillic",0,"afii10086","efcyrillic",0,
	"afii10087","khacyrillic",0,"afii10088","tsecyrillic",0,"afii10089",
	"checyrillic",0,"afii10090","shacyrillic",0,"afii10091","shchacyrillic",0,
	"afii10092","hardsigncyrillic",0,"afii10093","yericyrillic",0,"afii10094",
	"softsigncyrillic",0,"afii10095","ereversedcyrillic",0,"afii10096",
	"iucyrillic",0,"afii10097","iacyrillic",0,"afii10071","iocyrillic",0,
	"afii10099","djecyrillic",0,"afii10100","gjecyrillic",0,"afii10101",
	"ecyrillic",0,"afii10102","dzecyrillic",0,"afii10103","icyrillic",0,
	"afii10104","yicyrillic",0,"afii10105","jecyrillic",0,"afii10106",
	"ljecyrillic",0,"afii10107","njecyrillic",0,"afii10108","tshecyrillic",0,
	"afii10109","kjecyrillic",0,"afii10110","ushortcyrillic",0,"afii10193",
	"dzhecyrillic",0,"Yatcyrillic","afii10146",0,"afii10194","yatcyrillic",0,
	"Fitacyrillic","afii10147",0,"afii10195","fitacyrillic",0,"Izhitsacyrillic",
	"afii10148",0,"afii10196","izhitsacyrillic",0,"Gheupturncyrillic","afii10050",
	0,"afii10098","gheupturncyrillic",0,"afii10846","schwacyrillic",0,
	"etnahtafoukhhebrew","etnahtafoukhlefthebrew","etnahtahebrew",
	"etnahtalefthebrew",0,"tipehahebrew","tipehalefthebrew",0,"reviahebrew",
	"reviamugrashhebrew",0,"tevirhebrew","tevirlefthebrew",0,"munahhebrew",
	"munahlefthebrew",0,"mahapakhhebrew","mahapakhlefthebrew",0,"merkhahebrew",
	"merkhalefthebrew",0,"merkhakefulahebrew","merkhakefulalefthebrew",0,
	"dargahebrew","dargalefthebrew",0,"yerahbenyomohebrew",
	"yerahbenyomolefthebrew",0,"afii57799","sheva","sheva115","sheva15","sheva22",
	"sheva2e","shevahebrew","shevanarrowhebrew","shevaquarterhebrew",
	"shevawidehebrew",0,"afii57801","hatafsegol","hatafsegol17","hatafsegol24",
	"hatafsegol30","hatafsegolhebrew","hatafsegolnarrowhebrew",
	"hatafsegolquarterhebrew","hatafsegolwidehebrew",0,"afii57800","hatafpatah",
	"hatafpatah16","hatafpatah23","hatafpatah2f","hatafpatahhebrew",
	"hatafpatahnarrowhebrew","hatafpatahquarterhebrew","hatafpatahwidehebrew",0,
	"afii57802","hatafqamats","hatafqamats1b","hatafqamats28","hatafqamats34",
	"hatafqamatshebrew","hatafqamatsnarrowhebrew","hatafqamatsquarterhebrew",
	"hatafqamatswidehebrew",0,"afii57793","hiriq","hiriq14","hiriq21","hiriq2d",
	"hiriqhebrew","hiriqnarrowhebrew","hiriqquarterhebrew","hiriqwidehebrew",0,
	"afii57794","tsere","tsere12","tsere1e","tsere2b","tserehebrew",
	"tserenarrowhebrew","tserequarterhebrew","tserewidehebrew",0,"afii57795",
	"segol","segol13","segol1f","segol2c","segolhebrew","segolnarrowhebrew",
	"segolquarterhebrew","segolwidehebrew",0,"afii57798","patah","patah11",
	"patah1d","patah2a","patahhebrew","patahnarrowhebrew","patahquarterhebrew",
	"patahwidehebrew",0,"afii57797","qamats","qamats10","qamats1a","qamats1c",
	"qamats27","qamats29","qamats33","qamatsde","qamatshebrew",
	"qamatsnarrowhebrew","qamatsqatanhebrew","qamatsqatannarrowhebrew",
	"qamatsqatanquarterhebrew","qamatsqatanwidehebrew","qamatsquarterhebrew",
	"qamatswidehebrew",0,"afii57806","holam","holam19","holam26","holam32",
	"holamhebrew","holamnarrowhebrew","holamquarterhebrew","holamwidehebrew",0,
	"afii57796","qubuts","qubuts18","qubuts25","qubuts31","qubutshebrew",
	"qubutsnarrowhebrew","qubutsquarterhebrew","qubutswidehebrew",0,"afii57807",
	"dagesh","dageshhebrew",0,"afii57839","siluqhebrew","siluqlefthebrew",0,
	"afii57645","maqafhebrew",0,"afii57841","rafe","rafehebrew",0,"afii57842",
	"paseqhebrew",0,"afii57804","shindothebrew",0,"afii57803","sindothebrew",0,
	"afii57658","sofpasuqhebrew",0,"afii57664","alef","alefhebrew",0,"afii57665",
	"bet","bethebrew",0,"afii57666","gimel","gimelhebrew",0,"afii57667","dalet",
	"dalethatafpatah","dalethatafpatahhebrew","dalethatafsegol",
	"dalethatafsegolhebrew","dalethebrew","dalethiriq","dalethiriqhebrew",
	"daletholam","daletholamhebrew","daletpatah","daletpatahhebrew","daletqamats",
	"daletqamatshebrew","daletqubuts","daletqubutshebrew","daletsegol",
	"daletsegolhebrew","daletsheva","daletshevahebrew","dalettsere",
	"dalettserehebrew",0,"afii57668","he","hehebrew",0,"afii57669","vav",
	"vavhebrew",0,"afii57670","zayin","zayinhebrew",0,"afii57671","het",
	"hethebrew",0,"afii57672","tet","tethebrew",0,"afii57673","yod","yodhebrew",0,
	"afii57674","finalkaf","finalkafhebrew","finalkafqamats",
	"finalkafqamatshebrew","finalkafsheva","finalkafshevahebrew",0,"afii57675",
	"kaf","kafhebrew",0,"afii57676","lamed","lamedhebrew","lamedholam",
	"lamedholamdagesh","lamedholamdageshhebrew","lamedholamhebrew",0,"afii57677",
	"finalmem","finalmemhebrew",0,"afii57678","mem","memhebrew",0,"afii57679",
	"finalnun","finalnunhebrew",0,"afii57680","nun","nunhebrew",0,"afii57681",
	"samekh","samekhhebrew",0,"afii57682","ayin","ayinhebrew",0,"afii57683",
	"finalpe","finalpehebrew",0,"afii57684","pe","pehebrew",0,"afii57685",
	"finaltsadi","finaltsadihebrew",0,"afii57686","tsadi","tsadihebrew",0,
	"afii57687","qof","qofhatafpatah","qofhatafpatahhebrew","qofhatafsegol",
	"qofhatafsegolhebrew","qofhebrew","qofhiriq","qofhiriqhebrew","qofholam",
	"qofholamhebrew","qofpatah","qofpatahhebrew","qofqamats","qofqamatshebrew",
	"qofqubuts","qofqubutshebrew","qofsegol","qofsegolhebrew","qofsheva",
	"qofshevahebrew","qoftsere","qoftserehebrew",0,"afii57688","resh",
	"reshhatafpatah","reshhatafpatahhebrew","reshhatafsegol",
	"reshhatafsegolhebrew","reshhebrew","reshhiriq","reshhiriqhebrew","reshholam",
	"reshholamhebrew","reshpatah","reshpatahhebrew","reshqamats",
	"reshqamatshebrew","reshqubuts","reshqubutshebrew","reshsegol",
	"reshsegolhebrew","reshsheva","reshshevahebrew","reshtsere","reshtserehebrew",
	0,"afii57689","shin","shinhebrew",0,"afii57690","tav","tavhebrew",0,
	"afii57716","vavvavhebrew",0,"afii57717","vavyodhebrew",0,"afii57718",
	"yodyodhebrew",0,"afii57388","commaarabic",0,"afii57403","semicolonarabic",0,
	"afii57407","questionarabic",0,"afii57409","hamzaarabic","hamzadammaarabic",
	"hamzadammatanarabic","hamzafathaarabic","hamzafathatanarabic",
	"hamzalowarabic","hamzalowkasraarabic","hamzalowkasratanarabic",
	"hamzasukunarabic",0,"afii57410","alefmaddaabovearabic",0,"afii57411",
	"alefhamzaabovearabic",0,"afii57412","wawhamzaabovearabic",0,"afii57413",
	"alefhamzabelowarabic",0,"afii57414","yehhamzaabovearabic",0,"afii57415",
	"alefarabic",0,"afii57416","beharabic",0,"afii57417","tehmarbutaarabic",0,
	"afii57418","teharabic",0,"afii57419","theharabic",0,"afii57420","jeemarabic",
	0,"afii57421","haharabic",0,"afii57422","khaharabic",0,"afii57423",
	"dalarabic",0,"afii57424","thalarabic",0,"afii57425","reharabic",
	"rehyehaleflamarabic",0,"afii57426","zainarabic",0,"afii57427","seenarabic",0,
	"afii57428","sheenarabic",0,"afii57429","sadarabic",0,"afii57430","dadarabic",
	0,"afii57431","taharabic",0,"afii57432","zaharabic",0,"afii57433","ainarabic",
	0,"afii57434","ghainarabic",0,"afii57440","kashidaautoarabic",
	"kashidaautonosidebearingarabic","tatweelarabic",0,"afii57441","feharabic",0,
	"afii57442","qafarabic",0,"afii57443","kafarabic",0,"afii57444","lamarabic",0,
	"afii57445","meemarabic",0,"afii57446","noonarabic",0,"afii57470","heharabic",
	0,"afii57448","wawarabic",0,"afii57449","alefmaksuraarabic",0,"afii57450",
	"yeharabic",0,"afii57451","fathatanarabic",0,"afii57452",
	"dammatanaltonearabic","dammatanarabic",0,"afii57453","kasratanarabic",0,
	"afii57454","fathaarabic","fathalowarabic",0,"afii57455","dammaarabic",
	"dammalowarabic",0,"afii57456","kasraarabic",0,"afii57457","shaddaarabic",
	"shaddafathatanarabic",0,"afii57458","sukunarabic",0,"afii57392","zeroarabic",
	"zerohackarabic",0,"afii57393","onearabic","onehackarabic",0,"afii57394",
	"twoarabic","twohackarabic",0,"afii57395","threearabic","threehackarabic",0,
	"afii57396","fourarabic","fourhackarabic",0,"afii57397","fivearabic",
	"fivehackarabic",0,"afii57398","sixarabic","sixhackarabic",0,"afii57399",
	"sevenarabic","sevenhackarabic",0,"afii57400","eightarabic","eighthackarabic",
	0,"afii57401","ninearabic","ninehackarabic",0,"afii57381","percentarabic",0,
	"decimalseparatorarabic","decimalseparatorpersian",0,
	"thousandsseparatorarabic","thousandsseparatorpersian",0,"afii63167",
	"asteriskaltonearabic","asteriskarabic",0,"afii57511","tteharabic",0,
	"afii57506","peharabic",0,"afii57507","tcheharabic",0,"afii57512",
	"ddalarabic",0,"afii57513","rreharabic",0,"afii57508","jeharabic",0,
	"afii57505","veharabic",0,"afii57509","gafarabic",0,"afii57514",
	"noonghunnaarabic",0,"haaltonearabic","hehaltonearabic",0,"afii57519",
	"yehbarreearabic",0,"afii61664","zerowidthnonjoiner",0,"afii00208",
	"horizontalbar",0,"dbllowline","underscoredbl",0,"quoteleftreversed",
	"quotereversed",0,"twodotenleader","twodotleader",0,"colonmonetary",
	"colonsign",0,"afii08941","lira",0,"afii57636","newsheqelsign","sheqel",
	"sheqelhebrew",0,"Euro","euro",0,"afii61248","careof",0,"afii61289","lsquare",
	0,"afii61352","numero",0,"Ohm","Omega",0,"arrowupdnbse","arrowupdownbase",0,
	"arrowdblleft","arrowleftdbl",0,"arrowdblright","dblarrowright",0,
	"arrowdblboth","dblarrowleft",0,"forall","universal",0,"existential",
	"thereexists",0,"Delta","increment",0,"gradient","nabla",0,"notelement",
	"notelementof",0,"orthogonal","rightangle",0,"similar","tildeoperator",0,
	"approximatelyequal","congruent",0,"propersubset","subset",0,"propersuperset",
	"superset",0,"reflexsubset","subsetorequal",0,"reflexsuperset",
	"supersetorequal",0,"circleplus","pluscircle",0,"circlemultiply",
	"timescircle",0,"logicalnotreversed","revlogicalnot",0,"integraltop",
	"integraltp",0,"integralbottom","integralbt",0,"ltshade","shadelight",0,
	"shade","shademedium",0,"dkshade","shadedark",0,"blacksquare","filledbox",0,
	"H22073","whitesquare",0,"H18543","blacksmallsquare",0,"H18551",
	"whitesmallsquare",0,"blackrectangle","filledrect",0,
	"blackuppointingtriangle","triagup",0,"blackrightpointingpointer","triagrt",0,
	"blackdownpointingtriangle","triagdn",0,"blackleftpointingpointer","triaglf",
	0,"circle","whitecircle",0,"H18533","blackcircle",0,"bulletinverse",
	"invbullet",0,"invcircle","whitecircleinverse",0,"openbullet","whitebullet",0,
	"smileface","whitesmilingface",0,"blacksmilingface","invsmileface",0,
	"compass","sun",0,"female","venus",0,"male","mars",0,"spade","spadesuitblack",
	0,"club","clubsuitblack",0,"heart","heartsuitblack",0,"eighthnotebeamed",
	"musicalnotedbl",0,"afii57705","doubleyodpatah","doubleyodpatahhebrew",
	"yodyodpatahhebrew",0,"afii57694","shinshindot","shinshindothebrew",0,
	"afii57695","shinsindot","shinsindothebrew",0,"shindageshshindot",
	"shindageshshindothebrew",0,"shindageshsindot","shindageshsindothebrew",0,
	"betdagesh","betdageshhebrew",0,"gimeldagesh","gimeldageshhebrew",0,
	"daletdagesh","daletdageshhebrew",0,"hedagesh","hedageshhebrew",0,"afii57723",
	"vavdagesh","vavdagesh65","vavdageshhebrew",0,"zayindagesh",
	"zayindageshhebrew",0,"tetdagesh","tetdageshhebrew",0,"yoddagesh",
	"yoddageshhebrew",0,"finalkafdagesh","finalkafdageshhebrew",0,"kafdagesh",
	"kafdageshhebrew",0,"lameddagesh","lameddageshhebrew",0,"memdagesh",
	"memdageshhebrew",0,"nundagesh","nundageshhebrew",0,"samekhdagesh",
	"samekhdageshhebrew",0,"pedagesh","pedageshhebrew",0,"tsadidagesh",
	"tsadidageshhebrew",0,"qofdagesh","qofdageshhebrew",0,"shindagesh",
	"shindageshhebrew",0,"tavdages","tavdagesh","tavdageshhebrew",0,"afii57700",
	"vavholam","vavholamhebrew",0,"tchehinitialarabic","tchehmeeminitialarabic",0,
	"laminitialarabic","lammeemjeeminitialarabic","lammeemkhahinitialarabic",0,
	"noonhehinitialarabic","nooninitialarabic",0,"hehfinalalttwoarabic",
	"hehfinalarabic",0,"alefmaksurainitialarabic","yehinitialarabic",0,
	"alefmaksuramedialarabic","yehmedialarabic",0,
};

#define nelem(x) (sizeof(x)/sizeof((x)[0]))

int
	fz_strlcpy(char *dst, const char *src, int siz)
{
	register char *d = dst;
	register const char *s = src;
	register int n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0 && --n != 0) {
		do {
			if ((*d++ = *s++) == 0)
				break;
		} while (--n != 0);
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */
		while (*s++)
			;
	}

	return(s - src - 1);	/* count does not include NUL */
}

int pdf_lookup_agl(char *name)
{
	char buf[64];
	char *p;
	int l = 0;
	int r = nelem(agl_name_list) - 1;

	fz_strlcpy(buf, name, sizeof buf);

	/* kill anything after first period and underscore */
	p = strchr(buf, '.');
	if (p) p[0] = 0;
	p = strchr(buf, '_');
	if (p) p[0] = 0;

	while (l <= r)
	{
		int m = (l + r) >> 1;
		int c = strcmp(buf, agl_name_list[m]);
		if (c < 0)
			r = m - 1;
		else if (c > 0)
			l = m + 1;
		else
			return agl_code_list[m];
	}

	if (strstr(buf, "uni") == buf)
		return strtol(buf + 3, NULL, 16);
	else if (strstr(buf, "u") == buf)
		return strtol(buf + 1, NULL, 16);
	else if (strstr(buf, "a") == buf && strlen(buf) >= 3)
		return strtol(buf + 1, NULL, 10);

	return 0;
}

static const char *empty_dup_list[] = { 0 };

const char **
	pdf_lookup_agl_duplicates(int ucs)
{
	int l = 0;
	int r = nelem(agl_dup_offsets) / 2 - 1;
	while (l <= r)
	{
		int m = (l + r) >> 1;
		if (ucs < agl_dup_offsets[m << 1])
			r = m - 1;
		else if (ucs > agl_dup_offsets[m << 1])
			l = m + 1;
		else
			return agl_dup_names + agl_dup_offsets[(m << 1) + 1];
	}
	return empty_dup_list;
}

const HE_WCHAR	gPdfDocEncoding[256] = {
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009,
	0x000A, 0x000B,	0x000C, 0x000D,	0x000E, 0x000F, 0x0010, 0x0011, 0x0012, 0x0013,
	0x0014, 0x0015, 0x0017, 0x0017, 0x02D8, 0x02C7, 0x02C6, 0x02D9, 0x02DD, 0x02DB,
	0x02DA, 0x02DC,	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
	0x0028, 0x0029, 0x002A,	0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031,
	0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B,
	0x003C, 0x003D, 0x003E, 0x003F, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045,
	0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059,
	0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 0x0060, 0x0061, 0x0062, 0x0063,
	0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D,
	0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
	0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x0000, 0x2022, 0x2020,
	0x2021, 0x2026, 0x2014, 0x2013, 0x0192, 0x2044, 0x2039, 0x203A, 0x2212, 0x2030,
	0x201E, 0x201C, 0x201D, 0x2018, 0x2019, 0x201A, 0x2122, 0xFB01, 0xFB02, 0x0141,
	0x0152, 0x0160, 0x0178, 0x017D, 0x0131, 0x0142, 0x0153, 0x0161, 0x017E, 0x0000,
	0x20AC, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9,
	0x00AA, 0x00AB, 0x00AC, 0x0000, 0x00AE, 0x00AF, 0x00B0, 0x00B1, 0x00B2, 0x00B3,
	0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD,
	0x00BE, 0x00BF, 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x00D0, 0x00D1,
	0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB,
	0x00DC, 0x00DD, 0x00DE, 0x00DF, 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5,
	0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9,
	0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF };
const HE_WCHAR	gWinAnsiEncoding[256] = {	
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009,
	0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011, 0x0012, 0x0013,
	0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D,
	0x001E, 0x001F, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
	0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031,
	0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B,
	0x003C, 0x003D, 0x003E, 0x003F, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045,
	0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059,
	0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 0x0060, 0x0061, 0x0062, 0x0063,
	0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D,
	0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
	0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F, 0x20AC, 0x0000,
	0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6, 0x2030, 0x0160, 0x2039,
	0x0152, 0x0000, 0x017D, 0x0000, 0x0000, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022,
	0x2013, 0x2014, 0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x0000, 0x017E, 0x0178,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9,
	0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF, 0x00B0, 0x00B1, 0x00B2, 0x00B3,
	0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD,
	0x00BE, 0x00BF, 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x00D0, 0x00D1,
	0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB,
	0x00DC, 0x00DD, 0x00DE, 0x00DF, 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5,
	0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9,
	0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF };
const HE_WCHAR gMacRomanEncoding[256] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 
	0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011, 0x0012, 0x0013, 
	0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 
	0x001E, 0x001F, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 
	0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031, 
	0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 
	0x003C, 0x003D, 0x003E, 0x003F, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 
	0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 
	0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 0x0060, 0x0061, 0x0062, 0x0063, 
	0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 
	0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 
	0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F, 0x00C4, 0x00C5, 
	0x00C7, 0x00C9, 0x00D1, 0x00D6, 0x00DC, 0x00E1, 0x00E0, 0x00E2, 0x00E4, 0x00E3, 
	0x00E5, 0x00E7, 0x00E9, 0x00E8, 0x00EA, 0x00EB, 0x00ED, 0x00EC, 0x00EE, 0x00EF, 
	0x00F1, 0x00F3, 0x00F2, 0x00F4, 0x00F6, 0x00F5, 0x00FA, 0x00F9, 0x00FB, 0x00FC, 
	0x2020, 0x00B0, 0x00A2, 0x00A3, 0x00A7, 0x2022, 0x00B6, 0x00DF, 0x00AE, 0x00A9, 
	0x2122, 0x00B4, 0x00A8, 0x2260, 0x00C6, 0x00D8, 0x221E, 0x00B1, 0x2264, 0x2265, 
	0x00A5, 0x00B5, 0x2202, 0x2211, 0x220F, 0x03C0, 0x222B, 0x00AA, 0x00BA, 0x03A9, 
	0x00E6, 0x00F8, 0x00BF, 0x00A1, 0x00AC, 0x221A, 0x0192, 0x2248, 0x2206, 0x00AB, 
	0x00BB, 0x2026, 0x00A0, 0x00C0, 0x00C3, 0x00D5, 0x0152, 0x0153, 0x2013, 0x2014, 
	0x201C, 0x201D, 0x2018, 0x2019, 0x00F7, 0x25CA, 0x00FF, 0x0178, 0x2044, 0x20AC, 
	0x2039, 0x203A, 0xFB01, 0xFB02, 0x2021, 0x00B7, 0x201A, 0x201E, 0x2030, 0x00C2, 
	0x00CA, 0x00C1, 0x00CB, 0x00C8, 0x00CD, 0x00CE, 0x00CF, 0x00CC, 0x00D3, 0x00D4, 
	0xF8FF, 0x00D2, 0x00DA, 0x00DB, 0x00D9, 0x0131, 0x02C6, 0x02DC, 0x00AF, 0x02D8, 
	0x02D9, 0x02DA, 0x00B8, 0x02DD, 0x02DB, 0x02C7 };
const HE_WCHAR gMacExpertEncoding[256] = {
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0020, 0xF721, 0xF6F8, 0xF7A2, 0xF724, 0xF6E4, 0xF726, 0xF7B4,
	0x207D, 0x207E, 0x2025, 0x2024, 0x002C, 0x002D, 0x002E, 0x2044, 0xF730, 0xF731,
	0xF732, 0xF733, 0xF734, 0xF735, 0xF736, 0xF737, 0xF738, 0xF739, 0x003A, 0x003B,
	0x0000, 0xF6DE, 0x0000, 0xF73F, 0x0000, 0x0000, 0x0000, 0x0000, 0xF7F0, 0x0000,
	0x0000, 0x00BC, 0x00BD, 0x00BE, 0x215B, 0x215C, 0x215D, 0x215E, 0x2153, 0x2154,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFB00, 0xFB01, 0xFB02, 0xFB03,
	0xFB04, 0x208D, 0x0000, 0x208E, 0xF6F6, 0xF6E5, 0xF760, 0xF761, 0xF762, 0xF763,
	0xF764, 0xF765, 0xF766, 0xF767, 0xF768, 0xF769, 0xF76A, 0xF76B, 0xF76C, 0xF76D,
	0xF76E, 0xF76F, 0xF770, 0xF771, 0xF772, 0xF773, 0xF774, 0xF775, 0xF776, 0xF777,
	0xF778, 0xF779, 0xF77A, 0x20A1, 0xF6DC, 0xF6DD, 0xF6FE, 0x0000, 0x0000, 0xF6E9,
	0xF6E0, 0x0000, 0x0000, 0x0000, 0x0000, 0xF7E1, 0xF7E0, 0xF7E2, 0xF7E4, 0xF7E3,
	0xF7E5, 0xF7E7, 0xF7E9, 0xF7E8, 0xF7EA, 0xF7EB, 0xF7ED, 0xF7EC, 0xF7EE, 0xF7EF,
	0xF7F1, 0xF7F3, 0xF7F2, 0xF7F4, 0xF7F6, 0xF7F5, 0xF7FA, 0xF7F9, 0xF7FB, 0xF7FC,
	0x0000, 0x2078, 0x2084, 0x2083, 0x2086, 0x2088, 0x2087, 0xF6FD, 0x0000, 0xF6DF,
	0x2082, 0x0000, 0xF7A8, 0x0000, 0xF6F5, 0xF6F0, 0x2085, 0x0000, 0xF6E1, 0xF6E7,
	0xF7FD, 0x0000, 0xF6E3, 0x0000, 0x0000, 0xF7FE, 0x0000, 0x2089, 0x2080, 0xF6FF,
	0xF7E6, 0xF7F8, 0xF7BF, 0x2081, 0xF6F9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0xF7B8, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xF6FA, 0x2012, 0xF6E6,
	0x0000, 0x0000, 0x0000, 0x0000, 0xF7A1, 0x0000, 0xF7FF, 0x0000, 0x00B9, 0x00B2,
	0x00B3, 0x2074, 0x2075, 0x2076, 0x2077, 0x2079, 0x2070, 0x0000, 0xF6EC, 0xF6F1,
	0xF6F3, 0x0000, 0x0000, 0xF6ED, 0xF6F2, 0xF6EB, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0xF6EE, 0xF6FB, 0xF6F4, 0xF7AF, 0xF6EA, 0x207F, 0xF6EF, 0xF6E2, 0xF6E8,
	0xF6F7, 0xF6FC, 0x0000, 0x0000, 0x0000, 0x0000 };
const HE_WCHAR gStandardEncoding[256] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x2019,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F, 0x0030, 0x0031,
	0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B,
	0x003C, 0x003D, 0x003E, 0x003F, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045,
	0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059,
	0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F, 0x0060, 0x0061, 0x0062, 0x0063,
	0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D,
	0x006E, 0x006F, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
	0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x00A1, 0x00A2, 0x00A3, 0x2044, 0x00A5, 0x0192, 0x00A7, 0x00A4, 0x0027,
	0x201C, 0x00AB, 0x2039, 0x203A, 0xFB01, 0xFB02, 0x0000, 0x2013, 0x2020, 0x2021,
	0x00B7, 0x0000, 0x00B6, 0x2022, 0x201A, 0x201E, 0x201D, 0x00BB, 0x2026, 0x2030,
	0x0000, 0x00BF, 0x0000, 0x0060, 0x00B4, 0x02C6, 0x02DC, 0x00AF, 0x02C9, 0x02D8,
	0x02D9, 0x00A8, 0x0000, 0x02DA, 0x00B8, 0x02DD, 0x02DB, 0x02C7, 0x2014, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00C6, 0x0000, 0x00AA, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0141, 0x00D8, 0x0152, 0x00BA, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x00E6, 0x0000, 0x0000, 0x0000, 0x0131, 0x0000, 0x0000, 0x0142, 0x00F8,
	0x0153, 0x00DF, 0x0000, 0x0000, 0x0000, 0x0000 };
const HE_WCHAR gSymbolEncoding[256] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0020, 0x0021, 0x2200, 0x0023, 0x2203, 0x0025, 0x0026, 0x220B,
	0x0028, 0x0029, 0x2217, 0x002B, 0x002C, 0x2212, 0x002E, 0x002F, 0x0030, 0x0031,
	0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B,
	0x003C, 0x003D, 0x003E, 0x003F, 0x2245, 0x0391, 0x0392, 0x03A7, 0x0394, 0x0395,
	0x03A6, 0x0393, 0x0397, 0x0399, 0x03D1, 0x039A, 0x039B, 0x039C, 0x039D, 0x039F,
	0x03A0, 0x0398, 0x03A1, 0x03A3, 0x03A4, 0x03A5, 0x03C2, 0x03A9, 0x039E, 0x03A8,
	0x0396, 0x005B, 0x2234, 0x005D, 0x22A5, 0x005F, 0xF8E5, 0x03B1, 0x03B2, 0x03C7,
	0x03B4, 0x03B5, 0x03C6, 0x03B3, 0x03B7, 0x03B9, 0x03D5, 0x03BA, 0x03BB, 0x03BC,
	0x03BD, 0x03BF, 0x03C0, 0x03B8, 0x03C1, 0x03C3, 0x03C4, 0x03C5, 0x03D6, 0x03C9,
	0x03BE, 0x03C8, 0x03B6, 0x007B, 0x007C, 0x007D, 0x223C, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x20AC, 0x03D2, 0x2032, 0x2264, 0x2044, 0x221E, 0x0192, 0x2663, 0x2666, 0x2665, 
	0x2660, 0x2194, 0x2190, 0x2191, 0x2192, 0x2193, 0x00B0, 0x00B1, 0x2033, 0x2265, 
	0x00D7, 0x221D, 0x2202, 0x2022, 0x00F7, 0x2260, 0x2261, 0x2248, 0x2026, 0xF8E6, 
	0xF8E7, 0x21B5, 0x2135, 0x2111, 0x211C, 0x2118, 0x2297, 0x2295, 0x2205, 0x2229, 
	0x222A, 0x2283, 0x2287, 0x2284, 0x2282, 0x2286, 0x2208, 0x2209, 0x2220, 0x2207, 
	0xF6DA, 0xF6D9, 0xF6DB, 0x220F, 0x221A, 0x22C5, 0x00AC, 0x2227, 0x2228, 0x21D4, 
	0x21D0, 0x21D1, 0x21D2, 0x21D3, 0x25CA, 0x2329, 0xF8E8, 0xF8E9, 0xF8EA, 0x2211, 
	0xF8EB, 0xF8EC, 0xF8ED, 0xF8EE, 0xF8EF, 0xF8F0, 0xF8F1, 0xF8F2, 0xF8F3, 0xF8F4, 
	0x0000, 0x232A, 0x222B, 0x2320, 0xF8F5, 0x2321, 0xF8F6, 0xF8F7, 0xF8F8, 0xF8F9, 
	0xF8FA, 0xF8FB, 0xF8FC, 0xF8FD, 0xF8FE, 0x0000 };
const HE_WCHAR gPdfZapfDingbatsEncoding[256] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0020, 0x2701, 0x2702, 0x2703, 0x2704, 0x260E, 0x2706, 0x2707,  
	0x2708, 0x2709, 0x261B, 0x261E, 0x270C, 0x270D, 0x270E, 0x270F, 0x2710, 0x2711,
	0x2712, 0x2713, 0x2714, 0x2715, 0x2716, 0x2717, 0x2718, 0x2719, 0x271A, 0x271B,
	0x271C, 0x271D, 0x271E, 0x271F, 0x2720, 0x2721, 0x2722, 0x2723, 0x2724, 0x2725,
	0x2726, 0x2727, 0x2605, 0x2729, 0x272A, 0x272B, 0x272C, 0x272D, 0x272E, 0x272F,
	0x2730, 0x2731, 0x2732, 0x2733, 0x2734, 0x2735, 0x2736, 0x2737, 0x2738, 0x2739,
	0x273A, 0x273B, 0x273C, 0x273D, 0x273E, 0x273F, 0x2740, 0x2741, 0x2742, 0x2743,
	0x2744, 0x2745, 0x2746, 0x2747, 0x2748, 0x2749, 0x274A, 0x274B, 0x25CF, 0x274D,
	0x25A0, 0x274F, 0x2750, 0x2751, 0x2752, 0x25B2, 0x25BC, 0x25C6, 0x2756, 0x25D7,
	0x2758, 0x2759, 0x275A, 0x275B, 0x275C, 0x275D, 0x275E, 0x0000, 0xF8D7, 0xF8D8,
	0xF8D9, 0xF8DA, 0xF8DB, 0xF8DC, 0xF8DD, 0xF8DE, 0xF8DF, 0xF8E0, 0xF8E1, 0xF8E2,
	0xF8E3, 0xF8E4, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x2761, 0x2762, 0x2763, 0x2764, 0x2765, 0x2766, 0x2767, 0x2663, 0x2666,
	0x2665, 0x2660, 0x2460, 0x2461, 0x2462, 0x2463, 0x2464, 0x2465, 0x2466, 0x2467,
	0x2468, 0x2469, 0x2776, 0x2777, 0x2778, 0x2779, 0x277A, 0x277B, 0x277C, 0x277D,
	0x277E, 0x277F, 0x2780, 0x2781, 0x2782, 0x2783, 0x2784, 0x2785, 0x2786, 0x2787,
	0x2788, 0x2789, 0x278A, 0x278B, 0x278C, 0x278D, 0x278E, 0x278F, 0x2790, 0x2791,
	0x2792, 0x2793, 0x2794, 0x2192, 0x2194, 0x2195, 0x2798, 0x2799, 0x279A, 0x279B,
	0x279C, 0x279D, 0x279E, 0x279F, 0x27A0, 0x27A1, 0x27A2, 0x27A3, 0x27A4, 0x27A5,
	0x27A6, 0x27A7, 0x27A8, 0x27A9, 0x27AA, 0x27AB, 0x27AC, 0x27AD, 0x27AE, 0x27AF,
	0x0000, 0x27B1, 0x27B2, 0x27B3, 0x27B4, 0x27B5, 0x27B6, 0x27B7, 0x27B8, 0x27B9,
	0x27BA, 0x27BB, 0x27BC, 0x27BD, 0x27BE, 0x0000 };


HE_BOOL HE_GetCodeFromName( CHE_ByteString & name, HE_BYTE encoding, HE_BYTE & ret )
{
	if ( encoding == FONT_ENCODING_STANDARD )
	{
		switch ( name[0] )
		{
		case 'A':
			if ( name == "A" )
			{
				ret = 0x41;
				return TRUE;
			}else if ( name == "AE" )
			{
				ret = 0xE1;
				return TRUE;
			}else if ( name == "Aacute" )
			{
				return FALSE;
			}else if ( name == "Acircumflex" )
			{
				return FALSE;
			}else if ( name == "Adieresis" )
			{
				return FALSE;
			}else if ( name == "Agrave" )
			{
				return FALSE;
			}else if ( name == "Aring" )
			{
				return FALSE;
			}else if ( name == "Atilde" )
			{
				return FALSE;
			}
			break;
		case 'B':
			ret = 0x42;
			return TRUE;
			break;
		case 'C':
			if ( name == "C" )
			{
				ret = 0x43;
				return TRUE;
			}else if ( name == "Ccedilla" )
			{
				return FALSE;
			}
			break;
		case 'D':
			ret = 0x44;
			return TRUE;
			break;
		case 'E':
			if ( name == "E" )
			{
				ret = 0x45;
				return TRUE;
			}else if ( name == "Eacute" )
			{
				return FALSE;
			}else if ( name == "Ecircumflex" )
			{
				return FALSE;
			}else if ( name == "Edieresis" )
			{
				return FALSE;
			}else if ( name == "Egrave" )
			{
				return FALSE;
			}else if ( name == "Eth" )
			{
				return FALSE;
			}else if ( name == "Euro" )
			{
				return FALSE;
			}
			break;
		case 'F':
			ret = 0x46;
			return TRUE;
			break;
		case 'G':
			ret = 0x47;
			return TRUE;
			break;
		case 'H':
			ret = 0x48;
			return TRUE;
			break;
		case 'I':
			if ( name == "I" )
			{
				ret = 0x49;
				return TRUE;
			}else if ( name == "Iacute" )
			{
				return FALSE;
			}else if ( name == "Icircumflex" )
			{
				return FALSE;
			}else if ( name == "Idieresis" )
			{
				return FALSE;
			}else if ( name == "Igrave" )
			{
				return FALSE;
			}
			break;
		case 'J':
			ret = 0x4A;
			return TRUE;
			break;
		case 'K':
			ret = 0x4B;
			return TRUE;
			break;
		case 'L':
			if ( name == "L" )
			{
				ret = 0x4C;
				return TRUE;
			}else if ( name == "Lslash" )
			{
				ret = 0xE8;
				return TRUE;
			}
			break;
		case 'M':
			ret = 0x4D;
			return TRUE;
			break;
		case 'N':
			if ( name == "N" )
			{
				ret = 0x4E;
				return TRUE;
			}else if ( name == "Ntilde" )
			{
				return FALSE;
			}
			break;
		case 'O':
			if ( name == "O" )
			{
				ret = 0x4F;
				return TRUE;
			}else if ( name == "OE" )
			{
				ret = 0xEA;
				return TRUE;
			}else if ( name == "Oacute" )
			{
				return FALSE;
			}else if ( name == "Ocircumflex" )
			{
				return FALSE;
			}else if ( name == "Odieresis" )
			{
				return FALSE;
			}else if ( name == "Ograve" )
			{
				return FALSE;
			}else if ( name == "Oslash" )
			{
				ret = 0xE9;
				return FALSE;
			}else if ( name == "Otilde" )
			{
				return FALSE;
			}
			break;
		case 'P':
			ret = 0x50;
			return TRUE;
			break;
		case 'Q':
			ret = 0x51;
			return TRUE;
			break;
		case 'R':
			ret = 0x52;
			return TRUE;
			break;
		case 'S':
			if ( name == "S" )
			{
				ret = 0x53;
				return TRUE;
			}else if ( name == "Scaron" )
			{
				return FALSE;
			}
			break;
		case 'T':
			if ( name == "T" )
			{
				ret = 0x54;
				return TRUE;
			}else if ( name == "Thorn" )
			{
				return FALSE;
			}
			break;
		case 'U':
			if ( name == "U" )
			{
				ret = 0x55;
				return TRUE;
			}else if ( name == "Uacute" )
			{
				return FALSE;
			}else if ( name == "Ucircumflex" )
			{
				return FALSE;
			}else if ( name == "Udieresis" )
			{
				return FALSE;
			}else if ( name == "Ugrave" )
			{
				return FALSE;
			}
			break;
		case 'V':
			ret = 0x56;
			return TRUE;
			break;
		case 'W':
			ret = 0x57;
			return TRUE;
			break;
		case 'X':
			ret = 0x58;
			return TRUE;
			break;
		case 'Y':
			if ( name == "Y" )
			{
				ret = 0x59;
				return TRUE;
			}else if ( name == "Yacute" )
			{
				return FALSE;
			}else if ( name == "Ydieresis" )
			{
				return FALSE;
			}
			break;
		case 'Z':
			if ( name == "Z" )
			{
				ret = 0x5A;
			}else if ( name == "Zcaron" )
			{
				return FALSE;
			}
			break;
		case 'a':
			if ( name == "a" )
			{
				ret = 0x61;
				return TRUE;
			}else if ( name == "aacute" )
			{
				return FALSE;
			}else if ( name == "acircumflex" )
			{
				return FALSE;
			}else if ( name == "acute" )
			{
				ret = 0xC2;
				return TRUE;
			}else if ( name == "adieresis" )
			{
				return FALSE;
			}else if ( name == "ae" )
			{
				ret = 0xF1;
				return TRUE;
			}else if ( name == "agrave" )
			{
				return FALSE;
			}else if ( name == "ampersand" )
			{
				ret = 0x26;
				return TRUE;
			}else if ( name == "aring" )
			{
				return FALSE;
			}else if ( name == "asciicircum" )
			{
				ret = 0x5E;
				return TRUE;
			}else if ( name == "asciitidle" )
			{
				ret = 0x7E;
				return TRUE;
			}else if ( name == "asterisk" )
			{
				ret = 0x2A;
				return TRUE;
			}else if ( name == "at" )
			{
				ret = 0x40;
				return TRUE;
			}else if ( name == "atilde" )
			{
				return FALSE;
			}
			break;
		case 'b':
			if ( name == "b" )
			{
				ret = 0x62;
				return TRUE;
			}else if ( name == "backslash" )
			{
				ret = 0x5C;
				return TRUE;
			}else if ( name == "bar" )
			{
				ret = 0x7C;
				return TRUE;
			}else if ( name == "braceleft" )
			{
				ret = 0x7B;
				return TRUE;
			}else if ( name == "braceright" )
			{
				ret = 0x7D;
				return TRUE;
			}else if ( name == "bracketleft" )
			{
				ret = 0x5B;
				return TRUE;
			}else if ( name == "bracketright" )
			{
				ret = 0x5D;
				return TRUE;
			}else if ( name == "breve" )
			{
				ret = 0xC6;
				return TRUE;
			}else if ( name == "brokenbar" )
			{
				return FALSE;
			}else if ( name == "bullet" )
			{
				ret = 0xB7;
				return TRUE;
			}
			break;
		case 'c':
			if ( name == "c" )
			{
				ret = 0x63;
				return TRUE;
			}else if ( name == "caron" )
			{
				ret = 0xCF;
				return TRUE;
			}else if ( name == "ccedilla" )
			{
				return FALSE;
			}else if ( name == "cedilla" )
			{
				ret = 0xCB;
				return TRUE;
			}else if ( name == "cent" )
			{
				ret = 0xA2;
				return TRUE;
			}else if ( name == "circumflex" )
			{
				ret = 0xC3;
				return TRUE;
			}else if ( name == "colon" )
			{
				ret = 0x3A;
				return TRUE;
			}else if ( name == "comma" )
			{
				ret = 0x2C;
				return TRUE;
			}else if ( name == "copyright" )
			{
				return FALSE;
			}else if ( name == "currency" )
			{
				ret = 0xA8;
				return TRUE;
			}
			break;
		case 'd':
			if ( name == "d" )
			{
				ret = 0x64;
				return TRUE;
			}else if ( name == "dagger" )
			{
				ret = 0xB2;
				return TRUE;
			}else if ( name == "daggerdbl" )
			{
				ret = 0xB3;
				return TRUE;
			}else if ( name == "degree" )
			{
				return FALSE;
			}else if ( name == "dieresis" )
			{
				ret = 0xC8;
				return TRUE;
			}else if ( name == "divide" )
			{
				return FALSE;
			}else if ( name == "dollar" )
			{
				ret = 0x24;
				return TRUE;
			}else if ( name == "dotaccent" )
			{
				ret = 0xC7;
				return TRUE;
			}else if ( name == "dotlessi" )
			{
				ret = 0xF5;
				return TRUE;
			}
			break;
		case 'e':
			if ( name == "e" )
			{
				ret = 0x65;
				return TRUE;
			}else if ( name == "eacute" )
			{
				return FALSE;
			}else if ( name == "ecircumflex" )
			{
				return FALSE;
			}else if ( name == "edieresis" )
			{
				return FALSE;
			}else if ( name == "egrave" )
			{
				return FALSE;
			}else if ( name == "eight" )
			{
				ret = 0x38;
				return TRUE;
			}else if ( name == "ellipsis" )
			{
				ret = 0xBC;
				return TRUE;
			}else if ( name == "emdash" )
			{
				ret = 0xD0;
				return TRUE;
			}else if ( name == "endash" )
			{
				ret = 0xB1;
				return TRUE;
			}else if ( name == "equal" )
			{
				ret = 0x3D;
				return TRUE;
			}else if ( name == "eth" )
			{
				return FALSE;
			}else if ( name == "exclam" )
			{
				ret = 0x21;
				return TRUE;
			}else if ( name == "exclamdown" )
			{
				ret = 0xA1;
				return TRUE;
			}
			break;
		case 'f':
			if ( name == "f" )
			{
				ret = 0x66;
				return TRUE;
			}else if ( name == "fi" )
			{
				ret = 0xAE;
				return TRUE;
			}else if ( name == "five" )
			{
				ret = 0x35;
				return TRUE;
			}else if ( name == "fl" )
			{
				ret = 0xAF;
				return TRUE;
			}else if ( name == "florin" )
			{
				ret = 0xA6;
				return TRUE;
			}else if ( name == "four" )
			{
				ret = 0x34;
				return TRUE;
			}else if ( name == "fraction" )
			{
				ret = 0xA4;
				return TRUE;
			}
			break;
		case 'g':
			if ( name == "g" )
			{
				ret = 0x67;
				return TRUE;
			}else if ( name == "germandbls" )
			{
				ret = 0xFB;
				return TRUE;
			}else if ( name == "grave" )
			{
				ret = 0xC1;
				return TRUE;
			}else if ( name == "greater" )
			{
				ret = 0x3E;
				return TRUE;
			}else if ( name == "guillemotleft" )
			{
				ret = 0xAB;
				return TRUE;
			}else if ( name == "guillemotright" )
			{
				ret = 0xBB;
				return TRUE;
			}else if ( name == "guilsinglleft" )
			{
				ret = 0xAC;
				return TRUE;
			}else if ( name == "guilsinglright" )
			{
				ret = 0xAD;
				return TRUE;
			}
			break;
		case 'h':
			if ( name == "h" )
			{
				ret = 0x68;
				return TRUE;
			}else if ( name == "hungarumlaut" )
			{
				ret = 0xCD;
				return TRUE;
			}else if ( name == "hyphen" )
			{
				ret = 0x2D;
				return TRUE;
			}
			break;
		case 'i':
			if ( name == "i" )
			{
				ret = 0x69;
				return TRUE;
			}else if ( name == "iacute" )
			{
				return FALSE;
			}else if ( name == "icircumflex" )
			{
				return FALSE;
			}else if ( name == "idieresis" )
			{
				return FALSE;
			}else if ( name == "igrave" )
			{
				return FALSE;
			}
			break;
		case 'j':
			ret = 0x6A;
			return TRUE;
			break;
		case 'k':
			ret = 0x6B;
			return TRUE;
			break;
		case 'l':
			if ( name == "l" )
			{
				ret = 0x6C;
				return TRUE;
			}else if ( name == "less" )
			{
				ret = 0x3C;
				return TRUE;
			}else if ( name == "logicalnot" )
			{
				return FALSE;
			}else if ( name == "lslash" )
			{
				ret = 0xF8;
				return TRUE;
			}
			break;
		case 'm':
			if ( name == "m" )
			{
				ret = 0x6D;
				return TRUE;
			}else if ( name == "macron" )
			{
				ret = 0xC5;
				return TRUE;
			}else if ( name == "minus" )
			{
				return FALSE;
			}else if ( name == "mu" )
			{
				return FALSE;
			}else if ( name == "multiply" )
			{
				return FALSE;
			}
			break;
		case 'n':
			if ( name == "n" )
			{
				ret = 0x6E;
				return TRUE;
			}else if ( name == "nine" )
			{
				ret = 0x39;
				return TRUE;
			}else if ( name == "ntilde" )
			{
				return FALSE;
			}else if ( name == "numbersign" )
			{
				ret = 0x23;
				return TRUE;
			}
			break;
		case 'o':
			if ( name == "o" )
			{
				ret = 0x6F;
				return TRUE;
			}else if ( name == "oacute" )
			{
				return FALSE;
			}else if ( name == "ocircumflex" )
			{
				return FALSE;
			}else if ( name == "odieresis" )
			{
				return FALSE;
			}else if ( name == "oe" )
			{
				ret = 0xFA;
				return TRUE;
			}else if ( name == "ogonek" )
			{
				ret = 0xCE;
				return TRUE;
			}else if ( name == "ograve" )
			{
				return FALSE;
			}else if ( name == "one" )
			{
				ret = 0x31;
				return TRUE;
			}else if ( name == "onehalf" )
			{
				return FALSE;
			}else if ( name == "onequarter" )
			{
				return FALSE;
			}else if ( name == "onesuperior" )
			{
				return FALSE;
			}else if ( name == "ordfeminine" )
			{
				ret = 0xE3;
				return TRUE;
			}else if ( name == "ordmasculine" )
			{
				ret = 0xEB;
				return TRUE;
			}else if ( name == "oslash" )
			{
				ret = 0xF9;
				return TRUE;
			}else if ( name == "otilde" )
			{
				return FALSE;
			}
			break;
		case 'p':
			if ( name == "p" )
			{
				ret = 0x70;
				return TRUE;
			}else if ( name == "paragraph" )
			{
				ret = 0xB6;
				return TRUE;
			}else if ( name == "parenleft" )
			{
				ret = 0x28;
				return TRUE;
			}else if ( name == "parenright" )
			{
				ret = 0x29;
				return TRUE;
			}else if ( name == "percent" )
			{
				ret = 0x25;
				return TRUE;
			}else if ( name == "period" )
			{
				ret = 0x2E;
				return TRUE;
			}else if ( name == "periodcentered" )
			{
				ret = 0xB4;
				return TRUE;
			}else if ( name == "perthousand" )
			{
				ret = 0xBD;
				return TRUE;
			}else if ( name == "plus" )
			{
				ret = 0x2B;
				return TRUE;
			}else if ( name == "plusminus" )
			{
				return FALSE;
			}
			break;
		case 'q':
			if ( name == "q" )
			{
				ret = 0x71;
				return TRUE;
			}else if ( name == "question" )
			{
				ret = 0x3F;
				return TRUE;
			}else if ( name == "questiondown" )
			{
				ret = 0xBF;
				return TRUE;
			}else if ( name == "quotedbl" )
			{
				ret = 0x22;
				return TRUE;
			}else if ( name == "quotedblbase" )
			{
				ret = 0xB9;
				return TRUE;
			}else if ( name == "quotedblleft" )
			{
				ret = 0xAA;
				return TRUE;
			}else if ( name == "quotedblright" )
			{
				ret = 0xBA;
				return TRUE;
			}else if ( name == "quoteleft" )
			{
				ret = 0x60;
				return TRUE;
			}
			else if ( name == "quoteright" )
			{
				ret = 0x27;
				return TRUE;
			}else if ( name == "quotesinglbase" )
			{
				ret = 0xB8;
				return TRUE;
			}else if ( name == "quotesingle" )
			{
				ret = 0xA9;
				return TRUE;
			}
			break;
		case 'r':
			if ( name == "r" )
			{
				ret = 0x72;
				return TRUE;
			}else if ( name == "registered" )
			{
				return FALSE;
			}else if ( name == "ring" )
			{
				ret = 0xCA;
				return TRUE;
			}
			break;
		case 's':
			if ( name == "s" )
			{
				ret = 0x73;
				return TRUE;
			}else if ( name == "scaron" )
			{
				return FALSE;
			}else if ( name == "section" )
			{
				ret = 0xA7;
				return TRUE;
			}else if ( name == "semicolon" )
			{
				ret = 0x3B;
				return TRUE;
			}else if ( name == "seven" )
			{
				ret = 0x37;
				return TRUE;
			}else if ( name == "six" )
			{
				ret = 0x36;
				return TRUE;
			}else if ( name == "slash" )
			{
				ret = 0x2F;
				return TRUE;
			}else if ( name == "space" )
			{
				ret = 0x20;
				return TRUE;
			}
			else if ( name == "sterling" )
			{
				ret = 0xA3;
				return TRUE;
			}
			break;
		case 't':
			if ( name == "t" )
			{
				ret = 0x74;
				return TRUE;
			}else if ( name == "thorn" )
			{
				return FALSE;
			}else if ( name == "three" )
			{
				ret = 0x33;
				return TRUE;
			}else if ( name == "threequarters" )
			{
				return FALSE;
			}else if ( name == "threesuperior" )
			{
				return FALSE;
			}else if ( name == "tilde" )
			{
				ret = 0xC4;
				return TRUE;
			}else if ( name == "trademark" )
			{
				return FALSE;
			}else if ( name == "two" )
			{
				ret = 0x32;
				return TRUE;
			}
			else if ( name == "twosuperior" )
			{
				return FALSE;
			}
			break;
		case 'u':
			if ( name == "u" )
			{
				ret = 0x75;
				return TRUE;
			}else if ( name == "uacute" )
			{
				return FALSE;
			}else if ( name == "ucircumflex" )
			{
				return FALSE;
			}else if ( name == "udieresis" )
			{
				return FALSE;
			}else if ( name == "ugrave" )
			{
				return FALSE;
			}else if ( name == "underscore" )
			{
				ret = 0x5F;
				return TRUE;
			}
			break;
		case 'v':
			ret = 0x76;
			return TRUE;
			break;
		case 'w':
			ret = 0x77;
			return TRUE;
			break;
		case 'x':
			ret = 0x78;
			return TRUE;
			break;
		case 'y':
			if ( name == "y" )
			{
				ret = 0x79;
				return TRUE;
			}else if ( name == "yacute" )
			{
				return FALSE;
			}else if ( name == "ydieresis" )
			{
				return FALSE;
			}else if ( name == "yen" )
			{
				ret = 0xA5;
				return TRUE;
			}
			break;
		case 'z':
			if ( name == "z" )
			{
				ret = 0x7A;
				return TRUE;
			}else if ( name == "zcaron" )
			{
				return FALSE;
			}else if ( name == "zero" )
			{
				ret = 0x30;
				return TRUE;
			}
			break;
		}
	}else if ( encoding == FONT_ENCODING_WINANSI )
	{
		switch ( name[0] )
		{
		case 'A':
			if ( name == "A" )
			{
				ret = 0x41;
				return TRUE;
			}else if ( name == "AE" )
			{
				ret = 0xC6;
				return TRUE;
			}else if ( name == "Aacute" )
			{
				ret = 0xC1;
				return TRUE;
			}else if ( name == "Acircumflex" )
			{
				ret = 0xC2;
				return TRUE;
			}else if ( name == "Adieresis" )
			{
				ret = 0xC4;
				return TRUE;
			}else if ( name == "Agrave" )
			{
				ret = 0xC0;
				return TRUE;
			}else if ( name == "Aring" )
			{
				ret = 0xC5;
				return TRUE;
			}else if ( name == "Atilde" )
			{
				ret = 0xC3;
				return TRUE;
			}
			break;
		case 'B':
			ret = 0x42;
			return TRUE;
			break;
		case 'C':
			if ( name == "C" )
			{
				ret = 0x43;
				return TRUE;
			}else if ( name == "Ccedilla" )
			{
				ret = 0xC7;
				return TRUE;
			}
			break;
		case 'D':
			ret = 0x44;
			return TRUE;
			break;
		case 'E':
			if ( name == "E" )
			{
				ret = 0x45;
				return TRUE;
			}else if ( name == "Eacute" )
			{
				ret = 0xC9;
				return TRUE;
			}else if ( name == "Ecircumflex" )
			{
				ret = 0xCA;
				return TRUE;
			}else if ( name == "Edieresis" )
			{
				ret = 0xCB;
				return TRUE;
			}else if ( name == "Egrave" )
			{
				ret = 0xC8;
				return TRUE;
			}else if ( name == "Eth" )
			{
				ret = 0xD0;
				return TRUE;
			}else if ( name == "Euro" )
			{
				ret = 0x80;
				return TRUE;
			}
			break;
		case 'F':
			ret = 0x46;
			return TRUE;
			break;
		case 'G':
			ret = 0x47;
			return TRUE;
			break;
		case 'H':
			ret = 0x48;
			return TRUE;
			break;
		case 'I':
			if ( name == "I" )
			{
				ret = 0x49;
				return TRUE;
			}else if ( name == "Iacute" )
			{
				ret = 0xCD;
				return TRUE;
			}else if ( name == "Icircumflex" )
			{
				ret = 0xCE;
				return TRUE;
			}else if ( name == "Idieresis" )
			{
				ret = 0xCF;
				return TRUE;
			}else if ( name == "Igrave" )
			{
				ret = 0xCC;
				return TRUE;
			}
			break;
		case 'J':
			ret = 0x4A;
			return TRUE;
			break;
		case 'K':
			ret = 0x4B;
			return TRUE;
			break;
		case 'L':
			if ( name == "L" )
			{
				ret = 0x4C;
				return TRUE;
			}else if ( name == "Lslash" )
			{
				return FALSE;
			}
			break;
		case 'M':
			ret = 0x4D;
			return TRUE;
			break;
		case 'N':
			if ( name == "N" )
			{
				ret = 0x4E;
				return TRUE;
			}else if ( name == "Ntilde" )
			{
				ret = 0xD1;
				return TRUE;
			}
			break;
		case 'O':
			if ( name == "O" )
			{
				ret = 0x4F;
				return TRUE;
			}else if ( name == "OE" )
			{
				ret = 0x8C;
				return TRUE;
			}else if ( name == "Oacute" )
			{
				ret = 0xD3;
				return TRUE;
			}else if ( name == "Ocircumflex" )
			{
				ret = 0xD4;
				return TRUE;
			}else if ( name == "Odieresis" )
			{
				ret = 0xD6;
				return TRUE;
			}else if ( name == "Ograve" )
			{
				ret = 0xD2;
				return TRUE;
			}else if ( name == "Oslash" )
			{
				ret = 0xD8;
				return TRUE;
			}else if ( name == "Otilde" )
			{
				ret = 0xD5;
				return TRUE;
			}
			break;
		case 'P':
			ret = 0x50;
			return TRUE;
			break;
		case 'Q':
			ret = 0x51;
			return TRUE;
			break;
		case 'R':
			ret = 0x52;
			return TRUE;
			break;
		case 'S':
			if ( name == "S" )
			{
				ret = 0x53;
				return TRUE;
			}else if ( name == "Scaron" )
			{
				ret = 0x8A;
				return TRUE;
			}
			break;
		case 'T':
			if ( name == "T" )
			{
				ret = 0x54;
				return TRUE;
			}else if ( name == "Thorn" )
			{
				ret = 0xDE;
				return TRUE;
			}
			break;
		case 'U':
			if ( name == "U" )
			{
				ret = 0x55;
				return TRUE;
			}else if ( name == "Uacute" )
			{
				ret = 0xDA;
				return TRUE;
			}else if ( name == "Ucircumflex" )
			{
				ret = 0xDB;
				return TRUE;
			}else if ( name == "Udieresis" )
			{
				ret = 0xDC;
				return TRUE;
			}else if ( name == "Ugrave" )
			{
				ret = 0xD9;
				return TRUE;
			}
			break;
		case 'V':
			ret = 0x56;
			return TRUE;
			break;
		case 'W':
			ret = 0x57;
			return TRUE;
			break;
		case 'X':
			ret = 0x58;
			return TRUE;
			break;
		case 'Y':
			if ( name == "Y" )
			{
				ret = 0x59;
				return TRUE;
			}else if ( name == "Yacute" )
			{
				ret = 0xDD;
				return TRUE;
			}else if ( name == "Ydieresis" )
			{
				ret = 0x9F;
				return TRUE;
			}
			break;
		case 'Z':
			if ( name == "Z" )
			{
				ret = 0x5A;
			}else if ( name == "Zcaron" )
			{
				ret = 0x8E;
				return TRUE;
			}
			break;
		case 'a':
			if ( name == "a" )
			{
				ret = 0x61;
				return TRUE;
			}else if ( name == "aacute" )
			{
				ret = 0xE1;
				return TRUE;
			}else if ( name == "acircumflex" )
			{
				ret = 0xE2;
				return TRUE;
			}else if ( name == "acute" )
			{
				ret = 0xB4;
				return TRUE;
			}else if ( name == "adieresis" )
			{
				ret = 0xE4;
				return TRUE;
			}else if ( name == "ae" )
			{
				ret = 0xE6;
				return TRUE;
			}else if ( name == "agrave" )
			{
				ret = 0xE0;
				return TRUE;
			}else if ( name == "ampersand" )
			{
				ret = 0x26;
				return TRUE;
			}else if ( name == "aring" )
			{
				ret = 0xE5;
				return TRUE;
			}else if ( name == "asciicircum" )
			{
				ret = 0x5E;
				return TRUE;
			}else if ( name == "asciitidle" )
			{
				ret = 0x7E;
				return TRUE;
			}else if ( name == "asterisk" )
			{
				ret = 0x2A;
				return TRUE;
			}else if ( name == "at" )
			{
				ret = 0x40;
				return TRUE;
			}else if ( name == "atilde" )
			{
				ret = 0xE3;
				return TRUE;
			}
			break;
		case 'b':
			if ( name == "b" )
			{
				ret = 0x62;
				return TRUE;
			}else if ( name == "backslash" )
			{
				ret = 0x5C;
				return TRUE;
			}else if ( name == "bar" )
			{
				ret = 0x7C;
				return TRUE;
			}else if ( name == "braceleft" )
			{
				ret = 0x7B;
				return TRUE;
			}else if ( name == "braceright" )
			{
				ret = 0x7D;
				return TRUE;
			}else if ( name == "bracketleft" )
			{
				ret = 0x5B;
				return TRUE;
			}else if ( name == "bracketright" )
			{
				ret = 0x5D;
				return TRUE;
			}else if ( name == "breve" )
			{
				return FALSE;
			}else if ( name == "brokenbar" )
			{
				ret = 0xA6;
				return TRUE;
			}else if ( name == "bullet" )
			{
				ret = 0x95;
				return TRUE;
			}
			break;
		case 'c':
			if ( name == "c" )
			{
				ret = 0x63;
				return TRUE;
			}else if ( name == "caron" )
			{
				return FALSE;
			}else if ( name == "ccedilla" )
			{
				ret = 0xE7;
				return TRUE;
			}else if ( name == "cedilla" )
			{
				ret = 0xB8;
				return TRUE;
			}else if ( name == "cent" )
			{
				ret = 0xA2;
				return TRUE;
			}else if ( name == "circumflex" )
			{
				ret = 0x88;
				return TRUE;
			}else if ( name == "colon" )
			{
				ret = 0x3A;
				return TRUE;
			}else if ( name == "comma" )
			{
				ret = 0x2C;
				return TRUE;
			}else if ( name == "copyright" )
			{
				ret = 0xA9;
				return TRUE;
			}else if ( name == "currency" )
			{
				ret = 0xA4;
				return TRUE;
			}
			break;
		case 'd':
			if ( name == "d" )
			{
				ret = 0x64;
				return TRUE;
			}else if ( name == "dagger" )
			{
				ret = 0x86;
				return TRUE;
			}else if ( name == "daggerdbl" )
			{
				ret = 0x87;
				return TRUE;
			}else if ( name == "degree" )
			{
				ret = 0xB0;
				return TRUE;
			}else if ( name == "dieresis" )
			{
				ret = 0xA8;
				return TRUE;
			}else if ( name == "divide" )
			{
				ret = 0xF7;
				return TRUE;
			}else if ( name == "dollar" )
			{
				ret = 0x24;
				return TRUE;
			}else if ( name == "dotaccent" )
			{
				return FALSE;
			}else if ( name == "dotlessi" )
			{
				return FALSE;
			}
			break;
		case 'e':
			if ( name == "e" )
			{
				ret = 0x65;
				return TRUE;
			}else if ( name == "eacute" )
			{
				ret = 0xE9;
				return TRUE;
			}else if ( name == "ecircumflex" )
			{
				ret = 0xEA;
				return TRUE;
			}else if ( name == "edieresis" )
			{
				ret = 0xEB;
				return TRUE;
			}else if ( name == "egrave" )
			{
				ret = 0xE8;
				return TRUE;
			}else if ( name == "eight" )
			{
				ret = 0x38;
				return TRUE;
			}else if ( name == "ellipsis" )
			{
				ret = 0x85;
				return TRUE;
			}else if ( name == "emdash" )
			{
				ret = 0x97;
				return TRUE;
			}else if ( name == "endash" )
			{
				ret = 0x96;
				return TRUE;
			}else if ( name == "equal" )
			{
				ret = 0x3D;
				return TRUE;
			}else if ( name == "eth" )
			{
				ret = 0xF0;
				return TRUE;
			}else if ( name == "exclam" )
			{
				ret = 0x21;
				return TRUE;
			}else if ( name == "exclamdown" )
			{
				ret = 0xA1;
				return TRUE;
			}
			break;
		case 'f':
			if ( name == "f" )
			{
				ret = 0x66;
				return TRUE;
			}else if ( name == "fi" )
			{
				return FALSE;
			}else if ( name == "five" )
			{
				ret = 0x35;
				return TRUE;
			}else if ( name == "fl" )
			{
				return FALSE;
			}else if ( name == "florin" )
			{
				ret = 0x83;
				return TRUE;
			}else if ( name == "four" )
			{
				ret = 0x34;
				return TRUE;
			}else if ( name == "fraction" )
			{
				return FALSE;
			}
			break;
		case 'g':
			if ( name == "g" )
			{
				ret = 0x67;
				return TRUE;
			}else if ( name == "germandbls" )
			{
				ret = 0xDF;
				return TRUE;
			}else if ( name == "grave" )
			{
				ret = 0x60;
				return TRUE;
			}else if ( name == "greater" )
			{
				ret = 0x3E;
				return TRUE;
			}else if ( name == "guillemotleft" )
			{
				ret = 0xAB;
				return TRUE;
			}else if ( name == "guillemotright" )
			{
				ret = 0xBB;
				return TRUE;
			}else if ( name == "guilsinglleft" )
			{
				ret = 0x8B;
				return TRUE;
			}else if ( name == "guilsinglright" )
			{
				ret = 0x9B;
				return TRUE;
			}
			break;
		case 'h':
			if ( name == "h" )
			{
				ret = 0x68;
				return TRUE;
			}else if ( name == "hungarumlaut" )
			{
				return FALSE;
			}else if ( name == "hyphen" )
			{
				ret = 0x2D;
				return TRUE;
			}
			break;
		case 'i':
			if ( name == "i" )
			{
				ret = 0x69;
				return TRUE;
			}else if ( name == "iacute" )
			{
				ret = 0xED;
				return TRUE;
			}else if ( name == "icircumflex" )
			{
				ret = 0xEE;
				return TRUE;
			}else if ( name == "idieresis" )
			{
				ret = 0xEF;
				return TRUE;
			}else if ( name == "igrave" )
			{
				ret = 0xEC;
				return TRUE;
			}
			break;
		case 'j':
			ret = 0x6A;
			return TRUE;
			break;
		case 'k':
			ret = 0x6B;
			return TRUE;
			break;
		case 'l':
			if ( name == "l" )
			{
				ret = 0x6C;
				return TRUE;
			}else if ( name == "less" )
			{
				ret = 0x3C;
				return TRUE;
			}else if ( name == "logicalnot" )
			{
				ret = 0xAC;
				return TRUE;
			}else if ( name == "lslash" )
			{
				return FALSE;
			}
			break;
		case 'm':
			if ( name == "m" )
			{
				ret = 0x6D;
				return FALSE;
			}else if ( name == "macron" )
			{
				ret = 0xAF;
				return TRUE;
			}else if ( name == "minus" )
			{
				return FALSE;
			}else if ( name == "mu" )
			{
				ret = 0xB5;
				return TRUE;
			}else if ( name == "multiply" )
			{
				ret = 0xD7;
				return TRUE;
			}
			break;
		case 'n':
			if ( name == "n" )
			{
				ret = 0x6E;
				return TRUE;
			}else if ( name == "nine" )
			{
				ret = 0x39;
				return TRUE;
			}else if ( name == "ntilde" )
			{
				ret = 0xF1;
				return TRUE;
			}else if ( name == "numbersign" )
			{
				ret = 0x23;
				return TRUE;
			}
			break;
		case 'o':
			if ( name == "o" )
			{
				ret = 0x6F;
				return TRUE;
			}else if ( name == "oacute" )
			{
				ret = 0xF3;
				return TRUE;
			}else if ( name == "ocircumflex" )
			{
				ret = 0xF4;
				return TRUE;
			}else if ( name == "odieresis" )
			{
				ret = 0xF6;
				return TRUE;
			}else if ( name == "oe" )
			{
				ret = 0x9C;
				return TRUE;
			}else if ( name == "ogonek" )
			{
				return FALSE;
			}else if ( name == "ograve" )
			{
				ret = 0xF2;
				return TRUE;
			}else if ( name == "one" )
			{
				ret = 0x31;
				return TRUE;
			}else if ( name == "onehalf" )
			{
				ret = 0xBD;
				return TRUE;
			}else if ( name == "onequarter" )
			{
				ret = 0xBC;
				return TRUE;
			}else if ( name == "onesuperior" )
			{
				ret = 0xB9;
				return TRUE;
			}else if ( name == "ordfeminine" )
			{
				ret = 0xAA;
				return TRUE;
			}else if ( name == "ordmasculine" )
			{
				ret = 0xBA;
				return TRUE;
			}else if ( name == "oslash" )
			{
				ret = 0xF8;
				return TRUE;
			}else if ( name == "otilde" )
			{
				ret = 0xF6;
				return TRUE;
			}
			break;
		case 'p':
			if ( name == "p" )
			{
				ret = 0x70;
				return TRUE;
			}else if ( name == "paragraph" )
			{
				ret = 0xB6;
				return TRUE;
			}else if ( name == "parenleft" )
			{
				ret = 0x28;
				return TRUE;
			}else if ( name == "parenright" )
			{
				ret = 0x29;
				return TRUE;
			}else if ( name == "percent" )
			{
				ret = 0x25;
				return TRUE;
			}else if ( name == "period" )
			{
				ret = 0x2E;
				return TRUE;
			}else if ( name == "periodcentered" )
			{
				ret = 0xB7;
				return TRUE;
			}else if ( name == "perthousand" )
			{
				ret = 0x89;
				return TRUE;
			}else if ( name == "plus" )
			{
				ret = 0x2B;
				return TRUE;
			}else if ( name == "plusminus" )
			{
				ret = 0xB1;
				return TRUE;
			}
			break;
		case 'q':
			if ( name == "q" )
			{
				ret = 0x71;
				return TRUE;
			}else if ( name == "question" )
			{
				ret = 0x3F;
				return TRUE;
			}else if ( name == "questiondown" )
			{
				ret = 0xBF;
				return TRUE;
			}else if ( name == "quotedbl" )
			{
				ret = 0x22;
				return TRUE;
			}else if ( name == "quotedblbase" )
			{
				ret = 0x84;
				return TRUE;
			}else if ( name == "quotedblleft" )
			{
				ret = 0x93;
				return TRUE;
			}else if ( name == "quotedblright" )
			{
				ret = 0x94;
				return TRUE;
			}else if ( name == "quoteleft" )
			{
				ret = 0x91;
				return TRUE;
			}
			else if ( name == "quoteright" )
			{
				ret = 0x92;
				return TRUE;
			}else if ( name == "quotesinglbase" )
			{
				ret = 0x82;
				return TRUE;
			}else if ( name == "quotesingle" )
			{
				ret = 0x27;
				return TRUE;
			}
			break;
		case 'r':
			if ( name == "r" )
			{
				ret = 0x72;
				return TRUE;
			}else if ( name == "registered" )
			{
				ret = 0xAE;
				return TRUE;
			}else if ( name == "ring" )
			{
				return FALSE;
			}
			break;
		case 's':
			if ( name == "s" )
			{
				ret = 0x73;
				return TRUE;
			}else if ( name == "scaron" )
			{
				ret = 0x9A;
				return TRUE;
			}else if ( name == "section" )
			{
				ret = 0xA7;
				return TRUE;
			}else if ( name == "semicolon" )
			{
				ret = 0x3B;
				return TRUE;
			}else if ( name == "seven" )
			{
				ret = 0x37;
				return TRUE;
			}else if ( name == "six" )
			{
				ret = 0x36;
				return TRUE;
			}else if ( name == "slash" )
			{
				ret = 0x2F;
				return TRUE;
			}else if ( name == "space" )
			{
				ret = 0x20;
				return TRUE;
			}
			else if ( name == "sterling" )
			{
				ret = 0xA3;
				return TRUE;
			}
			break;
		case 't':
			if ( name == "t" )
			{
				ret = 0x74;
				return TRUE;
			}else if ( name == "thorn" )
			{
				ret = 0xFE;
				return TRUE;
			}else if ( name == "three" )
			{
				ret = 0x33;
				return TRUE;
			}else if ( name == "threequarters" )
			{
				ret = 0xBE;
				return TRUE;
			}else if ( name == "threesuperior" )
			{
				ret = 0xB3;
				return TRUE;
			}else if ( name == "tilde" )
			{
				ret = 0x98;
				return TRUE;
			}else if ( name == "trademark" )
			{
				ret = 0x99;
				return TRUE;
			}else if ( name == "two" )
			{
				ret = 0x32;
				return TRUE;
			}
			else if ( name == "twosuperior" )
			{
				ret = 0xB2;
				return TRUE;
			}
			break;
		case 'u':
			if ( name == "u" )
			{
				ret = 0x75;
				return TRUE;
			}else if ( name == "uacute" )
			{
				ret = 0xFA;
				return TRUE;
			}else if ( name == "ucircumflex" )
			{
				ret = 0xFB;
				return TRUE;
			}else if ( name == "udieresis" )
			{
				ret = 0xFC;
				return TRUE;
			}else if ( name == "ugrave" )
			{
				ret = 0xF9;
				return TRUE;
			}else if ( name == "underscore" )
			{
				ret = 0x5F;
				return TRUE;
			}
			break;
		case 'v':
			ret = 0x76;
			return TRUE;
			break;
		case 'w':
			ret = 0x77;
			return TRUE;
			break;
		case 'x':
			ret = 0x78;
			return TRUE;
			break;
		case 'y':
			if ( name == "y" )
			{
				ret = 0x79;
				return TRUE;
			}else if ( name == "yacute" )
			{
				ret = 0xFD;
				return TRUE;
			}else if ( name == "ydieresis" )
			{
				ret = 0xFF;
				return TRUE;
			}else if ( name == "yen" )
			{
				ret = 0xA5;
				return TRUE;
			}
			break;
		case 'z':
			if ( name == "z" )
			{
				ret = 0x7A;
				return TRUE;
			}else if ( name == "zcaron" )
			{
				ret = 0x9E;
				return TRUE;
			}else if ( name == "zero" )
			{
				ret = 0x30;
				return TRUE;
			}
			break;
		}
	}else if ( encoding == FONT_ENCODING_MACROMAN || encoding == FONT_ENCODING_MACEXPERT )
	{
		switch ( name[0] )
		{
		case 'A':
			if ( name == "A" )
			{
				ret = 0x41;
				return TRUE;
			}else if ( name == "AE" )
			{
				ret = 0xAE;
				return TRUE;
			}else if ( name == "Aacute" )
			{
				ret = 0xE7;
				return TRUE;
			}else if ( name == "Acircumflex" )
			{
				ret = 0xE5;
				return TRUE;
			}else if ( name == "Adieresis" )
			{
				ret = 0x80;
				return TRUE;
			}else if ( name == "Agrave" )
			{
				ret = 0xCB;
				return TRUE;
			}else if ( name == "Aring" )
			{
				ret = 0x81;
				return TRUE;
			}else if ( name == "Atilde" )
			{
				ret = 0xCC;
				return TRUE;
			}
			break;
		case 'B':
			ret = 0x42;
			return TRUE;
			break;
		case 'C':
			if ( name == "C" )
			{
				ret = 0x43;
				return TRUE;
			}else if ( name == "Ccedilla" )
			{
				ret = 0x82;
				return TRUE;
			}
			break;
		case 'D':
			ret = 0x44;
			return TRUE;
			break;
		case 'E':
			if ( name == "E" )
			{
				ret = 0x45;
				return TRUE;
			}else if ( name == "Eacute" )
			{
				ret = 0x83;
				return TRUE;
			}else if ( name == "Ecircumflex" )
			{
				ret = 0xE6;
				return TRUE;
			}else if ( name == "Edieresis" )
			{
				ret = 0xE8;
				return TRUE;
			}else if ( name == "Egrave" )
			{
				ret = 0xE9;
				return TRUE;
			}else if ( name == "Eth" )
			{
				return FALSE;
			}else if ( name == "Euro" )
			{
				return FALSE;
			}
			break;
		case 'F':
			ret = 0x46;
			return TRUE;
			break;
		case 'G':
			ret = 0x47;
			return TRUE;
			break;
		case 'H':
			ret = 0x48;
			return TRUE;
			break;
		case 'I':
			if ( name == "I" )
			{
				ret = 0x49;
				return TRUE;
			}else if ( name == "Iacute" )
			{
				ret = 0xEA;
				return TRUE;
			}else if ( name == "Icircumflex" )
			{
				ret = 0xEB;
				return TRUE;
			}else if ( name == "Idieresis" )
			{
				ret = 0xEC;
				return TRUE;
			}else if ( name == "Igrave" )
			{
				ret = 0xED;
				return TRUE;
			}
			break;
		case 'J':
			ret = 0x4A;
			return TRUE;
			break;
		case 'K':
			ret = 0x4B;
			return TRUE;
			break;
		case 'L':
			if ( name == "L" )
			{
				ret = 0x4C;
				return TRUE;
			}else if ( name == "Lslash" )
			{
				return FALSE;
			}
			break;
		case 'M':
			ret = 0x4D;
			return TRUE;
			break;
		case 'N':
			if ( name == "N" )
			{
				ret = 0x4E;
				return TRUE;
			}else if ( name == "Ntilde" )
			{
				ret = 0x84;
				return TRUE;
			}
			break;
		case 'O':
			if ( name == "O" )
			{
				ret = 0x4F;
				return TRUE;
			}else if ( name == "OE" )
			{
				ret = 0xCE;
				return TRUE;
			}else if ( name == "Oacute" )
			{
				ret = 0xEE;
				return TRUE;
			}else if ( name == "Ocircumflex" )
			{
				ret = 0xEF;
				return TRUE;
			}else if ( name == "Odieresis" )
			{
				ret = 0x85;
				return TRUE;
			}else if ( name == "Ograve" )
			{
				ret = 0xF1;
				return TRUE;
			}else if ( name == "Oslash" )
			{
				ret = 0xAF;
				return TRUE;
			}else if ( name == "Otilde" )
			{
				ret = 0xCD;
				return TRUE;
			}
			break;
		case 'P':
			ret = 0x50;
			return TRUE;
			break;
		case 'Q':
			ret = 0x51;
			return TRUE;
			break;
		case 'R':
			ret = 0x52;
			return TRUE;
			break;
		case 'S':
			if ( name == "S" )
			{
				ret = 0x53;
				return TRUE;
			}else if ( name == "Scaron" )
			{
				return FALSE;
			}
			break;
		case 'T':
			if ( name == "T" )
			{
				ret = 0x54;
				return TRUE;
			}else if ( name == "Thorn" )
			{
				return FALSE;
			}
			break;
		case 'U':
			if ( name == "U" )
			{
				ret = 0x55;
				return TRUE;
			}else if ( name == "Uacute" )
			{
				ret = 0xF2;
				return TRUE;
			}else if ( name == "Ucircumflex" )
			{
				ret = 0xF3;
				return TRUE;
			}else if ( name == "Udieresis" )
			{
				ret = 0x86;
				return TRUE;
			}else if ( name == "Ugrave" )
			{
				ret = 0xF4;
				return TRUE;
			}
			break;
		case 'V':
			ret = 0x56;
			return TRUE;
			break;
		case 'W':
			ret = 0x57;
			return TRUE;
			break;
		case 'X':
			ret = 0x58;
			return TRUE;
			break;
		case 'Y':
			if ( name == "Y" )
			{
				ret = 0x59;
				return TRUE;
			}else if ( name == "Yacute" )
			{
				return FALSE;
			}else if ( name == "Ydieresis" )
			{
				ret = 0xD9;
				return TRUE;
			}
			break;
		case 'Z':
			if ( name == "Z" )
			{
				ret = 0x5A;
			}else if ( name == "Zcaron" )
			{
				return FALSE;
			}
			break;
		case 'a':
			if ( name == "a" )
			{
				ret = 0x61;
				return TRUE;
			}else if ( name == "aacute" )
			{
				ret = 0x87;
				return TRUE;
			}else if ( name == "acircumflex" )
			{
				ret = 0x89;
				return TRUE;
			}else if ( name == "acute" )
			{
				ret = 0xAB;
				return TRUE;
			}else if ( name == "adieresis" )
			{
				ret = 0x8A;
				return TRUE;
			}else if ( name == "ae" )
			{
				ret = 0xBE;
				return TRUE;
			}else if ( name == "agrave" )
			{
				ret = 0x88;
				return TRUE;
			}else if ( name == "ampersand" )
			{
				ret = 0x26;
				return TRUE;
			}else if ( name == "aring" )
			{
				ret = 0x8C;
				return TRUE;
			}else if ( name == "asciicircum" )
			{
				ret = 0x5E;
				return TRUE;
			}else if ( name == "asciitidle" )
			{
				ret = 0x7E;
				return TRUE;
			}else if ( name == "asterisk" )
			{
				ret = 0x2A;
				return TRUE;
			}else if ( name == "at" )
			{
				ret = 0x40;
				return TRUE;
			}else if ( name == "atilde" )
			{
				ret = 0x8B;
				return TRUE;
			}
			break;
		case 'b':
			if ( name == "b" )
			{
				ret = 0x62;
				return TRUE;
			}else if ( name == "backslash" )
			{
				ret = 0x5C;
				return TRUE;
			}else if ( name == "bar" )
			{
				ret = 0x7C;
				return TRUE;
			}else if ( name == "braceleft" )
			{
				ret = 0x7B;
				return TRUE;
			}else if ( name == "braceright" )
			{
				ret = 0x7D;
				return TRUE;
			}else if ( name == "bracketleft" )
			{
				ret = 0x5B;
				return TRUE;
			}else if ( name == "bracketright" )
			{
				ret = 0x5D;
				return TRUE;
			}else if ( name == "breve" )
			{
				ret = 0xF9;
				return TRUE;
			}else if ( name == "brokenbar" )
			{
				return FALSE;
			}else if ( name == "bullet" )
			{
				ret = 0xA5;
				return TRUE;
			}
			break;
		case 'c':
			if ( name == "c" )
			{
				ret = 0x63;
				return TRUE;
			}else if ( name == "caron" )
			{
				ret = 0xFF;
				return TRUE;
			}else if ( name == "ccedilla" )
			{
				ret = 0x8D;
				return TRUE;
			}else if ( name == "cedilla" )
			{
				ret = 0xFC;
				return TRUE;
			}else if ( name == "cent" )
			{
				ret = 0xA2;
				return TRUE;
			}else if ( name == "circumflex" )
			{
				ret = 0xF6;
				return TRUE;
			}else if ( name == "colon" )
			{
				ret = 0x3A;
				return TRUE;
			}else if ( name == "comma" )
			{
				ret = 0x2C;
				return TRUE;
			}else if ( name == "copyright" )
			{
				ret = 0xA9;
				return TRUE;
			}else if ( name == "currency" )
			{
				ret = 0xDB;
				return TRUE;
			}
			break;
		case 'd':
			if ( name == "d" )
			{
				ret = 0x64;
				return TRUE;
			}else if ( name == "dagger" )
			{
				ret = 0xA0;
				return TRUE;
			}else if ( name == "daggerdbl" )
			{
				ret = 0xB0;
				return TRUE;
			}else if ( name == "degree" )
			{
				ret = 0xA1;
				return TRUE;
			}else if ( name == "dieresis" )
			{
				ret = 0xAC;
				return TRUE;
			}else if ( name == "divide" )
			{
				ret = 0xD6;
				return TRUE;
			}else if ( name == "dollar" )
			{
				ret = 0x24;
				return TRUE;
			}else if ( name == "dotaccent" )
			{
				ret = 0xFA;
				return TRUE;
			}else if ( name == "dotlessi" )
			{
				ret = 0xF5;
				return TRUE;
			}
			break;
		case 'e':
			if ( name == "e" )
			{
				ret = 0x65;
				return TRUE;
			}else if ( name == "eacute" )
			{
				ret = 0x8E;
				return TRUE;
			}else if ( name == "ecircumflex" )
			{
				ret = 0x90;
				return TRUE;
			}else if ( name == "edieresis" )
			{
				ret = 0x91;
				return TRUE;
			}else if ( name == "egrave" )
			{
				ret = 0x8F;
				return TRUE;
			}else if ( name == "eight" )
			{
				ret = 0x38;
				return TRUE;
			}else if ( name == "ellipsis" )
			{
				ret = 0xC9;
				return TRUE;
			}else if ( name == "emdash" )
			{
				ret = 0xD1;
				return TRUE;
			}else if ( name == "endash" )
			{
				ret = 0xD0;
				return TRUE;
			}else if ( name == "equal" )
			{
				ret = 0x3D;
				return TRUE;
			}else if ( name == "eth" )
			{
				return FALSE;
			}else if ( name == "exclam" )
			{
				ret = 0x21;
				return TRUE;
			}else if ( name == "exclamdown" )
			{
				ret = 0xC1;
				return TRUE;
			}
			break;
		case 'f':
			if ( name == "f" )
			{
				ret = 0x66;
				return TRUE;
			}else if ( name == "fi" )
			{
				ret = 0xDE;
				return TRUE;
			}else if ( name == "five" )
			{
				ret = 0x35;
				return TRUE;
			}else if ( name == "fl" )
			{
				ret = 0xDF;
				return TRUE;
			}else if ( name == "florin" )
			{
				ret = 0xC4;
				return TRUE;
			}else if ( name == "four" )
			{
				ret = 0x34;
				return TRUE;
			}else if ( name == "fraction" )
			{
				ret = 0xDA;
				return TRUE;
			}
			break;
		case 'g':
			if ( name == "g" )
			{
				ret = 0x67;
				return TRUE;
			}else if ( name == "germandbls" )
			{
				ret = 0xA7;
				return TRUE;
			}else if ( name == "grave" )
			{
				ret = 0x60;
				return TRUE;
			}else if ( name == "greater" )
			{
				ret = 0x3E;
				return TRUE;
			}else if ( name == "guillemotleft" )
			{
				ret = 0xC7;
				return TRUE;
			}else if ( name == "guillemotright" )
			{
				ret = 0xC8;
				return TRUE;
			}else if ( name == "guilsinglleft" )
			{
				ret = 0xDC;
				return TRUE;
			}else if ( name == "guilsinglright" )
			{
				ret = 0xDD;
				return TRUE;
			}
			break;
		case 'h':
			if ( name == "h" )
			{
				ret = 0x68;
				return TRUE;
			}else if ( name == "hungarumlaut" )
			{
				ret = 0xFD;
				return TRUE;
			}else if ( name == "hyphen" )
			{
				ret = 0x2D;
				return TRUE;
			}
			break;
		case 'i':
			if ( name == "i" )
			{
				ret = 0x69;
				return TRUE;
			}else if ( name == "iacute" )
			{
				ret = 0x92;
				return TRUE;
			}else if ( name == "icircumflex" )
			{
				ret = 0x94;
				return TRUE;
			}else if ( name == "idieresis" )
			{
				ret = 0x95;
				return TRUE;
			}else if ( name == "igrave" )
			{
				ret = 0x93;
				return TRUE;
			}
			break;
		case 'j':
			ret = 0x6A;
			return TRUE;
			break;
		case 'k':
			ret = 0x6B;
			return TRUE;
			break;
		case 'l':
			if ( name == "l" )
			{
				ret = 0x6C;
				return TRUE;
			}else if ( name == "less" )
			{
				ret = 0x3C;
				return TRUE;
			}else if ( name == "logicalnot" )
			{
				ret = 0xC2;
				return TRUE;
			}else if ( name == "lslash" )
			{
				return FALSE;
			}
			break;
		case 'm':
			if ( name == "m" )
			{
				ret = 0x6D;
				return FALSE;
			}else if ( name == "macron" )
			{
				ret = 0xF8;
				return TRUE;
			}else if ( name == "minus" )
			{
				return FALSE;
			}else if ( name == "mu" )
			{
				ret = 0xB5;
				return TRUE;
			}else if ( name == "multiply" )
			{
				return FALSE;
			}
			break;
		case 'n':
			if ( name == "n" )
			{
				ret = 0x6E;
				return TRUE;
			}else if ( name == "nine" )
			{
				ret = 0x39;
				return TRUE;
			}else if ( name == "ntilde" )
			{
				ret = 0x96;
				return TRUE;
			}else if ( name == "numbersign" )
			{
				ret = 0x23;
				return TRUE;
			}
			break;
		case 'o':
			if ( name == "o" )
			{
				ret = 0x6F;
				return TRUE;
			}else if ( name == "oacute" )
			{
				ret = 0x97;
				return TRUE;
			}else if ( name == "ocircumflex" )
			{
				ret = 0x99;
				return TRUE;
			}else if ( name == "odieresis" )
			{
				ret = 0x9A;
				return TRUE;
			}else if ( name == "oe" )
			{
				ret = 0xCF;
				return TRUE;
			}else if ( name == "ogonek" )
			{
				ret = 0xFE;
				return TRUE;
			}else if ( name == "ograve" )
			{
				ret = 0x98;
				return TRUE;
			}else if ( name == "one" )
			{
				ret = 0x31;
				return TRUE;
			}else if ( name == "onehalf" )
			{
				return FALSE;
			}else if ( name == "onequarter" )
			{
				return FALSE;
			}else if ( name == "onesuperior" )
			{
				return FALSE;
			}else if ( name == "ordfeminine" )
			{
				ret = 0xBB;
				return TRUE;
			}else if ( name == "ordmasculine" )
			{
				ret = 0xBC;
				return TRUE;
			}else if ( name == "oslash" )
			{
				ret = 0xBF;
				return TRUE;
			}else if ( name == "otilde" )
			{
				ret = 0x9B;
				return TRUE;
			}
			break;
		case 'p':
			if ( name == "p" )
			{
				ret = 0x70;
				return TRUE;
			}else if ( name == "paragraph" )
			{
				ret = 0xA6;
				return TRUE;
			}else if ( name == "parenleft" )
			{
				ret = 0x28;
				return TRUE;
			}else if ( name == "parenright" )
			{
				ret = 0x29;
				return TRUE;
			}else if ( name == "percent" )
			{
				ret = 0x25;
				return TRUE;
			}else if ( name == "period" )
			{
				ret = 0x2E;
				return TRUE;
			}else if ( name == "periodcentered" )
			{
				ret = 0xE1;
				return TRUE;
			}else if ( name == "perthousand" )
			{
				ret = 0xE4;
				return TRUE;
			}else if ( name == "plus" )
			{
				ret = 0x2B;
				return TRUE;
			}else if ( name == "plusminus" )
			{
				ret = 0xB1;
				return TRUE;
			}
			break;
		case 'q':
			if ( name == "q" )
			{
				ret = 0x71;
				return TRUE;
			}else if ( name == "question" )
			{
				ret = 0x3F;
				return TRUE;
			}else if ( name == "questiondown" )
			{
				ret = 0xC0;
				return TRUE;
			}else if ( name == "quotedbl" )
			{
				ret = 0x22;
				return TRUE;
			}else if ( name == "quotedblbase" )
			{
				ret = 0xE3;
				return TRUE;
			}else if ( name == "quotedblleft" )
			{
				ret = 0xD2;
				return TRUE;
			}else if ( name == "quotedblright" )
			{
				ret = 0xD3;
				return TRUE;
			}else if ( name == "quoteleft" )
			{
				ret = 0xD4;
				return TRUE;
			}
			else if ( name == "quoteright" )
			{
				ret = 0xD5;
				return TRUE;
			}else if ( name == "quotesinglbase" )
			{
				ret = 0xE2;
				return TRUE;
			}else if ( name == "quotesingle" )
			{
				ret = 0x27;
				return TRUE;
			}
			break;
		case 'r':
			if ( name == "r" )
			{
				ret = 0x72;
				return TRUE;
			}else if ( name == "registered" )
			{
				ret = 0xA8;
				return TRUE;
			}else if ( name == "ring" )
			{
				ret = 0xFB;
				return TRUE;
			}
			break;
		case 's':
			if ( name == "s" )
			{
				ret = 0x73;
				return TRUE;
			}else if ( name == "scaron" )
			{
				return FALSE;
			}else if ( name == "section" )
			{
				ret = 0xA4;
				return TRUE;
			}else if ( name == "semicolon" )
			{
				ret = 0x3B;
				return TRUE;
			}else if ( name == "seven" )
			{
				ret = 0x37;
				return TRUE;
			}else if ( name == "six" )
			{
				ret = 0x36;
				return TRUE;
			}else if ( name == "slash" )
			{
				ret = 0x2F;
				return TRUE;
			}else if ( name == "space" )
			{
				ret = 0x20;
				return TRUE;
			}
			else if ( name == "sterling" )
			{
				ret = 0xA3;
				return TRUE;
			}
			break;
		case 't':
			if ( name == "t" )
			{
				ret = 0x74;
				return TRUE;
			}else if ( name == "thorn" )
			{
				ret = 0xFE;
				return TRUE;
			}else if ( name == "three" )
			{
				ret = 0x33;
				return TRUE;
			}else if ( name == "threequarters" )
			{
				return FALSE;
			}else if ( name == "threesuperior" )
			{
				return FALSE;
			}else if ( name == "tilde" )
			{
				ret = 0xF7;
				return TRUE;
			}else if ( name == "trademark" )
			{
				ret = 0xAA;
				return TRUE;
			}else if ( name == "two" )
			{
				ret = 0x32;
				return TRUE;
			}
			else if ( name == "twosuperior" )
			{
				return FALSE;
			}
			break;
		case 'u':
			if ( name == "u" )
			{
				ret = 0x75;
				return TRUE;
			}else if ( name == "uacute" )
			{
				ret = 0x9C;
				return TRUE;
			}else if ( name == "ucircumflex" )
			{
				ret = 0x9E;
				return TRUE;
			}else if ( name == "udieresis" )
			{
				ret = 0x9F;
				return TRUE;
			}else if ( name == "ugrave" )
			{
				ret = 0x9D;
				return TRUE;
			}else if ( name == "underscore" )
			{
				ret = 0x5F;
				return TRUE;
			}
			break;
		case 'v':
			ret = 0x76;
			return TRUE;
			break;
		case 'w':
			ret = 0x77;
			return TRUE;
			break;
		case 'x':
			ret = 0x78;
			return TRUE;
			break;
		case 'y':
			if ( name == "y" )
			{
				ret = 0x79;
				return TRUE;
			}else if ( name == "yacute" )
			{
				return FALSE;
			}else if ( name == "ydieresis" )
			{
				ret = 0xD8;
				return TRUE;
			}else if ( name == "yen" )
			{
				ret = 0xD4;
				return TRUE;
			}
			break;
		case 'z':
			if ( name == "z" )
			{
				ret = 0x7A;
				return TRUE;
			}else if ( name == "zcaron" )
			{
				return FALSE;
			}else if ( name == "zero" )
			{
				ret = 0x30;
				return TRUE;
			}
			break;
		}
	}else if ( encoding == FONT_ENCODING_PDFDOC )
	{
		switch ( name[0] )
		{
		case 'A':
			if ( name == "A" )
			{
				ret = 0x41;
				return TRUE;
			}else if ( name == "AE" )
			{
				ret = 0xC6;
				return TRUE;
			}else if ( name == "Aacute" )
			{
				ret = 0xC1;
				return TRUE;
			}else if ( name == "Acircumflex" )
			{
				ret = 0xC2;
				return TRUE;
			}else if ( name == "Adieresis" )
			{
				ret = 0xC4;
				return TRUE;
			}else if ( name == "Agrave" )
			{
				ret = 0xC0;
				return TRUE;
			}else if ( name == "Aring" )
			{
				ret = 0xC5;
				return TRUE;
			}else if ( name == "Atilde" )
			{
				ret = 0xC3;
				return TRUE;
			}
			break;
		case 'B':
			ret = 0x42;
			return TRUE;
			break;
		case 'C':
			if ( name == "C" )
			{
				ret = 0x43;
				return TRUE;
			}else if ( name == "Ccedilla" )
			{
				ret = 0xC7;
				return TRUE;
			}
			break;
		case 'D':
			ret = 0x44;
			return TRUE;
			break;
		case 'E':
			if ( name == "E" )
			{
				ret = 0x45;
				return TRUE;
			}else if ( name == "Eacute" )
			{
				ret = 0xC9;
				return TRUE;
			}else if ( name == "Ecircumflex" )
			{
				ret = 0xCA;
				return TRUE;
			}else if ( name == "Edieresis" )
			{
				ret = 0xCB;
				return TRUE;
			}else if ( name == "Egrave" )
			{
				ret = 0xC8;
				return TRUE;
			}else if ( name == "Eth" )
			{
				ret = 0xD0;
				return TRUE;
			}else if ( name == "Euro" )
			{
				ret = 0xA0;
				return TRUE;
			}
			break;
		case 'F':
			ret = 0x46;
			return TRUE;
			break;
		case 'G':
			ret = 0x47;
			return TRUE;
			break;
		case 'H':
			ret = 0x48;
			return TRUE;
			break;
		case 'I':
			if ( name == "I" )
			{
				ret = 0x49;
				return TRUE;
			}else if ( name == "Iacute" )
			{
				ret = 0xCD;
				return TRUE;
			}else if ( name == "Icircumflex" )
			{
				ret = 0xCE;
				return TRUE;
			}else if ( name == "Idieresis" )
			{
				ret = 0xCF;
				return TRUE;
			}else if ( name == "Igrave" )
			{
				ret = 0xCC;
				return TRUE;
			}
			break;
		case 'J':
			ret = 0x4A;
			return TRUE;
			break;
		case 'K':
			ret = 0x4B;
			return TRUE;
			break;
		case 'L':
			if ( name == "L" )
			{
				ret = 0x4C;
				return TRUE;
			}else if ( name == "Lslash" )
			{
				ret = 0x95;
				return TRUE;
			}
			break;
		case 'M':
			ret = 0x4D;
			return TRUE;
			break;
		case 'N':
			if ( name == "N" )
			{
				ret = 0x4E;
				return TRUE;
			}else if ( name == "Ntilde" )
			{
				ret = 0xD1;
				return TRUE;
			}
			break;
		case 'O':
			if ( name == "O" )
			{
				ret = 0x4F;
				return TRUE;
			}else if ( name == "OE" )
			{
				ret = 0x96;
				return TRUE;
			}else if ( name == "Oacute" )
			{
				ret = 0xD3;
				return TRUE;
			}else if ( name == "Ocircumflex" )
			{
				ret = 0xD4;
				return TRUE;
			}else if ( name == "Odieresis" )
			{
				ret = 0xD6;
				return TRUE;
			}else if ( name == "Ograve" )
			{
				ret = 0xD2;
				return TRUE;
			}else if ( name == "Oslash" )
			{
				ret = 0xD8;
				return TRUE;
			}else if ( name == "Otilde" )
			{
				ret = 0xD5;
				return TRUE;
			}
			break;
		case 'P':
			ret = 0x50;
			return TRUE;
			break;
		case 'Q':
			ret = 0x51;
			return TRUE;
			break;
		case 'R':
			ret = 0x52;
			return TRUE;
			break;
		case 'S':
			if ( name == "S" )
			{
				ret = 0x53;
				return TRUE;
			}else if ( name == "Scaron" )
			{
				ret = 0x97;
				return TRUE;
			}
			break;
		case 'T':
			if ( name == "T" )
			{
				ret = 0x54;
				return TRUE;
			}else if ( name == "Thorn" )
			{
				ret = 0xDE;
				return TRUE;
			}
			break;
		case 'U':
			if ( name == "U" )
			{
				ret = 0x55;
				return TRUE;
			}else if ( name == "Uacute" )
			{
				ret = 0xDA;
				return TRUE;
			}else if ( name == "Ucircumflex" )
			{
				ret = 0xDB;
				return TRUE;
			}else if ( name == "Udieresis" )
			{
				ret = 0xDC;
				return TRUE;
			}else if ( name == "Ugrave" )
			{
				ret = 0xD9;
				return TRUE;
			}
			break;
		case 'V':
			ret = 0x56;
			return TRUE;
			break;
		case 'W':
			ret = 0x57;
			return TRUE;
			break;
		case 'X':
			ret = 0x58;
			return TRUE;
			break;
		case 'Y':
			if ( name == "Y" )
			{
				ret = 0x59;
				return TRUE;
			}else if ( name == "Yacute" )
			{
				ret = 0xDD;
				return TRUE;
			}else if ( name == "Ydieresis" )
			{
				ret = 0x98;
				return TRUE;
			}
			break;
		case 'Z':
			if ( name == "Z" )
			{
				ret = 0x5A;
			}else if ( name == "Zcaron" )
			{
				ret = 0x99;
				return TRUE;
			}
			break;
		case 'a':
			if ( name == "a" )
			{
				ret = 0x61;
				return TRUE;
			}else if ( name == "aacute" )
			{
				ret = 0xE1;
				return TRUE;
			}else if ( name == "acircumflex" )
			{
				ret = 0xE2;
				return TRUE;
			}else if ( name == "acute" )
			{
				ret = 0xB4;
				return TRUE;
			}else if ( name == "adieresis" )
			{
				ret = 0xE4;
				return TRUE;
			}else if ( name == "ae" )
			{
				ret = 0xE6;
				return TRUE;
			}else if ( name == "agrave" )
			{
				ret = 0xE0;
				return TRUE;
			}else if ( name == "ampersand" )
			{
				ret = 0x26;
				return TRUE;
			}else if ( name == "aring" )
			{
				ret = 0xE5;
				return TRUE;
			}else if ( name == "asciicircum" )
			{
				ret = 0x5E;
				return TRUE;
			}else if ( name == "asciitidle" )
			{
				ret = 0x7E;
				return TRUE;
			}else if ( name == "asterisk" )
			{
				ret = 0x2A;
				return TRUE;
			}else if ( name == "at" )
			{
				ret = 0x40;
				return TRUE;
			}else if ( name == "atilde" )
			{
				ret = 0xE3;
				return TRUE;
			}
			break;
		case 'b':
			if ( name == "b" )
			{
				ret = 0x62;
				return TRUE;
			}else if ( name == "backslash" )
			{
				ret = 0x5C;
				return TRUE;
			}else if ( name == "bar" )
			{
				ret = 0x7C;
				return TRUE;
			}else if ( name == "braceleft" )
			{
				ret = 0x7B;
				return TRUE;
			}else if ( name == "braceright" )
			{
				ret = 0x7D;
				return TRUE;
			}else if ( name == "bracketleft" )
			{
				ret = 0x5B;
				return TRUE;
			}else if ( name == "bracketright" )
			{
				ret = 0x5D;
				return TRUE;
			}else if ( name == "breve" )
			{
				ret = 0x18;
				return TRUE;
			}else if ( name == "brokenbar" )
			{
				ret = 0xA6;
				return TRUE;
			}else if ( name == "bullet" )
			{
				ret = 0x80;
				return TRUE;
			}
			break;
		case 'c':
			if ( name == "c" )
			{
				ret = 0x63;
				return TRUE;
			}else if ( name == "caron" )
			{
				ret = 0x19;
				return TRUE;
			}else if ( name == "ccedilla" )
			{
				ret = 0xE7;
				return TRUE;
			}else if ( name == "cedilla" )
			{
				ret = 0xB8;
				return TRUE;
			}else if ( name == "cent" )
			{
				ret = 0xA2;
				return TRUE;
			}else if ( name == "circumflex" )
			{
				ret = 0x1A;
				return TRUE;
			}else if ( name == "colon" )
			{
				ret = 0x3A;
				return TRUE;
			}else if ( name == "comma" )
			{
				ret = 0x2C;
				return TRUE;
			}else if ( name == "copyright" )
			{
				ret = 0xA9;
				return TRUE;
			}else if ( name == "currency" )
			{
				ret = 0xA4;
				return TRUE;
			}
			break;
		case 'd':
			if ( name == "d" )
			{
				ret = 0x64;
				return TRUE;
			}else if ( name == "dagger" )
			{
				ret = 0x81;
				return TRUE;
			}else if ( name == "daggerdbl" )
			{
				ret = 0x82;
				return TRUE;
			}else if ( name == "degree" )
			{
				ret = 0xB0;
				return TRUE;
			}else if ( name == "dieresis" )
			{
				ret = 0xA8;
				return TRUE;
			}else if ( name == "divide" )
			{
				ret = 0xF7;
				return TRUE;
			}else if ( name == "dollar" )
			{
				ret = 0x24;
				return TRUE;
			}else if ( name == "dotaccent" )
			{
				ret = 0x1B;
				return TRUE;
			}else if ( name == "dotlessi" )
			{
				ret = 0x9A;
				return TRUE;
			}
			break;
		case 'e':
			if ( name == "e" )
			{
				ret = 0x65;
				return TRUE;
			}else if ( name == "eacute" )
			{
				ret = 0xE9;
				return TRUE;
			}else if ( name == "ecircumflex" )
			{
				ret = 0xEA;
				return TRUE;
			}else if ( name == "edieresis" )
			{
				ret = 0xEB;
				return TRUE;
			}else if ( name == "egrave" )
			{
				ret = 0xE8;
				return TRUE;
			}else if ( name == "eight" )
			{
				ret = 0x38;
				return TRUE;
			}else if ( name == "ellipsis" )
			{
				ret = 0x83;
				return TRUE;
			}else if ( name == "emdash" )
			{
				ret = 0x84;
				return TRUE;
			}else if ( name == "endash" )
			{
				ret = 0x85;
				return TRUE;
			}else if ( name == "equal" )
			{
				ret = 0x3D;
				return TRUE;
			}else if ( name == "eth" )
			{
				ret = 0xF0;
				return TRUE;
			}else if ( name == "exclam" )
			{
				ret = 0x21;
				return TRUE;
			}else if ( name == "exclamdown" )
			{
				ret = 0xA1;
				return TRUE;
			}
			break;
		case 'f':
			if ( name == "f" )
			{
				ret = 0x66;
				return TRUE;
			}else if ( name == "fi" )
			{
				ret = 0x93;
				return TRUE;
			}else if ( name == "five" )
			{
				ret = 0x35;
				return TRUE;
			}else if ( name == "fl" )
			{
				ret = 0x94;
				return TRUE;
			}else if ( name == "florin" )
			{
				ret = 0x86;
				return TRUE;
			}else if ( name == "four" )
			{
				ret = 0x34;
				return TRUE;
			}else if ( name == "fraction" )
			{
				ret = 0x95;
				return TRUE;
			}
			break;
		case 'g':
			if ( name == "g" )
			{
				ret = 0x67;
				return TRUE;
			}else if ( name == "germandbls" )
			{
				ret = 0xDF;
				return TRUE;
			}else if ( name == "grave" )
			{
				ret = 0x60;
				return TRUE;
			}else if ( name == "greater" )
			{
				ret = 0x3E;
				return TRUE;
			}else if ( name == "guillemotleft" )
			{
				ret = 0xAB;
				return TRUE;
			}else if ( name == "guillemotright" )
			{
				ret = 0xBB;
				return TRUE;
			}else if ( name == "guilsinglleft" )
			{
				ret = 0x88;
				return TRUE;
			}else if ( name == "guilsinglright" )
			{
				ret = 0x89;
				return TRUE;
			}
			break;
		case 'h':
			if ( name == "h" )
			{
				ret = 0x68;
				return TRUE;
			}else if ( name == "hungarumlaut" )
			{
				ret = 0x1C;
				return TRUE;
			}else if ( name == "hyphen" )
			{
				ret = 0x2D;
				return TRUE;
			}
			break;
		case 'i':
			if ( name == "i" )
			{
				ret = 0x69;
				return TRUE;
			}else if ( name == "iacute" )
			{
				ret = 0xED;
				return TRUE;
			}else if ( name == "icircumflex" )
			{
				ret = 0xEE;
				return TRUE;
			}else if ( name == "idieresis" )
			{
				ret = 0xEF;
				return TRUE;
			}else if ( name == "igrave" )
			{
				ret = 0xEC;
				return TRUE;
			}
			break;
		case 'j':
			ret = 0x6A;
			return TRUE;
			break;
		case 'k':
			ret = 0x6B;
			return TRUE;
			break;
		case 'l':
			if ( name == "l" )
			{
				ret = 0x6C;
				return TRUE;
			}else if ( name == "less" )
			{
				ret = 0x3C;
				return TRUE;
			}else if ( name == "logicalnot" )
			{
				ret = 0xAC;
				return TRUE;
			}else if ( name == "lslash" )
			{
				ret = 0x9B;
				return TRUE;
			}
			break;
		case 'm':
			if ( name == "m" )
			{
				ret = 0x6D;
				return FALSE;
			}else if ( name == "macron" )
			{
				ret = 0xAF;
				return TRUE;
			}else if ( name == "minus" )
			{
				ret = 0x8A;
				return TRUE;
			}else if ( name == "mu" )
			{
				ret = 0xB5;
				return TRUE;
			}else if ( name == "multiply" )
			{
				ret = 0xD7;
				return TRUE;
			}
			break;
		case 'n':
			if ( name == "n" )
			{
				ret = 0x6E;
				return TRUE;
			}else if ( name == "nine" )
			{
				ret = 0x39;
				return TRUE;
			}else if ( name == "ntilde" )
			{
				ret = 0xF1;
				return TRUE;
			}else if ( name == "numbersign" )
			{
				ret = 0x23;
				return TRUE;
			}
			break;
		case 'o':
			if ( name == "o" )
			{
				ret = 0x6F;
				return TRUE;
			}else if ( name == "oacute" )
			{
				ret = 0xF3;
				return TRUE;
			}else if ( name == "ocircumflex" )
			{
				ret = 0xF4;
				return TRUE;
			}else if ( name == "odieresis" )
			{
				ret = 0xF6;
				return TRUE;
			}else if ( name == "oe" )
			{
				ret = 0x9C;
				return TRUE;
			}else if ( name == "ogonek" )
			{
				ret = 0x1D;
				return TRUE;
			}else if ( name == "ograve" )
			{
				ret = 0xF2;
				return TRUE;
			}else if ( name == "one" )
			{
				ret = 0x31;
				return TRUE;
			}else if ( name == "onehalf" )
			{
				ret = 0xBD;
				return TRUE;
			}else if ( name == "onequarter" )
			{
				ret = 0xBC;
				return TRUE;
			}else if ( name == "onesuperior" )
			{
				ret = 0xB9;
				return TRUE;
			}else if ( name == "ordfeminine" )
			{
				ret = 0xAA;
				return TRUE;
			}else if ( name == "ordmasculine" )
			{
				ret = 0xBA;
				return TRUE;
			}else if ( name == "oslash" )
			{
				ret = 0xF8;
				return TRUE;
			}else if ( name == "otilde" )
			{
				ret = 0xF6;
				return TRUE;
			}
			break;
		case 'p':
			if ( name == "p" )
			{
				ret = 0x70;
				return TRUE;
			}else if ( name == "paragraph" )
			{
				ret = 0xB6;
				return TRUE;
			}else if ( name == "parenleft" )
			{
				ret = 0x28;
				return TRUE;
			}else if ( name == "parenright" )
			{
				ret = 0x29;
				return TRUE;
			}else if ( name == "percent" )
			{
				ret = 0x25;
				return TRUE;
			}else if ( name == "period" )
			{
				ret = 0x2E;
				return TRUE;
			}else if ( name == "periodcentered" )
			{
				ret = 0xB7;
				return TRUE;
			}else if ( name == "perthousand" )
			{
				ret = 0x8B;
				return TRUE;
			}else if ( name == "plus" )
			{
				ret = 0x2B;
				return TRUE;
			}else if ( name == "plusminus" )
			{
				ret = 0xB1;
				return TRUE;
			}
			break;
		case 'q':
			if ( name == "q" )
			{
				ret = 0x71;
				return TRUE;
			}else if ( name == "question" )
			{
				ret = 0x3F;
				return TRUE;
			}else if ( name == "questiondown" )
			{
				ret = 0xBF;
				return TRUE;
			}else if ( name == "quotedbl" )
			{
				ret = 0x22;
				return TRUE;
			}else if ( name == "quotedblbase" )
			{
				ret = 0x8C;
				return TRUE;
			}else if ( name == "quotedblleft" )
			{
				ret = 0x8D;
				return TRUE;
			}else if ( name == "quotedblright" )
			{
				ret = 0x8E;
				return TRUE;
			}else if ( name == "quoteleft" )
			{
				ret = 0x8F;
				return TRUE;
			}
			else if ( name == "quoteright" )
			{
				ret = 0x90;
				return TRUE;
			}else if ( name == "quotesinglbase" )
			{
				ret = 0x91;
				return TRUE;
			}else if ( name == "quotesingle" )
			{
				ret = 0x27;
				return TRUE;
			}
			break;
		case 'r':
			if ( name == "r" )
			{
				ret = 0x72;
				return TRUE;
			}else if ( name == "registered" )
			{
				ret = 0xAE;
				return TRUE;
			}else if ( name == "ring" )
			{
				ret = 0x1E;
				return TRUE;
			}
			break;
		case 's':
			if ( name == "s" )
			{
				ret = 0x73;
				return TRUE;
			}else if ( name == "scaron" )
			{
				ret = 0x9D;
				return TRUE;
			}else if ( name == "section" )
			{
				ret = 0xA7;
				return TRUE;
			}else if ( name == "semicolon" )
			{
				ret = 0x3B;
				return TRUE;
			}else if ( name == "seven" )
			{
				ret = 0x37;
				return TRUE;
			}else if ( name == "six" )
			{
				ret = 0x36;
				return TRUE;
			}else if ( name == "slash" )
			{
				ret = 0x2F;
				return TRUE;
			}else if ( name == "space" )
			{
				ret = 0x20;
				return TRUE;
			}
			else if ( name == "sterling" )
			{
				ret = 0xA3;
				return TRUE;
			}
			break;
		case 't':
			if ( name == "t" )
			{
				ret = 0x74;
				return TRUE;
			}else if ( name == "thorn" )
			{
				ret = 0xFE;
				return TRUE;
			}else if ( name == "three" )
			{
				ret = 0x33;
				return TRUE;
			}else if ( name == "threequarters" )
			{
				ret = 0xBE;
				return TRUE;
			}else if ( name == "threesuperior" )
			{
				ret = 0xB3;
				return TRUE;
			}else if ( name == "tilde" )
			{
				ret = 0x1F;
				return TRUE;
			}else if ( name == "trademark" )
			{
				ret = 0x92;
				return TRUE;
			}else if ( name == "two" )
			{
				ret = 0x32;
				return TRUE;
			}
			else if ( name == "twosuperior" )
			{
				ret = 0xB2;
				return TRUE;
			}
			break;
		case 'u':
			if ( name == "u" )
			{
				ret = 0x75;
				return TRUE;
			}else if ( name == "uacute" )
			{
				ret = 0xFA;
				return TRUE;
			}else if ( name == "ucircumflex" )
			{
				ret = 0xFB;
				return TRUE;
			}else if ( name == "udieresis" )
			{
				ret = 0xFC;
				return TRUE;
			}else if ( name == "ugrave" )
			{
				ret = 0xF9;
				return TRUE;
			}else if ( name == "underscore" )
			{
				ret = 0x5F;
				return TRUE;
			}
			break;
		case 'v':
			ret = 0x76;
			return TRUE;
			break;
		case 'w':
			ret = 0x77;
			return TRUE;
			break;
		case 'x':
			ret = 0x78;
			return TRUE;
			break;
		case 'y':
			if ( name == "y" )
			{
				ret = 0x79;
				return TRUE;
			}else if ( name == "yacute" )
			{
				ret = 0xFD;
				return TRUE;
			}else if ( name == "ydieresis" )
			{
				ret = 0xFF;
				return TRUE;
			}else if ( name == "yen" )
			{
				ret = 0xA5;
				return TRUE;
			}
			break;
		case 'z':
			if ( name == "z" )
			{
				ret = 0x7A;
				return TRUE;
			}else if ( name == "zcaron" )
			{
				ret = 0x9E;
				return TRUE;
			}else if ( name == "zero" )
			{
				ret = 0x30;
				return TRUE;
			}
			break;
		}
	}
	return FALSE;
}

HE_ULONG HE_HexStrToValue( CHE_ByteString & str )
{
	HE_ULONG valRet = 0;
	HE_BYTE tmpByte = 0;
	for ( HE_ULONG i = 0; i < str.GetLength(); i++ )
	{
		tmpByte = str[i];
		valRet = valRet << 8;
		valRet += tmpByte;
	}
	return valRet;
}


FT_Library HE_GetFTLibrary()
{
	if ( gFTLibrary == NULL )
	{
		HE_InitFTLibrary();
	}
	return gFTLibrary;
}


HE_VOID HE_InitFTLibrary()
{
	FT_Init_FreeType( &gFTLibrary );
}


HE_VOID HE_DestroyFTLibrary()
{
	if ( gFTLibrary != NULL )
	{
		FT_Done_FreeType( gFTLibrary );
	}
}


IHE_SystemFontMgr * HE_GetSystemFontMgr( CHE_Allocator * pAllocator /*= NULL*/ )
{
	if ( gpSystemFontMgr == NULL )
	{
		gpSystemFontMgr = IHE_SystemFontMgr::Create( pAllocator );
	}
	return gpSystemFontMgr;
}


CHE_PDF_Encoding::CHE_PDF_Encoding( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mType( FONT_ENCODING_NONE ), mBaseType( FONT_ENCODING_NONE ), mbCodeTableRelease( FALSE ), mpCodeTable( NULL )
{
	//还需要完善对于Type3字体的Encoding的支持，还要添加字体类型的配合判断

	if ( !fontDict )
	{
		return;
	}

	CHE_PDF_ObjectPtr objPtr;

	objPtr = fontDict->GetElement( "Encoding", OBJ_TYPE_NAME );
	if ( !objPtr )
	{
		objPtr = fontDict->GetElement( "Encoding", OBJ_TYPE_DICTIONARY );
		if ( !objPtr )
		{
			objPtr = fontDict->GetElement( "Encoding", OBJ_TYPE_ARRAY );
		}
	}
	if ( !objPtr )
	{
        mType = FONT_ENCODING_PDFDOC;
        mBaseType = FONT_ENCODING_PDFDOC;
        mpCodeTable = (HE_WCHAR*)gPdfDocEncoding;
		return;
	}

	CHE_ByteString	str;
	if ( objPtr->GetType() == OBJ_TYPE_NAME )
	{
		str = objPtr->GetNamePtr()->GetString();
		if ( str == "MacRomanEncoding" )
		{
			mType = FONT_ENCODING_MACROMAN;
            mBaseType = FONT_ENCODING_MACROMAN;
			mpCodeTable = (HE_WCHAR*)gMacRomanEncoding;
		}else if ( str == "MacExpertEncoding" )
		{
			mType = FONT_ENCODING_MACEXPERT;
            mBaseType = FONT_ENCODING_MACEXPERT;
			mpCodeTable = (HE_WCHAR*)gMacExpertEncoding;
		}else if ( str == "WinAnsiEncoding" )
		{
			mType = FONT_ENCODING_WINANSI;
            mBaseType = FONT_ENCODING_WINANSI;
			mpCodeTable = (HE_WCHAR*)gWinAnsiEncoding;
		}else if ( str == "StandardEncoding" )
		{
			mType = FONT_ENCODING_STANDARD;
            mBaseType = FONT_ENCODING_STANDARD;
			mpCodeTable = (HE_WCHAR*)gStandardEncoding;
		}else if ( str == "PdfZapfDingbatsEncoding" )
		{
			mType = FONT_ENCODING_ZAPFDINGBAT;
            mBaseType = FONT_ENCODING_ZAPFDINGBAT;
			mpCodeTable = (HE_WCHAR*)gPdfZapfDingbatsEncoding;
		}else if ( str == "SymbolEncoding" )
		{
			mType = FONT_ENCODING_SYMBOL;
            mBaseType = FONT_ENCODING_SYMBOL;
			mpCodeTable = (HE_WCHAR*)gSymbolEncoding;
		}else if ( str == "Identity-H" || str == "Identity-V" )
		{
			mType = FONT_ENCODING_IDENTITY;
            mBaseType = FONT_ENCODING_IDENTITY;
		}else{
			mType = FONT_ENCODING_BUILDINCMAP;
            mBaseType = FONT_ENCODING_BUILDINCMAP;
		}
	}else if ( objPtr->GetType() == OBJ_TYPE_DICTIONARY )
	{
        mType = FONT_ENCODING_CUSTOM;
		mBaseType = FONT_ENCODING_STANDARD;

		CHE_PDF_DictionaryPtr pEncodingDict = objPtr->GetDictPtr();
		objPtr = pEncodingDict->GetElement( "BaseEncoding", OBJ_TYPE_NAME );
		if ( objPtr )
		{
			str = objPtr->GetNamePtr()->GetString();
			if ( str == "MacRomanEncoding" )
			{
				mBaseType = FONT_ENCODING_MACROMAN;
			}else if ( str == "MacExpertEncoding" )
			{
				mBaseType = FONT_ENCODING_MACEXPERT;
			}else if ( str == "WinAnsiEncoding" )
			{
				mBaseType = FONT_ENCODING_WINANSI;
			}else if ( str == "StandardEncoding" )
			{
				mBaseType = FONT_ENCODING_STANDARD;
			}else if ( str == "PdfZapfDingbatsEncoding" )
			{
				mBaseType = FONT_ENCODING_ZAPFDINGBAT;
			}else if ( str == "SymbolEncoding" )
			{
				mBaseType = FONT_ENCODING_SYMBOL;
			}
		}else{
			mBaseType = FONT_ENCODING_STANDARD;
		}

		mpCodeTable = GetAllocator()->NewArray<HE_WCHAR>( 256 );
		for ( HE_ULONG i = 0; i < 256; ++i )
		{
			mpCodeTable[i] = 0;
		}
		mbCodeTableRelease = TRUE;

		switch ( mBaseType )
		{ 
		case FONT_ENCODING_PDFDOC:
			memcpy( (HE_LPVOID)mpCodeTable, gPdfDocEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		case FONT_ENCODING_STANDARD:
			memcpy( (HE_LPVOID)mpCodeTable, gStandardEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		case FONT_ENCODING_MACROMAN:
			memcpy( (HE_LPVOID)mpCodeTable, gMacRomanEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		case FONT_ENCODING_MACEXPERT:
			memcpy( (HE_LPVOID)mpCodeTable, gMacExpertEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		case FONT_ENCODING_WINANSI:
			memcpy( (HE_LPVOID)mpCodeTable, gWinAnsiEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		case FONT_ENCODING_ZAPFDINGBAT:
			memcpy( (HE_LPVOID)mpCodeTable, gPdfZapfDingbatsEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		case FONT_ENCODING_SYMBOL:
			memcpy( (HE_LPVOID)mpCodeTable, gSymbolEncoding, sizeof(HE_WCHAR) * 256 );
			break;
		default:
			break;
		}

		CHE_PDF_ArrayPtr pDifArray = pEncodingDict->GetElement( "Differences", OBJ_TYPE_ARRAY )->GetArrayPtr();
		if ( pDifArray )
		{
			HE_ULONG iCount = pDifArray->GetCount();
			HE_ULONG iIndex = 0;
			HE_BYTE tmpByte;
			CHE_PDF_ObjectPtr pObj;
//			if ( iCount > 0 )
//			{
//				mType = FONT_ENCODING_CUSTOM;
//			}
			for ( HE_ULONG i = 0; i < iCount; i++ )
			{
				pObj = pDifArray->GetElement( i );
				if ( pObj->GetType() == OBJ_TYPE_NUMBER )
				{
					iIndex = pObj->GetNumberPtr()->GetInteger();
				}else if ( pObj->GetType() == OBJ_TYPE_NAME )
				{
					CHE_ByteString strTmp = pObj->GetNamePtr()->GetString();
					if ( HE_GetCodeFromName( strTmp, mType, tmpByte ) )
					{
						*(mpCodeTable + iIndex) = tmpByte;
						iIndex++;
					}
				}
			}
		}
	}
}


CHE_PDF_Encoding::~CHE_PDF_Encoding()
{
	if ( mbCodeTableRelease && mpCodeTable )
	{
		GetAllocator()->DeleteArray<HE_WCHAR>( mpCodeTable );
		mpCodeTable = NULL;
	}
}


PDF_FONT_ENCODING CHE_PDF_Encoding::GetType() const
{
	return mType;
}

PDF_FONT_ENCODING CHE_PDF_Encoding::GetBaseType() const
{
    return mBaseType;
}

HE_BOOL CHE_PDF_Encoding::GetUnicode( HE_BYTE charCode, HE_WCHAR & codeRet ) const
{
	if ( mpCodeTable )
	{
		codeRet =  mpCodeTable[charCode];
		return TRUE;
	}
	return FALSE;
}


CHE_PDF_FontDescriptor::CHE_PDF_FontDescriptor( const CHE_PDF_DictionaryPtr & fontDesDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object( pAllocator ), mFlags( 0 ),  mItalicAngle( 0 ), mAscent( 0 ), mDescent( 0 ), 
	mCapHeight( 0 ), mXHeight( 0 ),mMissingWidth( 0 ), mEmbedded( FALSE ), mWMode( 0 )
{
	if ( fontDesDict )
	{
		CHE_PDF_ObjectPtr objPtr = fontDesDict->GetElement( "Flags", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mFlags = objPtr->GetNumberPtr()->GetInteger();
		}

		objPtr = fontDesDict->GetElement( "ItalicAngle", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mItalicAngle = objPtr->GetNumberPtr()->GetFloat();
		}

		objPtr = fontDesDict->GetElement( "Ascent", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mAscent = objPtr->GetNumberPtr()->GetFloat();
		}

		objPtr = fontDesDict->GetElement( "Descent", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mDescent = objPtr->GetNumberPtr()->GetFloat();
		}

		objPtr = fontDesDict->GetElement( "CapHeight", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mCapHeight = objPtr->GetNumberPtr()->GetFloat();
		}

		objPtr = fontDesDict->GetElement( "XHeight", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mXHeight = objPtr->GetNumberPtr()->GetFloat();
		}

		objPtr = fontDesDict->GetElement( "MissingWidth", OBJ_TYPE_NUMBER );
		if ( objPtr )
		{
			mMissingWidth = objPtr->GetNumberPtr()->GetFloat();
		}

		objPtr = fontDesDict->GetElement( "FontBBox", OBJ_TYPE_ARRAY );
		if ( objPtr )
		{
			CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
			if ( arrayPtr && arrayPtr->GetCount() >= 4 )
			{
				objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontBBox.left = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontBBox.bottom = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 2, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontBBox.width = objPtr->GetNumberPtr()->GetFloat() - mFontBBox.left;
				}
				objPtr = arrayPtr->GetElement( 3, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontBBox.height = objPtr->GetNumberPtr()->GetFloat() - mFontBBox.bottom;
				}
			}
		}

		objPtr = fontDesDict->GetElement( "FontFile", OBJ_TYPE_REFERENCE );
		if ( ! objPtr )
		{
			objPtr = fontDesDict->GetElement( "FontFile2", OBJ_TYPE_REFERENCE );
			if ( !objPtr )
			{
				objPtr = fontDesDict->GetElement( "FontFile3", OBJ_TYPE_REFERENCE );
			}
		}
		if ( objPtr )
		{
			mEmbedded = TRUE;
			mEmbedFont = objPtr->GetRefPtr();
		}

		//todo 获取字体的WMode 只有CIDFont才有可能WMode为1，也就是垂直显示
	}
}


CHE_PDF_FontDescriptor::~CHE_PDF_FontDescriptor()
{

}


HE_BOOL CHE_PDF_FontDescriptor::IsFixedPitch() const
{
	return ( (mFlags & 1) > 0 );
}


HE_BOOL CHE_PDF_FontDescriptor::IsSerif() const
{
	return ( (mFlags & 2) > 0 );
}


HE_BOOL CHE_PDF_FontDescriptor::IsSymbolic() const
{
	return ( (mFlags & 4) > 0 );
}


HE_BOOL CHE_PDF_FontDescriptor::IsScript() const
{
	return ( (mFlags & 8) > 0 );
}


HE_BOOL	CHE_PDF_FontDescriptor::IsNonsymbolic() const
{
	return ( (mFlags & 20) > 0 );
}


HE_BOOL	CHE_PDF_FontDescriptor::IsItalic() const
{
	return ( (mFlags & 40) > 0 );
}


HE_BOOL	CHE_PDF_FontDescriptor::IsAllCap() const
{
	return ( (mFlags & 10000) > 0 );
}


HE_BOOL	CHE_PDF_FontDescriptor::IsSmallCap() const
{
	return ( (mFlags & 20000) > 0 );
}


HE_BOOL	CHE_PDF_FontDescriptor::IsForceBold() const
{
	return ( (mFlags & 40000) > 0 );
}


CHE_PDF_Font * CHE_PDF_Font::Create( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator /*= NULL*/ )
{
	CHE_PDF_Font * pFont = NULL;
	
	if ( pAllocator == NULL )
	{
		pAllocator = GetDefaultAllocator();
	}

	if ( fontDict )
	{
		CHE_PDF_ObjectPtr objPtr = fontDict->GetElement( "Subtype", OBJ_TYPE_NAME );
		if ( objPtr )
		{
			CHE_ByteString str( pAllocator );
			str = objPtr->GetNamePtr()->GetString();
			if ( str == "Type0" )
			{
				pFont = pAllocator->New<CHE_PDF_Type0_Font>( fontDict, pAllocator );
			}
			else if ( str == "Type1"  )
			{
				pFont = pAllocator->New<CHE_PDF_Type1_Font>( fontDict, pAllocator );
			}
			else if ( str == "MMType1" )
			{
				pFont = pAllocator->New<CHE_PDF_MMType1_Font>( fontDict, pAllocator );
			}
			else if ( str == "Type3" )
			{
				pFont = pAllocator->New<CHE_PDF_Type3_Font>( fontDict, pAllocator );
			}
			else if ( str == "TrueType" )
			{
				pFont = pAllocator->New<CHE_PDF_TrueType_Font>( fontDict, pAllocator );
			}
		}
	}

	return pFont;
}

static int ft_char_index(FT_Face face, int cid)
{
	int gid = FT_Get_Char_Index(face, cid);
	if (gid == 0)
		gid = FT_Get_Char_Index(face, 0xf000 + cid);
    
	/* some chinese fonts only ship the similarly looking 0x2026 */
	if (gid == 0 && cid == 0x22ef)
		gid = FT_Get_Char_Index(face, 0x2026);
    
	return gid;
}


CHE_PDF_Font::CHE_PDF_Font( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_Object(pAllocator), mType(FONT_TYPE1), mBaseFont(pAllocator), mEncoding(fontDict, pAllocator), mFontDict(fontDict),
	mFace(NULL), mpToUnicodeMap(NULL), mpFontDescriptor(NULL), mpEmbeddedFontFile(NULL), mFontFileSize(0), mCIDTOGID(NULL), mCIDTOGIDLength(0), mPlatformFontInfo(NULL), mCleanCallBack(NULL)
{
	CHE_PDF_ObjectPtr objPtr = mFontDict->GetElement( "Subtype", OBJ_TYPE_NAME );
	if ( objPtr )
	{
		CHE_ByteString str( GetAllocator() );
		str = objPtr->GetNamePtr()->GetString();
		if ( str == "Type0" )
		{
			mType = FONT_TYPE0;
		}else if ( str == "Type1"  )
		{
			mType = FONT_TYPE1;
		}else if ( str == "MMType1" )
		{
			mType = FONT_MMTYPE1;
		}else if ( str == "Type3" )
		{
			mType = FONT_TYPE3;
		}else if ( str == "TrueType" )
		{
			mType = FONT_TRUETYPE;
		}
	}

	objPtr = mFontDict->GetElement( "BaseFont", OBJ_TYPE_NAME );
	if ( objPtr )
	{
		mBaseFont = objPtr->GetNamePtr()->GetString();;
	}else{
		objPtr = mFontDict->GetElement( "FontName", OBJ_TYPE_NAME );
		if ( objPtr )
		{
			mBaseFont = objPtr->GetNamePtr()->GetString();
		}
	}

	if ( mBaseFont == "\xCB\xCE\xCC\xE5" )
	{
		mBaseFont = "SimSun,Regular";
	}else if ( mBaseFont == "\xBA\xDA\xCC\xE5" )
	{
		mBaseFont = "SimHei,Regular";
	}else if ( mBaseFont == "\xBF\xAC\xCC\xE5_GB2312" )
	{
		mBaseFont = "SimKai,Regular";
			
	}else if ( mBaseFont == "\xB7\xC2\xCB\xCE_GB2312" )
	{
		mBaseFont = "SimFang,Regular";
			
	}else if ( mBaseFont == "\xC1\xA5\xCA\xE9" )
	{
		mBaseFont = "SimLi,Regular";
	}

	objPtr = mFontDict->GetElement( "ToUnicode", OBJ_TYPE_STREAM );
	if ( objPtr )
	{
		mpToUnicodeMap = GetToUnicodeMap( objPtr->GetStreamPtr() );
	}

	objPtr = mFontDict->GetElement( "FontDescriptor", OBJ_TYPE_DICTIONARY );
	if ( objPtr )
	{
		mFontDescriptorDict = objPtr->GetDictPtr();
		mpFontDescriptor = GetAllocator()->New<CHE_PDF_FontDescriptor>( mFontDescriptorDict, GetAllocator() );
	}else{
		//Type0 Font
		objPtr = mFontDict->GetElement( "DescendantFonts", OBJ_TYPE_ARRAY );
		if ( objPtr )
		{
			objPtr = objPtr->GetArrayPtr()->GetElement( 0, OBJ_TYPE_DICTIONARY );
			if ( objPtr )
			{
				objPtr = objPtr->GetDictPtr()->GetElement( "FontDescriptor", OBJ_TYPE_DICTIONARY );
				if ( objPtr )
				{
					mFontDescriptorDict = objPtr->GetDictPtr();
					mpFontDescriptor = GetAllocator()->New<CHE_PDF_FontDescriptor>( mFontDescriptorDict, GetAllocator() );
				}
			}
		}
		//No font descriptor dictionary
	}

	if ( mpFontDescriptor )
	{
		CHE_PDF_ReferencePtr refPtr = mpFontDescriptor->GetEmbeddedStream();
		if ( refPtr )
		{
			objPtr = refPtr->GetRefObj( OBJ_TYPE_STREAM );
			if ( objPtr )
			{
				CHE_PDF_StreamAcc stmAcc( GetAllocator() );
				CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
				stmAcc.Attach( stmPtr );
				if ( stmAcc.GetSize() > 0 )
				{
					mFontFileSize = stmAcc.GetSize();
					mpEmbeddedFontFile = GetAllocator()->NewArray<HE_BYTE>( mFontFileSize );
					memcpy( mpEmbeddedFontFile, stmAcc.GetData(), mFontFileSize );
				}
				stmAcc.Detach();
			}
		}

		if ( mpEmbeddedFontFile )
		{
			FT_Library ftlib = HE_GetFTLibrary();
			FT_New_Memory_Face( ftlib, mpEmbeddedFontFile, mFontFileSize, 0, (FT_Face*)&mFace );
		}
	}

	if ( mFace == NULL )
	{
		//type1 base 14 font
		if ( mType == FONT_TYPE1 || mType == FONT_MMTYPE1 )
		{
			HE_LPBYTE pBuf = NULL;
			HE_ULONG bufSize = 0;
			if ( HE_GetType1BaseFontFile( mBaseFont, pBuf, bufSize ) )
			{
				FT_Library ftlib = HE_GetFTLibrary();
				FT_New_Memory_Face( ftlib, pBuf, bufSize, 0, (FT_Face*)&mFace );
                mFontFileSize = bufSize;
                mpEmbeddedFontFile = pBuf;
			}else if ( mpFontDescriptor )
			{
				//对于无法通过baseFont获得对于的文件的，应该通过某种其他的规则获得替代字体
				if ( HE_GetType1BaseFontFile( *mpFontDescriptor, pBuf, bufSize ) )
				{
					FT_Library ftlib = HE_GetFTLibrary();
					FT_New_Memory_Face( ftlib, pBuf, bufSize, 0, (FT_Face*)&mFace );
				}
			}
		}

		if ( mFace == NULL && mBaseFont.GetLength() > 0 )
		{
			IHE_SystemFontMgr * pSystemFontMgr = HE_GetSystemFontMgr( GetAllocator() );
			if ( pSystemFontMgr )
			{
				CHE_ByteString filePath( pAllocator );
				filePath = pSystemFontMgr->GetFontFilePath( mBaseFont );
				if ( filePath.GetLength() == 0 )
				{

					//根据fontDescriptor的信息获取匹配字体的算法还有待完善
// 					HE_FLOAT ascent = 0;
// 					HE_FLOAT descent = 0;
// 					CHE_PDF_ObjectPtr objPtr = mFontDescriptorDict->GetElement( "Ascent", OBJ_TYPE_NUMBER );
// 					if ( objPtr )
// 					{
// 						ascent = objPtr->GetNumberPtr()->GetFloat();
// 					}
// 					objPtr = mFontDescriptorDict->GetElement( "Descent", OBJ_TYPE_NUMBER );
// 					if ( objPtr )
// 					{
// 						descent = objPtr->GetNumberPtr()->GetFloat();
// 					}
// 					filePath = pSystemFontMgr->GetFontFilePath( ascent, descent );
// 					if ( filePath.GetLength() == 0 )
// 					{					
//					}
                    
#ifdef _MAC_OS_X_
                    
                    if ( mType == FONT_TYPE0 )
                    {
                        filePath = "/Library/Fonts/Songti.ttc";
                        
                    }else{
                        filePath = "/System/Library/Fonts/Helvetica.dfont";
                    }
				
#endif
                
#ifdef WIN32
					filePath = pSystemFontMgr->GetFontFilePath( "微软雅黑" );
					if ( filePath.GetLength() == 0 )
					{
						filePath = pSystemFontMgr->GetFontFilePath( "宋体" );
						if ( filePath.GetLength() == 0 )
						{
							filePath = pSystemFontMgr->GetFontFilePath( "黑体" );
						}
					}
#endif

                }
                
				FT_Library ftlib = HE_GetFTLibrary();
				FT_New_Face( ftlib, filePath.GetData(), 0, (FT_Face*)&mFace );
                mFontPath = filePath;
			}
		}
	}
    
    
    if ( mType == FONT_TYPE0 )
    {
        return;
    }

	//处理一下字体的charmap
	FT_CharMap cmap = NULL;
	if ( mFace )
	{
		FT_Face ftface = (FT_Face)mFace;
		for ( HE_INT32 i = 0; i < ftface->num_charmaps; ++i )
		{
			FT_CharMap test = ftface->charmaps[i];
			if ( mType == FONT_TYPE1 )
			{
				if (test->platform_id == 7)
					cmap = test;
			}
			else if ( mType == FONT_TRUETYPE )
			{
				if ( test->platform_id == 1 && test->encoding_id == 0 )
					cmap = test;
				if ( test->platform_id == 3 && test->encoding_id == 1 )
					cmap = test;
				if ( mpFontDescriptor && mpFontDescriptor->IsSymbolic() && test->platform_id == 3 && test->encoding_id == 0 )
					cmap = test;
			}
		}
		if ( cmap )
		{
			FT_Set_Charmap( ftface, cmap );
		}
	}
    
    CHE_ByteString estrings[256];
    mCIDTOGID = GetAllocator()->NewArray<HE_WCHAR>( 256 );
    mCIDTOGIDLength = 256;
    if ( mFace )
    {
        for ( HE_INT32 i = 0; i < 256; ++i )
        {
            mCIDTOGID[i] = ft_char_index( (FT_Face)mFace, i );
        }
    }
    
    switch ( mEncoding.GetBaseType() )
    {
        case FONT_ENCODING_STANDARD:
            for ( HE_ULONG i = 0; i < 256; ++i )
            {
                estrings[i] = pdf_standard[i];
            }
            break;
        case FONT_ENCODING_WINANSI:
            for ( HE_ULONG i = 0; i < 256; ++i )
            {
                estrings[i] = pdf_win_ansi[i];
            }
            break;
        case FONT_ENCODING_MACROMAN:
            for ( HE_ULONG i = 0; i < 256; ++i )
            {
                estrings[i] = pdf_mac_roman[i];
            }
            break;
        case FONT_ENCODING_MACEXPERT:
            for ( HE_ULONG i = 0; i < 256; ++i )
            {
                estrings[i] = pdf_mac_expert[i];
            }
            break;
        default:break ;
    }
    
    CHE_PDF_DictionaryPtr EncodingDict;
    objPtr = fontDict->GetElement( "Encoding", OBJ_TYPE_DICTIONARY );
    if ( objPtr )
    {
        EncodingDict = objPtr->GetDictPtr();
        
        CHE_PDF_ArrayPtr pDifArray = EncodingDict->GetElement( "Differences", OBJ_TYPE_ARRAY )->GetArrayPtr();
        if ( pDifArray )
        {
            HE_ULONG iCount = pDifArray->GetCount();
            HE_ULONG iIndex = 0;
            
            CHE_PDF_ObjectPtr pObj;
            for ( HE_ULONG i = 0; i < iCount; i++ )
            {
                pObj = pDifArray->GetElement( i );
                if ( pObj->GetType() == OBJ_TYPE_NUMBER )
                {
                    iIndex = pObj->GetNumberPtr()->GetInteger();
                }else if ( pObj->GetType() == OBJ_TYPE_NAME )
                {
                    estrings[iIndex] = pObj->GetNamePtr()->GetString();
                    iIndex++;
                    // 							CHE_ByteString strTmp = pObj->GetNamePtr()->GetString();
                    // 							HE_ULONG gid = FT_Get_Name_Index( (FT_Face)mFace, (char *)strTmp.GetData() );
                    // 							if ( gid > 0 )
                    // 							{
                    // 								*(mEncoding.mpCodeTable + iIndex) = gid;
                    // 								iIndex++;
                    // 							}
                }
            }
        }
    }


	if ( mType == FONT_TYPE1 )
	{
        
        for ( HE_ULONG i = 0; i < 256; ++i )
        {
            if ( estrings[i].GetLength() > 0 )
            {
                mCIDTOGID[i] = FT_Get_Name_Index( (FT_Face)mFace, (char*)( estrings[i].GetData() ) );
                if ( mCIDTOGID[i] == 0 )
                {
                    int aglcode = pdf_lookup_agl( (char*)( estrings[i].GetData() ) );
                    const char **dupnames = pdf_lookup_agl_duplicates(aglcode);
                    while (*dupnames)
                    {
                        mCIDTOGID[i] = FT_Get_Name_Index( (FT_Face)mFace, (char*)*dupnames);
                        if (mCIDTOGID[i])
                            break;
                        dupnames++;
                    }
                }
            }
        }
	}
    
    /* encode by glyph name where we can */
    if ( mType == FONT_TRUETYPE )
    {
        /* Unicode cmap */
        if ( mpFontDescriptor && !mpFontDescriptor->IsSymbolic() && ((FT_Face)mFace)->charmap && ((FT_Face)mFace)->charmap->platform_id == 3)
        {
            for ( HE_ULONG i = 0; i < 256; ++i )
            {
                if ( estrings[i].GetData() )
                {
                    int aglcode = pdf_lookup_agl( (char*)(estrings[i].GetData()));
                    if (!aglcode)
                        mCIDTOGID[i] = FT_Get_Name_Index(((FT_Face)mFace), (char*)(estrings[i].GetData()));
                    //else
                        //mCIDTOGID[i] = ft_char_index(((FT_Face)mFace), aglcode);
                }
            }
        }
        
        /* MacRoman cmap */
        //else if (mpFontDescriptor && !mpFontDescriptor->IsSymbolic() && ((FT_Face)mFace)->charmap->platform_id == 1)
        //{
        //    for ( HE_ULONG i = 0; i < 256; i++ )
        //    {
        //        if (estrings[i].GetData())
        //        {
        //            int k = lookup_mre_code((char*)(estrings[i].GetData()));
        //            if (k <= 0)
        //                mCIDTOGID[i] = FT_Get_Name_Index(((FT_Face)mFace), (char*)(estrings[i].GetData()));
        //            //else
        //            //    mCIDTOGID[i] = ft_char_index(((FT_Face)mFace), k);
        //        }
        //    }
        //}
        
        /* Symbolic cmap */
        else if (!((FT_Face)mFace)->charmap || ((FT_Face)mFace)->charmap->encoding != FT_ENCODING_MS_SYMBOL)
        {
            for ( HE_ULONG i = 0; i < 256; i++)
            {
                if (estrings[i].GetData())
                {
                    mCIDTOGID[i] = FT_Get_Name_Index(((FT_Face)mFace), (char*)(estrings[i].GetData()));
                    //if (mCIDTOGID[i] == 0)
                    //    mCIDTOGID[i] = ft_char_index(((FT_Face)mFace), i);
                }
            }
        }
    }
    
    
    /* try to reverse the glyph names from the builtin encoding */
    for ( HE_ULONG i = 0; i < 256; i++)
    {
        if ( mCIDTOGID[i] && estrings[i].GetData() == NULL )
        {
            if (FT_HAS_GLYPH_NAMES(((FT_Face)mFace)))
            {
                //FT_Get_Glyph_Name(((FT_Face)mFace), mCIDTOGID[i], ebuffer[i], 32);
                //if (fterr)
                //    fz_warn(ctx, "freetype get glyph name (gid %d): %s", etable[i], ft_error_string(fterr));
                //if (ebuffer[i][0])
                //    estrings[i] = ebuffer[i];
            }
            else
            {
                estrings[i] = (char*) pdf_win_ansi[i]; /* discard const */
            }
        }
    }
    
    /* symbolic Type 1 fonts with an implicit encoding and non-standard glyph names */
    if ( mType == FONT_TYPE1 && mpFontDescriptor && mpFontDescriptor->IsSymbolic() )
    {
        for ( HE_ULONG i = 0; i < 256; i++)
            if (mCIDTOGID[i] && estrings[i].GetData() && !pdf_lookup_agl( (char*)(estrings[i].GetData())))
                estrings[i] = (char*) pdf_standard[i];
    }
}


CHE_PDF_Font::~CHE_PDF_Font()
{
	if ( mpFontDescriptor )
	{
		mpFontDescriptor->GetAllocator()->Delete<CHE_PDF_FontDescriptor>( mpFontDescriptor );
		mpFontDescriptor = NULL;
	}
	if ( mpToUnicodeMap )
	{
		mpToUnicodeMap->GetAllocator()->Delete<CHE_NumToPtrMap>( mpToUnicodeMap );
		mpToUnicodeMap = NULL;
	}
	if ( mFace )
	{
		FT_Done_Face( (FT_Face)mFace );
		mFace = NULL;
	}
	if ( mpEmbeddedFontFile )
	{
		GetAllocator()->DeleteArray<HE_BYTE>( mpEmbeddedFontFile );
		mpEmbeddedFontFile = NULL;
	}
    if ( mCleanCallBack && mPlatformFontInfo )
    {
        mCleanCallBack( mPlatformFontInfo );
    }
}


PDF_FONT_TYPE CHE_PDF_Font::GetType() const
{
	return mType;
}


CHE_ByteString CHE_PDF_Font::GetBaseFont() const
{
	return mBaseFont;
}


PDF_FONT_ENCODING CHE_PDF_Font::GetEncodingType() const
{
	return mEncoding.GetType();
}


CHE_PDF_DictionaryPtr CHE_PDF_Font::GetFontDictPtr() const
{
	return mFontDict;
}


CHE_PDF_DictionaryPtr CHE_PDF_Font::GetFontDescriptorDictPtr() const
{
	return mFontDescriptorDict;
}


HE_VOID* CHE_PDF_Font::GetFTFace()
{
	return mFace;
}

HE_ULONG CHE_PDF_Font::GetFTFaceGlyphCount() const
{
    if ( mFace )
    {
        return ((FT_Face)mFace)->num_glyphs;
    }
    return 0;
}

CHE_ByteString CHE_PDF_Font::GetGlyphNameForStandard( HE_WCHAR id ) const
{
    CHE_ByteString strRet;
    if ( id <= 255 && id >= 0 )
    {
        strRet = pdf_standard[id];
    }
    return strRet;
}

HE_WCHAR CHE_PDF_Font::GetGlyphIdForStrandard( HE_WCHAR id ) const
{
    CHE_ByteString strRet;
    if ( id <= 255 && id >= 0 && mFace )
    {
        strRet = pdf_standard[id];
        if ( strRet.GetLength() > 0 )
        {
            HE_WCHAR gid = FT_Get_Name_Index( (FT_Face)mFace, (char*)( strRet.GetData() ) );
            if ( gid == 0 )
            {
                int aglcode = pdf_lookup_agl( (char*)( strRet.GetData() ) );
                const char **dupnames = pdf_lookup_agl_duplicates(aglcode);
                while (*dupnames)
                {
                    gid = FT_Get_Name_Index( (FT_Face)mFace, (char*)*dupnames);
                    if (gid)
                        break;
                    dupnames++;
                }
            }
            return gid;
        }
    }
    return 0;
}

HE_BOOL CHE_PDF_Font::GetGlyphId( HE_WCHAR charCode, HE_ULONG & codeRet ) const
{
	if ( mFace == NULL )
	{
		return FALSE;
	}
    
    //for simple font: mCIDTOGID means charcode to gid
    //for composite font: mCIDTOGID means cid to gid
    if ( mCIDTOGID )
	{
		if ( charCode < mCIDTOGIDLength )
		{
			codeRet = mCIDTOGID[charCode];
			if ( codeRet != 0 )
            {
                return TRUE;
            }
		}
	}

    switch ( mEncoding.mType )
    {
    case FONT_ENCODING_NONE:
        return FALSE;
    case FONT_ENCODING_STANDARD:
    case FONT_ENCODING_PDFDOC:
    case FONT_ENCODING_WINANSI:
    case FONT_ENCODING_MACROMAN:
    case FONT_ENCODING_MACEXPERT:
    case FONT_ENCODING_SYMBOL:
    case FONT_ENCODING_ZAPFDINGBAT:
    case FONT_ENCODING_CUSTOM:
        mEncoding.GetUnicode( (HE_BYTE)charCode, charCode );
        break;
    case FONT_ENCODING_IDENTITY:
        {
            if ( ! GetUnicode( charCode, charCode ) )
            {
                codeRet = charCode;
                return FALSE;
            }
        }
        return TRUE;
    case FONT_ENCODING_BUILDINCMAP:
        break;
    default:
        return FALSE;
    }
	
	FT_Face ftface = (FT_Face)mFace;
	codeRet = FT_Get_Char_Index( ftface, charCode );
	if ( codeRet == 0 )
	{
		codeRet = FT_Get_Char_Index( ftface, 0xf000 + charCode );
		/* some chinese fonts only ship the similarly looking 0x2026 */
		if ( codeRet == 0 && charCode == 0x22ef )
			codeRet = FT_Get_Char_Index( ftface, 0x2026 );
	}

	if ( codeRet == 0 )
	{
		return FALSE;
	}
	return TRUE;
}


CHE_NumToPtrMap	* CHE_PDF_Font::GetToUnicodeMap( const CHE_PDF_StreamPtr & pToUnicodeStream )
{
	if ( ! pToUnicodeStream )
	{
		return NULL;
	}

	CHE_PDF_StreamAcc stmAcc( GetAllocator() );
	stmAcc.Attach( pToUnicodeStream );
	if ( stmAcc.GetSize() == 0 )
	{
		stmAcc.Detach();
		return NULL;
	}

	CHE_NumToPtrMap * tmpMap = GetAllocator()->New<CHE_NumToPtrMap>( GetAllocator() );
	IHE_Read * pFileRead = HE_CreateMemBufRead( (HE_BYTE*)(stmAcc.GetData()), stmAcc.GetSize(), GetAllocator() );
	CHE_PDF_SyntaxParser parser( NULL, GetAllocator() );
	CHE_PDF_ParseWordDes wordDes( GetAllocator() );
	HE_ULONG lMaxIndex = 0;
	HE_ULONG lCodeCount = 0;
	parser.InitParser( pFileRead );
	while ( parser.GetWord( wordDes ) )
	{
		if ( wordDes.type == PARSE_WORD_INTEGER )
		{
			HE_ULONG lCount = wordDes.str.GetInteger();
			if ( parser.GetWord( wordDes ) == FALSE )
			{
				break;
			}
			HE_ULONG lIndex = 0;

			if ( wordDes.str == "beginbfchar" )
			{
				for ( HE_ULONG i = 0; i < lCount; i++ )
				{
					parser.GetWord( wordDes );
					lIndex = HE_HexStrToValue( wordDes.str );
					if ( lIndex > lMaxIndex )
					{
						lMaxIndex = lIndex;
					}
					parser.GetWord( wordDes );
					tmpMap->Append( lIndex, (HE_LPVOID)HE_HexStrToValue( wordDes.str ) );
					lCodeCount++;
				}
			}else if ( wordDes.str == "beginbfrange" )
			{
				for ( HE_ULONG j = 0; j < lCount; j++ )
				{
					HE_ULONG lIndexEnd = 0;
					HE_ULONG tmpValue = 0;
					HE_ULONG offset = 0;
					parser.GetWord( wordDes );
					lIndex = HE_HexStrToValue( wordDes.str );
					parser.GetWord( wordDes );
					lIndexEnd = HE_HexStrToValue( wordDes.str );
					offset = parser.GetPos();
					parser.GetWord( wordDes );
					if ( wordDes.type == PARSE_WORD_ARRAY_B )
					{
						parser.SetPos( offset );
						CHE_PDF_ArrayPtr pArray = parser.GetArrayPtr();
						if ( ! pArray )
						{
							continue;
						}
						CHE_PDF_StringPtr pStrObj;
						if ( lIndexEnd > lMaxIndex )
						{
							lMaxIndex = lIndexEnd;
						}
						for ( HE_ULONG i = lIndex ; i <= lIndexEnd; i++ )
						{
							pStrObj = pArray->GetElement( i - lIndex )->GetStringPtr();
							if ( ! pStrObj || pStrObj->GetType() != OBJ_TYPE_STRING )
							{
								continue;
							}
							CHE_ByteString strTmp = pStrObj->GetString();
							tmpValue = HE_HexStrToValue( strTmp );
							tmpMap->Append( i, (HE_LPVOID)tmpValue );
							lCodeCount++;
						}
					}else if ( wordDes.type == PARSE_WORD_STRING )
					{
						tmpValue = HE_HexStrToValue( wordDes.str );
						if ( lIndexEnd > lMaxIndex )
						{
							lMaxIndex = lIndexEnd;
						}
						for ( HE_ULONG i = lIndex ; i <= lIndexEnd; i++ )
						{
							tmpMap->Append( i, (HE_LPVOID)tmpValue );
							lCodeCount++;
							tmpValue++;
						}
					}
				}
			}
		}
	}
	pFileRead->Release();
	HE_DestoryIHERead( pFileRead );
	stmAcc.Detach();
	return tmpMap;
}


HE_ULONG CHE_PDF_Font::GetWMode() const
{
	if ( mpFontDescriptor )
	{
		return mpFontDescriptor->GetWMode();
	}
	return 0;
}

HE_VOID CHE_PDF_Font::Lock()
{
	mLock.Lock();
}

HE_VOID	CHE_PDF_Font::UnLock()
{
	mLock.UnLock();
}


CHE_PDF_Type0_Font::CHE_PDF_Type0_Font( const CHE_PDF_DictionaryPtr & fontDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_PDF_Font( fontDict, pAllocator ), mpCIDMap( NULL ), mpUnicodeMap( NULL )
{
	if ( mFontDict )
	{
		CHE_PDF_ObjectPtr objPtr;

		if ( GetEncodingType() == FONT_ENCODING_BUILDINCMAP )
		{
			objPtr = mFontDict->GetElement( "Encoding", OBJ_TYPE_NAME );
			if ( objPtr )
			{
				CHE_ByteString str( pAllocator );
				str = objPtr->GetNamePtr()->GetString();
				mpCIDMap = CHE_PDF_CMap::LoadBuildinCMap( str, GetAllocator() );
			}
		}

		objPtr = mFontDict->GetElement( "DescendantFonts", OBJ_TYPE_ARRAY );
		if ( objPtr )
		{
			objPtr = objPtr->GetArrayPtr()->GetElement( 0, OBJ_TYPE_DICTIONARY );
			if ( objPtr )
			{
				mDescdtFontDict = objPtr->GetDictPtr();
			}
		}
		if ( mDescdtFontDict )
		{
            objPtr = mDescdtFontDict->GetElement( "CIDToGIDMap", OBJ_TYPE_REFERENCE );
            if ( objPtr )
            {
                objPtr = objPtr->GetRefPtr()->GetRefObj( OBJ_TYPE_STREAM );
                if ( objPtr )
                {
                    CHE_PDF_StreamPtr stmPtr = objPtr->GetStreamPtr();
                    CHE_PDF_StreamAcc stmAcc;
                    if ( stmAcc.Attach( stmPtr ) )
                    {
                        mCIDTOGIDLength = stmAcc.GetSize() / 2;
                        mCIDTOGID = GetAllocator()->NewArray<HE_WCHAR>( mCIDTOGIDLength );
                        for ( size_t i = 0 ; i < mCIDTOGIDLength; ++i )
                        {
                            mCIDTOGID[i] = ( stmAcc.GetData()[i*2] << 8 ) + stmAcc.GetData()[i*2+1];
                        }
                    }
                }
            }
            
			objPtr = mDescdtFontDict->GetElement( "CIDSystemInfo", OBJ_TYPE_DICTIONARY );
			if ( objPtr )
			{
				CHE_PDF_DictionaryPtr pCIDSystemInfoDict = objPtr->GetDictPtr();
				objPtr = pCIDSystemInfoDict->GetElement( "Registry", OBJ_TYPE_STRING );
				if ( objPtr )
				{
					CHE_PDF_StringPtr pSt = objPtr->GetStringPtr();
					CHE_ByteString cmapNuame( pAllocator );
					cmapNuame = pSt->GetString();
					cmapNuame += "-";
					objPtr = pCIDSystemInfoDict->GetElement( "Ordering", OBJ_TYPE_STRING );
					if ( objPtr )
					{
						pSt = objPtr->GetStringPtr();
						cmapNuame += pSt->GetString();
						if ( cmapNuame == "Adobe-CNS1" )
						{
							cmapNuame = "Adobe-CNS1-UCS2";
						}else if ( cmapNuame == "Adobe-GB1" )
						{
							cmapNuame = "Adobe-GB1-UCS2";
						}else if ( cmapNuame == "Adobe-Japan1" )
						{
							cmapNuame = "Adobe-Japan1-UCS2";
						}else if ( cmapNuame == "Adobe-Japan2" )
						{
							cmapNuame = "Adobe-Japan2-UCS2";
						}else if ( cmapNuame == "Adobe-Korea1" )
						{
							cmapNuame = "Adobe-Korea1-UCS2";
						}
						mpUnicodeMap = CHE_PDF_CMap::LoadBuildinCMap( cmapNuame, GetAllocator() );
					}
				}
			}
		}
	}	
}


HE_BOOL CHE_PDF_Type0_Font::HandleEncoding( HE_WCHAR charCode, HE_WCHAR & ucs, HE_ULONG & gid, HE_ULONG & cid ) const
{
    ucs = 0;
    gid = 0;
    cid = 0;
    switch ( mEncoding.GetType() )
    {
    case FONT_ENCODING_NONE:
        {
            gid = charCode;
            GetUnicode( charCode, ucs );
            break;
        }
    case FONT_ENCODING_BUILDINCMAP:
        {
            if ( GetCID( charCode, cid ) )
            {
                if ( ! GetUnicode( cid, ucs ) )
                {
                    GetGlyphId( charCode, gid );
                }else{
                    if ( ! GetGlyphId( ucs, gid ) )
                    {
                        GetGlyphId( charCode, gid );
                    }
                }
            }else{
                GetUnicode( charCode, ucs );
                if ( ucs == 0 )
                {
                    ucs = charCode;
                }
                GetGlyphId( charCode, gid );
                if ( gid == 0 )
                {
                    GetGlyphId( ucs, gid );
                    if ( gid == 0 )
                    {
                        gid = charCode;
                    }
                }
            }
            break;
        }
    case FONT_ENCODING_IDENTITY:
        {
            if ( GetCID( charCode, cid ) )
            {
                GetGlyphId( cid, gid );
                if ( GetUnicode( charCode, ucs ) )
                {
                    if ( ! GetGlyphId( ucs, gid ) )
                    {
                        gid = charCode;
                    }
                }else{
                   //error
                }
            }else{
                gid = charCode;
                GetUnicode( charCode, ucs );
            }
            break;
        }
    default:
        {
            GetUnicode( charCode, ucs );
            GetCID( charCode, cid );
            if ( ! GetGlyphId( charCode, gid ) )
            {
                if ( ! GetGlyphId( ucs, gid ) )
                {
                    GetGlyphId( cid, gid );
                }
            }
            if ( gid == 0 )
            {
                gid = charCode;
            }
        }
        break;
    }
    
    return TRUE;
}


HE_BOOL	CHE_PDF_Type0_Font::GetUnicode( HE_WCHAR charCode, HE_WCHAR & codeRet ) const
{
	if ( mpUnicodeMap )
	{
		HE_ULONG tmpCode = 0;
		if ( mpUnicodeMap->LookupCode( charCode, tmpCode ) )
		{
			codeRet = (HE_WCHAR)( tmpCode );
			return TRUE;
		}
		return FALSE;
	}
	if ( mpToUnicodeMap )
	{
		codeRet = (HE_ULONG)( mpToUnicodeMap->GetItem( charCode ) );
		return TRUE;
	}
	return FALSE;
}


HE_FLOAT CHE_PDF_Type0_Font::GetWidth( const CHE_PDF_TextItem & item, const CHE_Matrix & matrix /*= CHE_Matrix()*/ ) const
{
	CHE_Matrix tmpMatrix;
	tmpMatrix.a = 0;
	tmpMatrix.b = 0;
	tmpMatrix.c = 0;
	tmpMatrix.d = 0;
	tmpMatrix.e = 0;
	tmpMatrix.f = 0;

	HE_ULONG qureyVal = 0;
	if ( GetEncodingType() == FONT_ENCODING_NONE )
	{
		qureyVal = item.charCode;
	}else if ( GetEncodingType() == FONT_ENCODING_IDENTITY )
	{
		qureyVal = item.charCode;
	}else{
		qureyVal = item.cid;
	}

	//这里去读取W数据，可以优化一下
	if ( mFontDict )
	{
		CHE_PDF_ObjectPtr objPtr = mFontDict->GetElement( "DescendantFonts", OBJ_TYPE_ARRAY );
		if ( objPtr && objPtr->GetArrayPtr()->GetCount() > 0 )
		{
			objPtr = objPtr->GetArrayPtr()->GetElement( 0, OBJ_TYPE_DICTIONARY );
			if ( objPtr )
			{
				CHE_PDF_DictionaryPtr dictPtr = objPtr->GetDictPtr();
				objPtr = dictPtr->GetElement( "W", OBJ_TYPE_ARRAY );
				if ( objPtr )
				{
					HE_BOOL bIndex = FALSE;
					HE_ULONG indexStart = 0;
					HE_ULONG indexEnd = 0;
					CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
					CHE_PDF_ArrayPtr tmpArrayPtr;
					for ( HE_ULONG i = 0; i < arrayPtr->GetCount(); ++i )
					{
						if ( bIndex == FALSE )
						{
							objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
							if ( objPtr )
							{
								indexStart = objPtr->GetNumberPtr()->GetInteger();
								indexEnd = 0;
								bIndex = TRUE;
							}
						}else{
							objPtr = arrayPtr->GetElement( i, OBJ_TYPE_ARRAY );
							if ( objPtr )
							{
								tmpArrayPtr = objPtr->GetArrayPtr();
								if ( qureyVal >= indexStart && qureyVal < indexStart + tmpArrayPtr->GetCount() )
								{
									objPtr = tmpArrayPtr->GetElement( qureyVal - indexStart, OBJ_TYPE_NUMBER );
									if ( objPtr )
									{
										return objPtr->GetNumberPtr()->GetFloat() / 1000;
									}
								}
								bIndex = FALSE;
							}else{
								objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
								indexEnd = objPtr->GetNumberPtr()->GetInteger();
								if ( qureyVal >= indexStart && qureyVal <= indexEnd )
								{
									objPtr = arrayPtr->GetElement( ++i, OBJ_TYPE_NUMBER );
									if ( objPtr )
									{
										return objPtr->GetNumberPtr()->GetFloat() / 1000;
									}
								}
								++i;
								bIndex = FALSE;
							}
						}
					}
				}else{
					objPtr = dictPtr->GetElement( "DW", OBJ_TYPE_NUMBER );
					if ( objPtr )
					{
						return objPtr->GetNumberPtr()->GetFloat() / 1000;
					}else{
						return 1.0;
					}
				}
			}
		}
	}

// 	if ( mFace )
// 	{
// 		FT_Set_Transform( mFace, NULL, NULL );
// 		FT_Error err = FT_Load_Glyph( mFace, item.gid, FT_LOAD_NO_SCALE );
// 		if ( err == 0 )
// 		{
// 			tmpMatrix.a = mFace->glyph->advance.x;
// 			tmpMatrix.d = mFace->glyph->advance.y;
// 		}
// 
// 		tmpMatrix.Concat( matrix );
// 
// 		return tmpMatrix.a * 1.0 / mFace->units_per_EM;
// 	}
	return 1;
}

HE_BOOL CHE_PDF_Type0_Font::IsCode( HE_ULONG cpt, HE_BYTE byteCount )
{
	if ( mpCIDMap )
	{
		return mpCIDMap->IsCode( cpt, byteCount );
	}
	return FALSE;
}

HE_BOOL CHE_PDF_Type0_Font::GetCID( HE_WCHAR charCode, HE_ULONG & codeRet ) const
{
	if ( mpCIDMap )
	{
		if ( mpCIDMap->LookupCode( charCode, codeRet ) )
		{
			return TRUE;
		}
		HE_ULONG gid = FT_Get_Char_Index( (FT_Face)mFace, codeRet );
		if ( gid == 0 )
		{
			gid = FT_Get_Char_Index( (FT_Face)mFace, 0xf000 + codeRet );

			/* some chinese fonts only ship the similarly looking 0x2026 */
			if ( gid == 0 && gid == 0x22ef )
			{
				gid = FT_Get_Char_Index( (FT_Face)mFace, 0x2026 );
			}
			if ( gid != 0 )
			{
				codeRet = gid;
				return TRUE;
			}
		}
	}
	return FALSE;
}


CHE_PDF_Type0_Font::~CHE_PDF_Type0_Font()
{
	if ( mpCIDMap )
	{
		mpCIDMap->GetAllocator()->Delete<CHE_PDF_CMap>( mpCIDMap );
	}
	if ( mpUnicodeMap )
	{
		mpUnicodeMap->GetAllocator()->Delete<CHE_PDF_CMap>( mpUnicodeMap );
	}
}


CHE_PDF_Type1_Font::CHE_PDF_Type1_Font( const CHE_PDF_DictionaryPtr & pFontDcit, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_PDF_Font( pFontDcit ), mFirstChar( 0 ), mLastChar( 255 )
{
	CHE_PDF_ObjectPtr objPtr = mFontDict->GetElement( "FirstChar", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
		mFirstChar = objPtr->GetNumberPtr()->GetInteger();
	}

	objPtr = mFontDict->GetElement( "LastChar", OBJ_TYPE_NUMBER );
	if ( objPtr )
	{
 		mLastChar = objPtr->GetNumberPtr()->GetInteger();
	}

	for ( HE_ULONG i = 0; i < 255; ++i )
	{
		mCharWidths[i] = 0;
	}

	objPtr = mFontDict->GetElement( "Widths", OBJ_TYPE_ARRAY );
	if ( objPtr )
	{
		CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
		if ( arrayPtr )
		{
			for ( HE_ULONG i = 0; i < arrayPtr->GetCount(); ++i )
			{
				objPtr = arrayPtr->GetElement( i, OBJ_TYPE_NUMBER );
				if ( objPtr && mFirstChar-1+i > 0 && mFirstChar-1+i  < 256 )
				{
					mCharWidths[mFirstChar-1+i] = objPtr->GetNumberPtr()->GetInteger();
				}
			}
		}
	}
}


CHE_PDF_Type1_Font::~CHE_PDF_Type1_Font()
{
}


HE_BOOL CHE_PDF_Type1_Font::HandleEncoding( HE_WCHAR charCode, HE_WCHAR & ucs, HE_ULONG & gid, HE_ULONG & cid ) const
{
    cid = 0;
    ucs = 0;
    gid = 0;
    
    HE_BOOL bUCSGet = GetUnicode( charCode, ucs );
    if ( ! bUCSGet )
    {
        ucs = charCode;
    }
    
    //for simple font: mCIDTOGID means charcode to gid
    if ( mCIDTOGID )
	{
		if ( charCode < mCIDTOGIDLength )
		{
			gid = mCIDTOGID[charCode];
			if ( gid != 0 )
            {
                return TRUE;
            }
		}
	}
    
    if ( mFace )
	{
        FT_Face ftface = (FT_Face)mFace;
        gid = FT_Get_Char_Index( ftface, ucs );
        //if ( gid == 0 )
        //{
        //    gid = FT_Get_Char_Index( ftface, 0xf000 + ucs );
        //    /* some chinese fonts only ship the similarly looking 0x2026 */
        //    if ( gid == 0 && ucs == 0x22ef )
        //        gid = FT_Get_Char_Index( ftface, 0x2026 );
        //}
	}
    
    if ( gid == 0 )
    {
        gid = charCode;
    }
    
	return TRUE;
}


HE_BOOL	CHE_PDF_Type1_Font::GetUnicode( HE_WCHAR charCode, HE_WCHAR & codeRet ) const
{
	if (    mEncoding.GetType() != FONT_ENCODING_NONE &&
            mEncoding.GetType() != FONT_ENCODING_IDENTITY &&
            mEncoding.GetType() != FONT_ENCODING_BUILDINCMAP )
	{
		if ( mEncoding.GetUnicode( (HE_BYTE)charCode, codeRet ) )
		{
			return TRUE;
		}
	}

	if ( mpToUnicodeMap )
	{
		HE_LPVOID pRet = mpToUnicodeMap->GetItem( charCode );
		HE_ULONG dwRet = (HE_ULONG)( pRet );
		codeRet = (HE_WCHAR)( dwRet );
		return TRUE;
	}

	return FALSE;
}


HE_FLOAT CHE_PDF_Type1_Font::GetWidth( const CHE_PDF_TextItem & item, const CHE_Matrix & matrix /*= CHE_Matrix()*/ ) const
{
	CHE_Matrix tmpMatrix;
	tmpMatrix.a = 0;
	tmpMatrix.b = 0;
	tmpMatrix.c = 0;
	tmpMatrix.d = 0;
	tmpMatrix.e = 0;
	tmpMatrix.f = 0;

	if ( item.charCode > 0 && item.charCode < 256 )
	{
		tmpMatrix.a = mCharWidths[item.charCode-1];
		tmpMatrix.d = tmpMatrix.a;
		if ( ( tmpMatrix.a - 0 ) >= FLT_EPSILON )
		{
			tmpMatrix.Concat( matrix );
			return tmpMatrix.a / 1000.0f;
		}
	}
	if ( mFace )
	{
		FT_Face ftface = (FT_Face)mFace;
		mLock.Lock();
		FT_Set_Transform( ftface, NULL, NULL );
		FT_Error err = FT_Load_Glyph( ftface, item.gid, FT_LOAD_NO_SCALE );
		if ( err == 0 )
		{
			tmpMatrix.a = ftface->glyph->advance.x;
			tmpMatrix.d = ftface->glyph->advance.y;

			tmpMatrix.Concat( matrix );

			mLock.UnLock();

			return tmpMatrix.a * 1.0f / ftface->units_per_EM;
		}
		mLock.UnLock();
	}

	return 0;
}


CHE_PDF_MMType1_Font::CHE_PDF_MMType1_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_PDF_Type1_Font( pFontDict, pAllocator )
{
}


CHE_PDF_MMType1_Font::~CHE_PDF_MMType1_Font()
{
}


CHE_PDF_TrueType_Font::CHE_PDF_TrueType_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_PDF_Type1_Font( pFontDict, pAllocator )
{
}


CHE_PDF_TrueType_Font::~CHE_PDF_TrueType_Font()
{
}


CHE_PDF_Type3_Font::CHE_PDF_Type3_Font( const CHE_PDF_DictionaryPtr & pFontDict, CHE_Allocator * pAllocator /*= NULL*/ )
	: CHE_PDF_Type1_Font( pFontDict, pAllocator )
{
	if ( pFontDict )
	{
		CHE_PDF_ObjectPtr objPtr = pFontDict->GetElement( "FontBBox", OBJ_TYPE_ARRAY );
		if ( objPtr )
		{
			mFontBBox = objPtr->GetArrayPtr();
		}
		objPtr = pFontDict->GetElement( "FontMatrix", OBJ_TYPE_ARRAY );
		if ( objPtr )
		{
			CHE_PDF_ArrayPtr arrayPtr = objPtr->GetArrayPtr();
			if ( arrayPtr && arrayPtr->GetCount() >= 6 )
			{
				objPtr = arrayPtr->GetElement( 0, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontMatrix.a = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 1, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontMatrix.b = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 2, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontMatrix.c = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 3, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontMatrix.d = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 4, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontMatrix.e = objPtr->GetNumberPtr()->GetFloat();
				}
				objPtr = arrayPtr->GetElement( 5, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					mFontMatrix.f = objPtr->GetNumberPtr()->GetFloat();
				}
			}
		}
		objPtr = pFontDict->GetElement( "Resources", OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			mResDict = objPtr->GetDictPtr();
		}

		objPtr = pFontDict->GetElement( "CharProcs", OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			mCharProcsDict = objPtr->GetDictPtr();
		}

		if ( ! mCharProcsDict )
		{
			return;
		}
		
		CHE_PDF_ArrayPtr encodingDiffArray;
		objPtr = pFontDict->GetElement( "Encoding", OBJ_TYPE_DICTIONARY );
		if ( objPtr )
		{
			objPtr = objPtr->GetDictPtr()->GetElement( "Differences", OBJ_TYPE_ARRAY );
			if ( objPtr )
			{
				encodingDiffArray = objPtr->GetArrayPtr();
			}
		}
		if ( encodingDiffArray )
		{
			HE_BYTE code = 0;
			for ( HE_ULONG i = 0; i < encodingDiffArray->GetCount(); ++i )
			{
				objPtr = encodingDiffArray->GetElement( i, OBJ_TYPE_NUMBER );
				if ( objPtr )
				{
					code = (HE_BYTE)( objPtr->GetNumberPtr()->GetInteger() );
				}else{
					objPtr = encodingDiffArray->GetElement( i, OBJ_TYPE_NAME );
					if ( objPtr )
					{
						objPtr = mCharProcsDict->GetElement( objPtr->GetNamePtr()->GetString(), OBJ_TYPE_STREAM );
						if ( objPtr && code >= 0 && code <= 255 )
						{
							mCharProcsSet[code-1] = objPtr->GetStreamPtr();
						}
					}
					++code;
				}
			}
		}
	}
}


CHE_PDF_Type3_Font::~CHE_PDF_Type3_Font()
{
}


CHE_PDF_ArrayPtr CHE_PDF_Type3_Font::GetFontBBox() const
{
	return mFontBBox;
}


CHE_Matrix CHE_PDF_Type3_Font::GetFontMatrix() const
{
	return mFontMatrix;
}


CHE_PDF_DictionaryPtr CHE_PDF_Type3_Font::GetResDict() const
{
	return mResDict;
}


CHE_PDF_StreamPtr CHE_PDF_Type3_Font::GetCharProc( HE_BYTE index ) const
{
	if ( index >= mFirstChar && index <= mLastChar )
	{
		return mCharProcsSet[index];
	}
	return CHE_PDF_StreamPtr();
}