from mojom.parse.parser import Parse
from mojom.generator.generator import Serialize
filename = 'mojom/test.mojom'

f = open(filename)

tree = Parse(f.read(), filename)
print Serialize(tree, filename)