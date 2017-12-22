%option noyywrap
%option yylineno

%%

[0-9]+								{ return CNUM; }
"SE"|"SW"|"NE"|"NW"|"N"|"S"|"W"|"E"	{ return DIR; }
"WALL"|"TOGGLE"|"UNWALL"			{ return tk_INST; }
"SIZE"								{ return tk_SIZE; }
"IN"								{ return tk_IN; }
"OUT"								{ return tk_OUT; }
"SHOW"								{ return tk_SHOW; }
"PTA"								{ return tk_PTA; }
"PTD"								{ return tk_PTD; }
"R"									{ return tk_R; }
"F"									{ return tk_F; }
"FOR"								{ return tk_FOR; }
"WH"								{ return tk_WH; }
"MD"								{ return tk_MD; }
[a-zA-Z][a-zA-Z0-9]*				{ return IDENT; }
[ \n\t]								;
#.*\n								;
.									return *yytext;
