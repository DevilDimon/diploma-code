import os

from mojom.parse.ast import Interface


def GenerateRuntime(trees, filenames):
    res = '#pragma once\n'
    for filename in filenames:
        res += '#include \"' + os.path.basename(os.path.normpath(filename)) + '.h\"\n'

    res += '\n'
    res += '#include <cstdint>\n\n'
    for namespace in set(map(lambda t: t.module.mojom_namespace[1], filter(lambda t: t.module is not None, trees))):
        res += 'using namespace ' + namespace + ';\n'
    res += '\n'

    res += 'class GeneRuntime final {\n'
    res += '\tprivate:\n'
    for tree in trees:
        for interface in filter(lambda obj: isinstance(obj, Interface), tree.definition_list):
            res += '\t' + interface.mojom_name + ' *' + GenerateFieldName(interface) + ' = nullptr;\n'
    res += '\tpublic:\n'
    for tree in trees:
        for interface in filter(lambda obj: isinstance(obj, Interface), tree.definition_list):
            res += GenerateHandlerRegistrator(interface)

    res += '\t~GeneRuntime() {\n'
    for tree in trees:
        for interface in filter(lambda obj: isinstance(obj, Interface), tree.definition_list):
            res += '\t\tdelete ' + GenerateFieldName(interface) + ';\n'
            res += '\t\t' + GenerateFieldName(interface) + ' = nullptr;\n'
    res += '\t}\n'
    res += '};\n'

    f = open('gene_runtime.h', 'w')
    f.write(res)

    return res


def GenerateHandlerRegistrator(interface):
    res = '\tvoid Register' + interface.mojom_name + 'Handler(' + interface.mojom_name + ' *handler) {\n'
    res += '\t\t__' + interface.mojom_name + '_handler = handler;\n'
    res += '\t}\n'

    return res

def GenerateFieldName(interface):
    return '__' + interface.mojom_name + '_handler'