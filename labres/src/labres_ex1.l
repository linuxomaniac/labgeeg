%option noyywrap

%%

"SE"	{ return SE; }
"SW"	{ return SW; }
"NE"	{ return NE; }
"NW"	{ return NW; }
"N"		{ return N; }
"S"		{ return S; }
"W"		{ return W; }
"E"		{ return E; }
[ \n\t]	;
#.*\n	;
.		return *yytext;
