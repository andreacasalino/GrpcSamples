import os, subprocess, shutil
from argparse import ArgumentParser

class Paths:
    ROOT = os.path.dirname(__file__)

def resetFolder(path):
    if os.path.exists(path):
        shutil.rmtree(path)
    os.makedirs(path)
    return path

def for_each_proto():
    folder_abs = os.path.join(Paths.ROOT, Paths.ROOT)
    for name in os.listdir(folder_abs):
        if name.find('.proto') == -1:
            continue
        yield os.path.join(folder_abs, name)

def clear_protos(folder):
    def isProto(filename):
        for ext in ['.pb.h', '.pb.cc']:
            if not filename.find(ext) == -1:
                return True
        return False
    names = [name for name in filter(lambda filename : isProto(filename), os.listdir(folder))]
    for name in names:
        os.remove(os.path.join(folder, name))

class Protos:
    def __init__(self, grpc_plugin, build_path):
        self.grpcPlugin = grpc_plugin
        self.buildPath = build_path
        self.builtTimePlaceHolder = os.path.join(self.buildPath, 'last_build')
        self.lastBuildTime = self.getLastBuildTime_()
        self.lastProtoChangeTime = self.getLastProtoChangeTime_()

    def getLastBuildTime_(self):
        return os.path.getmtime(self.builtTimePlaceHolder) if os.path.exists(self.builtTimePlaceHolder) else None

    def getLastProtoChangeTime_(self):
        lastChangeTime = 0
        for proto_file in for_each_proto():
            lastChangeTime = max(lastChangeTime, os.path.getmtime(proto_file))
        return lastChangeTime

    def generate(self):
        if self.lastBuildTime == None or self.lastBuildTime < self.lastProtoChangeTime:
            clear_protos(self.buildPath)
            protos = ' '.join([proto for proto in for_each_proto()])
            frmt = {
                'OUT':self.buildPath,
                'INCLUDE_PATH':os.path.join(Paths.ROOT),
                'SRC':protos,
                'PLUGIN':self.grpcPlugin
            }
            cmd = 'protoc -I {INCLUDE_PATH} --cpp_out {OUT} --grpc_out {OUT} --plugin=protoc-gen-grpc={PLUGIN} {SRC}'.format(**frmt)
            cmd = cmd.split()
            print('===> generating proto using\n{}'.format( '\n'.join(cmd) ))
            retVal = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            if not retVal.returncode == 0:
                raise Exception(retVal.stderr)        
            with open(self.builtTimePlaceHolder, 'w') as stream:
                stream.write('-')

def main(grpc_plugin, build_folder):
    build_folder = resetFolder(os.path.join(Paths.ROOT, 'build')) if build_folder == None else build_folder
    Protos(grpc_plugin, build_folder).generate()

if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument('--grpc_plugin', default='/usr/local/bin/grpc_cpp_plugin')
    parser.add_argument('--build_folder', default=None)
    args = parser.parse_args()

    main(args.grpc_plugin, args.build_folder)
