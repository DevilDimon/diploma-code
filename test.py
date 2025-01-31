from mojom.parse.parser import Parse
from mojom.generator.definitions_generator import GenerateDefinitions
from mojom.generator.servers_generator import GenerateServers
from mojom.generator.runtime_generator import GenerateRuntime
from mojom.generator.clients_generator import GenerateClients
import shutil
import glob

filenames = glob.glob("mojom/*.mojom")
trees = []
dst = 'mojom/generator/gene'
for filename in filenames:
    f = open(filename)
    tree = Parse(f.read(), filename)
    trees.append(tree)
    GenerateDefinitions(tree, filename)
    GenerateClients(tree, filename)
    GenerateServers(tree, filename)
    shutil.copy(filename + '.h', dst)
    shutil.copy(filename + '.client.h', dst)
    shutil.copy(filename + '.server.h', dst)

GenerateRuntime(trees, filenames)
shutil.copy('gene_runtime.h', dst)


