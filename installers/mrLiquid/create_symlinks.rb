#!/usr/bin/env ruby
#
# Create an "install" and "demo" distribution by creating
# symlinks or copies of the actual built files.
#

require 'pathname'
require 'fileutils'


@root         = Pathname.new('../..').realpath
@build        = "#{@root}/BUILD"

PLATFORMS    = %w( Linux-x86 Windows OSX )

SUBDIRS = { 
  'Linux-x86' => %w( i686 x86_64 ),
  'Windows'   => %w( win32 win64 ),
  'OSX'       => %w( ),
}

IGNORE_EXT     = /\.(?:exp|lib|pdb|a)$/
IGNORE_BACKUPS = /~$/
MIDIRS = [
          "#{@root}/mrClasses/GGShaderLib/mi",
          "#{@root}/mrLiquid/src/shaders",
         ]


MAYA_LIBRARIES = %w(
uuid
util
fam
awCacheShared
HumanIKShared
DeformSlice
DynUISlice
AnimUISlice
Devices
ImageUI
HWRenderMaya
SharedUI
cxaguard
irc
guide
svml
Xpm
Xp
Xm
imf
tbb
Base
RenderSlice
KinSlice
Subdiv
PolySlice
AnimSlice
SubdivGeom
SubdivEngine
python2.5
python2.4
UrchinSlice
NurbsSlice
DynSlice
Translators
HWRender
HWGL
HWFoundation
ModelSlice
Shared
Poly
RenderModel
Nurbs
3dGraphics
PolyEngine
DataModel
ExtensionLayer
DependCommand
Image
NurbsEngine
GeometryEngine
DependEngine
CommandEngine
AnimEngine
AppVersion
iff
awnSolver
AG
IMFbase
awxml2
ModelUISlice
Manips
OpenMayaAnim
OpenMayaRender
OpenMayaUI
Foundation
OpenMaya
OpenMayaFX
StringCatalog
IMF
plc4
plds4
nspr4
xpcom
).join(' ')


soft         = 'mrLiquid'
soft_version = '0.8.0'
out          = "../install/#{soft}/#{soft_version}"
demo         = "../demo/#{soft}-demo/#{soft_version}"
@v           = true




#
# Fix a relative symbolic link to work in new destination
#
def ln_s(src, dest, opts = {})
  #FileUtils.ln_s(src, dest, { :force => true, :verbose => @v }.merge(opts) ) rescue nil
  if File.directory?(src)
    FileUtils.cp_r(src, dest, { :verbose => @v }.merge(opts) ) rescue nil
  else
    FileUtils.cp(src, dest, { :verbose => @v }.merge(opts) ) rescue nil
  end
end

#
# Create one or more dirs if they don't exist
#
def mkdir(dir)
  FileUtils.mkdir_p(dir, :verbose => @v, :mode => 0775) rescue nil
end


