#!/usr/bin/env python3
import errno
import json
import os
import pathlib as pt
import pprint
import subprocess as sp
import sys

def parse_c_header(source, defines={}, recursive=True):
    c_std_lib = [
        'assert.h', 'complex.h', 'ctype.h', 'errno.h',
        'fenv.h', 'float.h', 'inttypes.h', 'iso646.h',
        'limits.h', 'locale.h', 'math.h', 'setjmp.h',
        'signal.h', 'stdalign.h', 'stdarg.h', 'stdatomic.h',
        'stdbool.h', 'stddef.h', 'stdint.h', 'stdio.h',
        'stdlib.h', 'stdnoreturn.h', 'string.h', 'tgmath.h',
        'threads.h', 'time.h', 'uchar.h', 'wchar.h', 'wctype.h']

    orig_wd = os.getcwd()
    source = pt.Path(source).resolve()

    if not source.exists():
        raise FileNotFoundError(errno.ENOENT, os.strerror(errno.ENOENT), source.as_posix())
    os.chdir(source.parents[0])

    result_dict = {
        'keyword_defined' : {},
        'include_list' : {
            'lib':[],
            'src':[]
        }
    }
    if defines: result_dict['keyword_defined'].update(defines)
    keyword_ifdef = []

    file_lines = None
    with open(source, 'r', encoding='utf-8') as fp:
        file_lines = fp.readlines()

    for index, line in enumerate(file_lines):
        cur_line = line.strip()
        if not cur_line.startswith('#'):
            continue
        else:
            cur_line = cur_line[1:] # remove '#'
            token = cur_line.strip().split(' ')
        if not token: continue

        # Preprocessor IF control
        if token[0] == 'ifdef':
            if len(token) < 2: raise Exception(f'ifdef line {index} too short')
            keyword_ifdef.append((token[1], bool(token[1] in result_dict['keyword_defined'])))
        elif token[0] == 'ifndef':
            if len(token) < 2: raise Exception(f'ifndef line {index} too short')
            keyword_ifdef.append((token[1], bool(not token[1] in result_dict['keyword_defined'])))
        elif token[0] == 'endif':
            # TODO : FIX HERE (try this code with only keyword_ifdef.pop() then you'll notice a problem.)
            try:
                keyword_ifdef.pop()
            except:
                pass
        else:
            # Preprocessor VAR control
            if not keyword_ifdef or keyword_ifdef[-1][1]:
                if token[0] == 'define':
                    if len(token) < 2: raise Exception(f'define line {index} too short')
                    result_dict['keyword_defined'][token[1]] = None if len(token) < 3 else ''.join(token[2:])
                elif token[0] == 'include':
                    if len(token) < 2: raise Exception(f'include line {index} too short')
                    value = ''.join(token[1:])
                    if '<' in value:
                        result_dict['include_list']['lib'].append(value.replace('<', '').replace('>', ''))
                    else:
                        value = value.replace('"', '')
                        result_dict['include_list']['src'].append(pt.Path(value).resolve(strict=True))
    if recursive:
        for file in result_dict['include_list']['src']:
            recursive_result = parse_c_header(file, result_dict['keyword_defined'], True)
            result_dict['include_list']['lib'] += recursive_result['include_list']['lib']
            result_dict['include_list']['src'] += recursive_result['include_list']['src']
            result_dict['keyword_defined'].update(recursive_result['keyword_defined'])

    os.chdir(orig_wd)

    #remove duplicated items
    result_dict['include_list']['lib'] = list(dict.fromkeys(result_dict['include_list']['lib']))
    result_dict['include_list']['src'] = list(dict.fromkeys(result_dict['include_list']['src']))

    #remove C standard library
    result_dict['include_list']['lib'] = [z for z in result_dict['include_list']['lib']\
                                            if z not in c_std_lib]

    return result_dict

def get_target_src_files(target):
    result = parse_c_header(target)

    #include C files if exists
    tmp_src_c_list = list()
    for file in result['include_list']['src']:
        target_c_src = file.with_suffix('.c')
        if target_c_src.exists():
            tmp_src_c_list.append(file.with_suffix('.c'))
    result['include_list']['src'] += tmp_src_c_list
    result['include_list']['src'].sort()

    result['include_list']['src'].insert(0, pt.Path(target).resolve())

    return result

