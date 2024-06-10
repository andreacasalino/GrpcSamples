import os, subprocess, shutil
from argparse import ArgumentParser

def resetFolder(path):
    if os.path.exists(path):
        shutil.rmtree(path)
    os.makedirs(path)
    return path

class Paths:
    ROOT = os.path.dirname(__file__)

def for_each_proto():
    folder_abs = os.path.join(Paths.ROOT, Paths.ROOT)
    for name in os.listdir(folder_abs):
        if name.find('.proto') == -1:
            continue
        yield os.path.join(folder_abs, name)

def main(grpc_plugin, build_folder):
    build_folder = resetFolder(os.path.join(Paths.ROOT, 'build')) if build_folder == None else build_folder
    protos = ' '.join([proto for proto in for_each_proto()])
    frmt = {
        'OUT':build_folder,
        'INCLUDE_PATH':os.path.join(Paths.ROOT),
        'SRC':protos,
        'PLUGIN':grpc_plugin
    }
    cmd = 'protoc -I {INCLUDE_PATH} --cpp_out {OUT} --grpc_out {OUT} --plugin=protoc-gen-grpc={PLUGIN} {SRC}'.format(**frmt)
    cmd = cmd.split()
    print('===> generating proto using\n{}'.format( '\n'.join(cmd) ))
    retVal = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if not retVal.returncode == 0:
        raise Exception(retVal.stderr)

if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument('--grpc_plugin', default='/usr/local/bin/grpc_cpp_plugin')
    parser.add_argument('--build_folder', default=None)
    args = parser.parse_args()

    main(args.grpc_plugin, args.build_folder)