def create(out, type = nil)
  FileUtils.rm_r( out, :force => true )

  unless File.directory?(out)
    mkdir(out)
  end

  out  = Pathname.new(out).realpath

  #
  # Link docs
  #
  dest = "#{out}/docs"
  mkdir(dest)
  ln_s  "#{@root}/doc/Instructions.pdf", dest
  ln_s  "#{@root}/installers/License_Commercial.txt", "#{dest}/LICENSE.txt"
  

  for platform in PLATFORMS
    os = platform.sub(/-.*/, '')

    subdirs = SUBDIRS[platform]
    for archdir in subdirs

      arch = { 
        'i686'   => 32,
        'win32'  => 32,
        'x86_64' => 64,
        'win64'  => 64
      }
      arch = arch[archdir]


      build_dir = Dir.glob("#{@build}/#{os}-*-#{arch}")
      next if build_dir.empty?

      build_dir = build_dir[0]

      #
      # Link common executables
      #
      dest = "#{out}/#{platform}/#{archdir}/bin"
      mkdir dest

      bin_files  = "#{build_dir}/Release/bin/*"
      root_files = "#{@root}/bin/*"
      bin_files  = ( Dir.glob(bin_files) + Dir.glob(root_files) )

      bin_files  = bin_files.delete_if { |x| x =~ IGNORE_EXT }
      bin_files  = bin_files.delete_if { |x| x =~ IGNORE_BACKUPS }

      for file in bin_files
        ln_s(file, dest)
      end

      #
      # For each mentalray version, copy shaders
      #
      mr_dirs = "#{build_dir}/Release/lib/mentalray*"
      mr_dirs = Dir.glob(mr_dirs).delete_if { |x| !File.directory? x }

      for mr_dir in mr_dirs

        mentalray = mr_dir.sub(/.*\//, '')

        dest = "#{out}/#{platform}/#{archdir}/shaders/#{mentalray}"

        #
        # Create mentalray directory
        #
        mkdir dest

        
        #
        # Copy mentalray shaders
        #

        pwd = Dir.pwd
        Dir.chdir(mr_dir)
        versions = Dir.glob("*")
        versions = versions.delete_if { |x| not File.directory?(x) }
        versions = [''] if versions.empty?
        Dir.chdir pwd

        for version in versions
          mkdir "#{dest}/#{version}"

          shaders = "#{mr_dir}/#{version}/*"
          shaders = Dir.glob(shaders).delete_if { |x| x =~ IGNORE_EXT }

          for shader in shaders
            if shader !~ /\.(?:sog|so|dll|manifest)$/ and shader !~ /\/lib[^\/]+/
              next
            end
            case shader
            when /libmrLibrary\.so$/
              next
            when /libmrLibrary\.so\..*$/
              ln_s(shader, "#{dest}/#{version}")
              name = File.basename(shader)
              FileUtils.ln_s("./#{name}", 
                             "#{dest}/#{version}/libmrLibrary.so")
            else
              ln_s(shader, "#{dest}/#{version}")
            end
          end
        end
        
        #
        # Copy mentalray .mi files
        #
        for version in versions
          for midir in MIDIRS
            files = Dir.glob( "#{midir}/*.mi" )
            for file in files
              ln_s(file, "#{dest}/#{version}")
            end
          end
        end

      end

      #
      # For each maya version, link plug-ins, scripts, icons and auxiliary
      # libraries.
      #
      maya_dirs = "#{build_dir}/Release/lib/maya*"
      p "maya_dirs:", maya_dirs

      maya_dirs = Dir.glob(maya_dirs).delete_if { |x| !File.directory? x }

      p "maya_dirs:", maya_dirs

      for maya_dir in maya_dirs

        maya = maya_dir.sub(/.*\//, '')
        maya_ver = maya.sub(/-x64$/, '')

        dest = "#{out}/#{platform}/#{archdir}/#{maya}"

        #
        # Create maya module root
        #
        mkdir dest

        #
        # Create maya module directories
        #
        module_dirs = %w{ bin lib icons plug-ins scripts resources/en }
        for module_dir in module_dirs
          mkdir "#{dest}/#{module_dir}"
        end
        
        #
        # Copy config files to root
        #
        files = Dir.glob("#{@root}/config/*")
        files = files.delete_if { |x| x =~ IGNORE_BACKUPS }
        for file in files
          ln_s(file, "#{dest}")
        end

        #
        # Copy maya plugins
        #
        plugins = "#{maya_dir}/*"
        plugins = Dir.glob(plugins).delete_if { |x| x =~ IGNORE_EXT }


        for plugin in plugins
          next unless plugin =~ /\.(?:sog|so|mll|manifest)$/
          if type
            next unless plugin =~ /-#{type}/
          else
            next if plugin =~ /-demo/
          end
          ln_s(plugin, "#{dest}/plug-ins")

          # puts "symlibs #{plugin} -x #{MAYA_LIBRARIES} -d lib"
          # system("symlibs #{plugin} -x #{MAYA_LIBRARIES} -d lib")
        end

        #
        # Copy maya scripts
        #
        scripts = Dir.glob("#{@root}/mrLiquid/scripts/#{maya_ver}/*.mel") +
          Dir.glob("#{@root}/rubyMEL/scripts/*.mel") +
          Dir.glob("#{@root}/mrClasses/GGShaderLib/AEtemplates/*.mel")
        for script in scripts
          ln_s(script, "#{dest}/scripts")
        end

        #
        # Copy maya icons
        #
        icons = Dir.glob("#{@root}/mrClasses/GGShaderLib/icons/*.xpm") +
                Dir.glob("#{@root}/mrLiquid/icons/*.xpm")
        for icon in icons
          ln_s(icon, "#{dest}/icons")
        end

        #
        # Link auxiliary libraries
        #
        lib_files  = "#{maya_dir}/lib/*"
        lib_files  = Dir.glob(lib_files)

        lib_files = lib_files.delete_if { |x| 
          File.directory?(x) or x =~ IGNORE_EXT
        }

        for file in lib_files
          ln_s(file, "#{dest}/lib")
        end

        #
        # Link executables
        #
        bin_files  = "#{maya_dir}/bin/*"
        bin_files  = Dir.glob(bin_files)

        bin_files = bin_files.delete_if { |x| 
          File.directory?(x) or x =~ IGNORE_EXT
        }

        for file in bin_files
          ln_s(file, "#{dest}/bin")
        end
      end

    end
  end
end


create(demo, 'demo')
create(out)


