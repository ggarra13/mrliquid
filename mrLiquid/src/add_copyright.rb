#!/bin/env ruby

require 'fileutils'

def check_copyright(file)
  puts "Checking #{file}..."
  f = File.open(file, 'r')
  t = f.readlines.join()
  return if t =~ /The contents of this file are subject to the Mozilla Public/
  f = File.open(file + ".copy", 'w')

  if t !~ /Redistributions of source code must retain the above copyright/
    puts "#{file} does not have copyright.  Adding..."
    add_copyright(f)
  else
    year = Time.now.year
    t.gsub! /Copyright\s+\(c\)\s+\d+/, "Copyright (c) #{year}"
  end
  f.puts t
  f.close

  FileUtils.mv( file + '.copy', file, :force => true )
end

def add_copyright(f)
  year = Time.now.year
  f.puts <<"EOF"
//
//  Copyright (c) #{year}, Gonzalo Garramuno
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  *       Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//  *       Redistributions in binary form must reproduce the above
//  copyright notice, this list of conditions and the following disclaimer
//  in the documentation and/or other materials provided with the
//  distribution.
//  *       Neither the name of Gonzalo Garramuno nor the names of
//  its other contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
EOF
end


globs = ['*.h', '*.cpp', '*.inl', 
  'shaders/*.h', 'shaders/*.cpp', 
  'raylib/*.inl', 'raylib/*.h']

globs.each { |g| 
  Dir.glob(g) { |file|
    check_copyright(file)
  }
}
