from mojom.parse.parser import Parse
from mojom.generator.generator import Serialize
from shutil import copy

filename = 'mojom/test2.mojom'
f = open(filename)

tree = Parse(f.read(), filename)
print Serialize(tree, filename)
copy(filename + '.h', 'mojom/generator/gene')
