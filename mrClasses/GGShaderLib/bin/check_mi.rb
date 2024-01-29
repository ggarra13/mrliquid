
TYPES = {
  'boolean'       => 'miBoolean',
  'color'         => [ 'miColor', 'color' ],
  'scalar'        => [ 'miScalar', 'float' ],
  'integer'       => [ 'miInteger', 'int'  ],
  'vector'        => [ 'miVector', 'vector', 'point', 'normal' ],
  'matrix'        => [ 'miMatrix', 'matrix' ],
  'geometry'      => 'miTag',
  'array\s+\w+'   => '__ARRAY__',
  'light'         => 'miTag',
  'struct'        => '__STRUCT__'
}

DECLARE = 'declare\s+shader'
SHADER_TYPE = '(color|scalar)\s*(#.*)?'
PARAMS  = '([^\)]*)'
END_DEC = '.*end\s+declare'

TYPES_REX = TYPES.keys().join('|')
PARAMS_REX = /\s*(#{TYPES_REX})\s*("\w*"),?\s*(#:.*)?/
SHADER_MI_REX = /#{DECLARE}\s+#{SHADER_TYPE}\s+("\w+")\s+\(\s+#{PARAMS}\s*\)/

mifile = ARGV[0]
cfile  = ARGV[1]

puts "mifile",mifile
puts " cfile",cfile

in_shader = nil
shadername = nil


f = File.read( mifile )  # slurp file

shaders = {}

t = f.scan( SHADER_MI_REX )
t.each { |x|
  shader = x[2]
  shader = shader[1,shader.length-2]


  params = []
  p = x[3].scan( PARAMS_REX )
  p.each { |x|
    n = x[1]
    n = n[1,n.length-2]
    params.push( [x[0], n]  )
  }
  
  shaders[shader] = [x[0], params]
}

shaders.each { |x,y|
  puts "#{x} => #{y.inspect}"
}