def include_concat(filename):
    result_txt = ''
    
    src_target_list = get_target_src_files(filename)
    for src_path in src_target_list['include_list']['src']:
        if src_path.suffix == '.h':
            result_txt += src_path.read_text()
            result_txt += '\n'

    return result_txt

def extract_typedef_struct_code(include_data):
    struct_data = dict()
    struct_parenthesis_stack = list()
    enum_parenthesis_stack = list()

    tmp_line_buffer = ''
    tmp_line_buffer_enum = ''
    for index, line in enumerate(include_data.splitlines()):
        if 'typedef struct' in line:
            struct_parenthesis_stack.append(line)
            tmp_line_buffer += (line + '\n')
            continue
        elif line.startswith('enum'):
            enum_parenthesis_stack.append(line)
            tmp_line_buffer_enum += (line + '\n')
            continue
        elif line.startswith('}'):
            if enum_parenthesis_stack:
                enum_parenthesis_stack.pop()
                # TODO : NEED TO PARSE ENUM!
                continue

            struct_name = line.replace('}', '').replace(';', '').strip()
            if struct_name:
                struct_data[struct_name] = tmp_line_buffer
                tmp_line_buffer = ''
            continue
        elif struct_parenthesis_stack:
            tmp_line_buffer += (line + '\n')
            continue
    return struct_data



def ctags_parse(project_dir):
    orig_wd = pt.Path()
    print(orig_wd.absolute())
    os.chdir(project_dir)

    ctags_proc = None
    try:
        ctags_proc = sp.run(['ctags', '-R', '--output-format=json'], capture_output=True, check=True, shell=True)
    except Exception as e:
        raise e

    ctags_output = ctags_proc.stdout.decode()
    ctags_output_corrected = ''
    for line in ctags_output.splitlines():
        ctags_output_corrected += f'{line},\n'
    ctags_output_corrected = ctags_output_corrected[:-2]
    ctags_output_corrected = '{"tagdata":[' + ctags_output_corrected + ']}'

    try:
        ctags_data = json.loads(ctags_output_corrected)['tagdata']
    except json.JSONDecodeError as err:
        # grab a reasonable section, say 40 characters.
        start, stop = max(0, err.pos - 20), err.pos + 20
        snippet = err.doc[start:stop]
        print(err)
        print('... ' if start else '', snippet, ' ...' if stop < len(err.doc) else '', sep="")
        print('^'.rjust(21 if not start else 25))
        raise err

    function_list = [func_data.get('name', '') for func_data in ctags_data if func_data.get('kind', '') == 'function' and 'glew.c' not in func_data.get('path', '')]
    struct_list = [struct_data.get('name', '') for struct_data in ctags_data if struct_data.get('kind', '') == 'typedef' and 'glew.c' not in struct_data.get('path', '')]
    global_list = [global_data.get('name', '') for global_data in ctags_data if global_data.get('kind', '') == 'variable' and 'glew.c' not in global_data.get('path', '')]

    os.chdir(orig_wd)

    return {
        'ctags_data': ctags_data,
        'function_list': function_list,
        'struct_list': struct_list,
        'global_list': global_list,
    }

def create_vs_def_file(ctags_data):
    file_data = 'LIBRARY FOXSNOW\nEXPORTS\n'

    for function_name in ctags_data['function_list']:
        file_data += f'\t{function_name}\n'

    for global_name in ctags_data['global_list']:
        file_data += f'\t{global_name} DATA\n'

    target_path = pt.Path('foxsnow_dll.def').absolute()
    print(target_path)
    if target_path.exists():
        target_path.unlink()

    with target_path.open('w') as fp:
        fp.write(file_data)

def create_cffi_json_file(ctags_data, input_file):
    # We need to include function and struct definition.

    target_path = pt.Path('foxsnow_cffi_data.json').absolute()
    print(target_path)
    if target_path.exists():
        target_path.unlink()

    with target_path.open('w') as fp:
        fp.write(file_data)

if __name__ == '__main__':
    import pprint

    if len(sys.argv) > 2:
        target_main_src = ''.join(sys.argv[1:])
    else:
        target_main_src = './foxsnow'

    # include_concat_data = include_concat(target_main_src)
    # struct_data = extract_typedef_struct_code(include_concat_data)
    # print(struct_data.keys())
    parsed_data = ctags_parse(target_main_src)
    result = create_vs_def_file(parsed_data)