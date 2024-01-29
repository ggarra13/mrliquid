#!/usr/bin/env ruby

MAYA_VERSIONS = %w(
   7.0 8.0 8.0-x64 8.5 8.5-x64 2008 2008-x64 2009-x64 2013-x64 2013
)
RUBY_VERSIONS = %w(
   1.9 1.8
)

@program_files = ENV['PROGRAMFILES'] || ''
@program_files = @program_files.gsub(/\\/, '/')
@program_files = @program_files.gsub(/ \(x86\)/, '')

MAYA_PLACES = [
               "#{ENV['MAYA_ROOT']}/maya",
               "#{ENV['MAYA_ROOT']}/Maya",
               "/usr/autodesk/maya",
               "/usr/aw/maya",
               "/usr/local/autodesk/maya",
               "/usr/local/aw/maya",
               "/opt/autodesk/maya",
               "/opt/aw/maya",
               "#@program_files/Autodesk/Maya",
              ]

MAYA_COMPILERS = { 
  'Windows' => { 
    '7.0'      => '.NET 2003',
    '8.0'      => '.NET 2003',
    '8.0-x64'  => '8',
    '8.5'      => '8',
    '8.5-x64'  => '8',
    '2008'     => '8',
    '2008-x64' => '8',
    '2013-x64' => '10.0'
  }
}


def msvc_environment(version)
  compiler_version = MAYA_COMPILERS['Windows'][version]
  dir = "#@program_files/Microsoft Visual Studio #{compiler_version}"
  puts <<EOF
-------------------------------------------------------------------------
  Compiler version for maya #{version} is #{compiler_version}.
-------------------------------------------------------------------------
EOF

  unless File.directory?(dir)
    puts <<EOF
Compiler version for maya version #{version} is #{compiler_version}.
Directory '#{dir}' does not exist.
EOF
    exit 1
  end


  vsvars = "#{dir}/Common7/Tools/vsvars32.bat"

  unless File.readable?(vsvars)
    puts <<EOF
File '#{vsvars}' is missing or unreadable.
Cannot set compiler environment.
EOF
    exit 1
  end

  msvc = `"#{vsvars}" && set`

  envvars = {}
  msvc.each_line do |line|
    line =~ /^(.*)=(.*)$/
    next unless $1
    envvars[$1] = $2
  end

  envvars.each { |k,v|
    ENV[k] = v
  }

end

def _run_build(maya_location, *args)
  puts <<EOF
####################################################################
 BEGIN Building for maya #{maya_location}, ruby #{ENV['RUBY_VERSION']}
####################################################################

EOF

  maya_location =~ /maya(\d+(?:\.\d+)?)/i
  version = $1
  version += "-ruby#{ENV['RUBY_VERSION']}"

  maya_location =~ /maya(\d+(?:\.\d+)?(?:-x64)?)/i
  full_version = $1

  type=32
  if full_version =~ /.*-x64/
    type=64
  end
  
  if RUBY_PLATFORM =~ /mswin/
    msvc_environment( full_version )
    ok = system("sh -c 'mk #{type} #{args.join(' ')}'")
  else
    ok = system("mk debug #{type} #{args.join(' ')}")
  end

  text = ok ? "END" : "FAILED"

  puts <<EOF
####################################################################
 #{text}  Building for maya #{maya_location}, ruby #{ENV['RUBY_VERSION']}
####################################################################
EOF

  if !ok
    exit(1)
  end

end

# for debugging environment
def dump_env(start)
  puts '-'*50 + " #{start}"
  ENV.each { |k,v|
    puts "#{k}=#{v}"
  }
  puts '-'*78
end

def run_build(maya_location, *args)

  sep = ':'
  sep = ';' if RUBY_PLATFORM =~ /mswin/ or RUBY_PLATFORM =~ /cygwin/

  if ENV['RUBY_VERSION']
    _run_build( maya_location, *args )
  else
    paths = ENV['PATH'].split(sep)
    for v in RUBY_VERSIONS
      for path in paths
        ruby = "#{path}/ruby#{v}"
        if File.exists?(ruby)
          ENV['RUBY_VERSION'] = v
          _run_build( maya_location, *args )
          break
        end
      end
    end
    ENV.delete('RUBY_VERSION')
  end
end



if ENV['MAYA_LOCATION']
  run_build( ENV['MAYA_LOCATION'], ARGV )
else
  for v in MAYA_VERSIONS
    for i in MAYA_PLACES
      maya_location = "#{i}#{v}"
      if File.directory?( maya_location )
        oldenv = {}
        ENV.each { |k,val| oldenv[k] = val }
        ENV['MAYA_LOCATION'] = maya_location
        puts maya_location
        run_build( maya_location, ARGV )
        ENV.clear
        oldenv.each { |k,val| ENV[k] = val }
      end
    end
  end
end


