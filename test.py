from mojom.parse.parser import Parse
from mojom.generator.generator import Generate
from shutil import copy

filename = 'mojom/test.mojom'
f = open(filename)

tree = Parse(f.read(), filename)
print Generate(tree, filename)
copy(filename + '.h', 'mojom/generator/gene')
