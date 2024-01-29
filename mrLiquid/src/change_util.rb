
REGEX = /(mi_api_[_\w]+\(.*\))/
REPLACE = 'MRL_CHECK(\1)'

CFILEREGEX = /\.cpp$|\.inl$|\.h$/
NEWFILEREGEX = /\.new$/


require 'FileUtils'

Dir.foreach('.') { |file|
  next if file == '.' or file == '..' or file !~ CFILEREGEX

  puts file

  newFile = File.open(file + ".new", 'w')
  File.foreach(file) { |x|
    x.gsub!(REGEX, REPLACE)
    newFile.print x
  }
  newFile.close
}

Dir.foreach('.') { |file|
  next if file == '.' or file == '..' or file !~ NEWFILEREGEX

  oldfile = file.sub(NEWFILEREGEX, '')
  puts "#{file} #{oldfile}"
  FileUtils.mv file, oldfile, :force => true, :verbose => true
}
