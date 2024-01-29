#!/usr/env ruby

tokens = []

File.foreach("fileobjgrammar.y") { |x|
  next if x !~ /^%token\s+[A-Z]/
  x.sub!(/%token/,"")
  x.squeeze!(' ')
  tokens += x.split(' ');
}

tokens.uniq!
tokens = tokens.sort_by { |a| m = a.sub(/_$/,""); [m.length(), a] }
tokens.reverse!
tokens.each { |t|
  m = t.sub(/_$/,"")
  puts "<INITIAL>\"#{m.downcase}\"\t{ DBG_TOKEN(); return #{t}; }";
}
