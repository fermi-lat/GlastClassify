# -*- python -*-
# $Header: /nfs/slac/g/glast/ground/cvs/GlastClassify/SConscript,v 1.19.2.1 2010/02/10 02:04:22 heather Exp $
# Authors: Tracy Usher <usher@slac.stanford.edu>
# Version: GlastClassify-07-05-00-gr02
Import('baseEnv')
Import('listFiles')
Import('packages')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

libEnv.Tool('GlastClassifyLib', depsOnly = 1)

ClassificationTree = libEnv.StaticLibrary('ClassificationTree', listFiles(['src/*.cxx',
			'src/ImActivityNodes/*.cxx',
			'src/XT/*.cxx',
			'src/xmlBuilders/*.cxx']))


GlastClassify = libEnv.SharedLibrary('GlastClassify', listFiles(['src/Dll/*.cxx']))

progEnv.Tool('GlastClassifyLib')
progEnv.Tool('addLibrary', library = ['ClassificationTree'])
apply = progEnv.Program('apply', listFiles(['src/apply/*.cxx']))
progEnv.Tool('registerObjects', package = 'GlastClassify', 
	libraries = [ClassificationTree, GlastClassify], 
	binaries = [apply], 
	includes = listFiles(['GlastClassify/*.h']))




