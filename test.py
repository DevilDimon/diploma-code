from mojom.parse.parser import Parse
from mojom.generator.definitions_generator_um import GenerateDefinitions
from mojom.generator.servers_generator_um import GenerateServers
import shutil
import glob

filenames = glob.glob("mojom/*.mojom")
for filename in filenames:
    f = open(filename)
    tree = Parse(f.read(), filename)
    GenerateDefinitions(tree, filename)
    GenerateServers(tree, filename)
    dst = 'mojom/generator/gene'
    shutil.copy(filename + '.h', dst)
    shutil.copy(filename + '.server.h', dst)

