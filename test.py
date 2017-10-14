from mojom.parse.parser import Parse

filename = 'mojom/test.mojom'

f = open(filename)

print Parse(f.read(), filename)