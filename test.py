from mojom.parse.parser import Parse
from mojom.generator.definitions_generator_um import GenerateDefinitions
from mojom.generator.servers_generator_um import GenerateServers
from mojom.generator.runtime_generator_um import GenerateRuntime
import shutil
import glob

filenames = glob.glob("mojom/*.mojom")
trees = []
dst = 'mojom/generator/gene'
for filename in filenames:
    f = open(filename)
    tree = Parse(f.read(), filename)
    trees.append(tree)
    # Generate defs & clients
    GenerateDefinitions(tree, filename)
    # Generate servers
    GenerateServers(tree, filename)
    shutil.copy(filename + '.h', dst)
    shutil.copy(filename + '.server.h', dst)

GenerateRuntime(trees, filenames)
shutil.copy('gene_runtime.h', dst)


