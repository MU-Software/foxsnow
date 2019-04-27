# TODO
# - 

import array
import copy
import ctypes
import os

dir_path = lambda x : os.path.dirname(os.path.abspath(x)).replace(os.path.sep, '/')
file_real_path = lambda x, y : dir_path(x) + '/' + y

# OBJ format support
# From https://www.pygame.org/wiki/OBJFileLoader
def load_obj(filename,
			 p_vert=None, p_index=None,
			 enable_cull=True, # cull=gl.GL_CCW,
			 scale=.25, swapyz=False):
	"""Loads a Wavefront OBJ file. """
	vertex   = []
	normal   = []
	texcoord = []
	index    = []

	# mtl = []

	# material = None
	for line in open(filename, 'r', encoding='utf-8'):
		if line.startswith('#'): continue
		values = line.split()
		if not values: continue

		if values[0] == 'v': # Position
			v = list(map(float, values[1:4]))
			if swapyz:
				v = (v[0], v[2], v[1])

			v = list(map(lambda x: x*scale, v))
			vertex += v

		elif values[0] == 'vn': # Normal
			v = list(map(float, values[1:4]))
			if swapyz:
				v = (v[0], v[2], v[1])
			normal.append(v)

		elif values[0] == 'vt': # UV
			texcoord.append(list(map(float, values[1:3])))

		# elif values[0] in ('usemtl', 'usemat'): # Material related
		# 	material = values[1]
		# elif values[0] == 'mtllib': # Material related
		# 	mtl = MTL(file_real_path(filename, values[1]))

		elif values[0] == 'f':
			if len(values[1].split('/')) == 1: # Legacy, only for testing
				# This will be just poligon only model.
				index += list(map(lambda x:int(x)-1, values[1:]))

			# for v in values[1:]:

			# 	w = v.split('/')
			# 	face.append(int(w[0]))
			# 	if len(w) >= 2 and len(w[1]) > 0:
			# 		texcoord.append(int(w[1]))
			# 	else:
			# 		texcoord.append(0)
			# 	if len(w) >= 3 and len(w[2]) > 0:
			# 		norms.append(int(w[2]))
			# 	else:
			# 		norms.append(0)
			# self.faces.append((face, norms, texcoord, material))

	# TODO : Culling control should be modifiable
	# np_vertex = np.array(vertex, dtype=np.float32)
	# np_index  = np.array(index,  dtype=np.int)
	# print(np_vertex[100])
	# print(np_index)
	c_vertex = array.array('f', vertex)
	c_index  = array.array('I', index)

	vert_info = c_vertex.buffer_info()
	index_info = c_index.buffer_info()
	print('vertex | size = {0}, pointer = 0{1} | AT PYTHON'.format(vert_info[1], hex(vert_info[0])[2:].upper()))
	print('index  | size = {0}, pointer = 0{1} | AT PYTHON'.format(index_info[1], hex(index_info[0])[2:].upper()))
	print('result = {0}'.format(c_vertex.tolist()[100]))
	print('result = {0}'.format(c_index.tolist()[100]))
	return (*c_vertex.buffer_info(), *c_index.buffer_info())
	#if type(p_vert) == ctypes.

# def MTL(filename):
# 	contents = {}
# 	mtl = None
# 	for line in open(filename, "r"):
# 		if line.startswith('#'): continue
# 		values = line.split()
# 		if not values: continue
# 		if values[0] == 'newmtl':
# 			mtl = contents[values[1]] = {}
# 		elif mtl is None:
# 			raise ValueError('mtl file doesn\'t start with newmtl stmt')
# 		elif values[0].startswith('map_'):
# 			target = values[0][4:]
# 			# load the texture referred to by this declaration
# 			mtl[values[0]] = values[1]
# 			surf = Image.open(file_real_path(filename, mtl['map_'+target])).transpose(Image.FLIP_TOP_BOTTOM)
# 			image = np.frombuffer(copy.deepcopy(surf.tobytes()), np.uint8)
# 			ix, iy = surf.size
# 			texid = mtl['texture_'+target] = gl.glGenTextures(1)

# 			# gl.glPixelStorei(gl.GL_UNPACK_ALIGNMENT,1)
# 			# gl.glBindTexture(gl.GL_TEXTURE_2D, texid)
# 			# #gl.glTexParameterf(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_S, gl.GL_REPEAT)
# 			# #gl.glTexParameterf(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_T, gl.GL_REPEAT)
# 			# gl.glTexParameterf(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MIN_FILTER, gl.GL_LINEAR)
# 			# gl.glTexParameterf(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MAG_FILTER, gl.GL_LINEAR)
# 			# #gl.glTexParameterf(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MIN_FILTER, gl.GL_LINEAR_MIPMAP_LINEAR)
# 			# gl.glTexImage2D(gl.GL_TEXTURE_2D, 0, gl.GL_RGB, ix, iy, 0, gl.GL_RGB, gl.GL_UNSIGNED_BYTE, image)
# 		else:
# 			mtl[values[0]] = list(map(float, values[1:]))
# 	return contents
