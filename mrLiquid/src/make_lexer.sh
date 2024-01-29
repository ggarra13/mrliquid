#!/bin/sh

bison -l -o grammar.tab.cpp grammar.y
flex -8 -CFe grammar.lex

bison -d -p fileobj -l -o fileobjgrammar.cpp fileobjgrammar.y
sed -e "s/YYSTYPE yylval;/extern YYSTYPE yylval;/" fileobjgrammar.cpp > tmp.cpp
mv tmp.cpp fileobjgrammar.cpp

