#!/usr/bin/env ruby
#
# Put script description here.
#
# Author::    
# Copyright:: 
# License::   Ruby
#


text = File.read('mrLiquid-demo.mpi')


text.gsub!(/demo\//, 'install/')
text.gsub!(/-demo/, '')

f = File.open('mrLiquid.mpi', 'w')
f.puts text
f.close
