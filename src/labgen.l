%option noyywrap
%option yylineno

%%

[0-9]+								{ yylval.entier=atoi(yytext); return CNUM; }
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
"WALL"								{ return tk_WALL; }
"UNWALL"							{ return tk_UNWALL; }
"TOGGLE"							{ return tk_TOGGLE; }
"SE"|"SW"|"NE"|"NW"|"N"|"S"|"W"|"E"	{ yylval.direction=wr_fromstr(yytext); if(yylval.direction == LG_WrUU) yyerror("%s: unknown direction", yytext); return DIR; }
[a-zA-Z][a-zA-Z0-9]*				{ yylval.chaine=strdup(yytext); return IDENT; }
[ \n\t]								;
#.*\n								;
.									return *yytext;
