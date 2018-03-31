import uuid

import os

from mojom.generator.definitions_generator import GenerateTypename
from mojom.parse.ast import Struct, Constraint, Interface, ComparisonPredicate

def GenerateServers(tree, filename):
    res = '#pragma once\n'

    import_list = tree.import_list
    if import_list is not None:
        for import_item in import_list:
            res += "#include \"" + import_item.import_filename + ".h\"\n"

    res += '#include \"' + os.path.basename(os.path.normpath(filename)) + '.h\"\n'
    res += "#include \"gene_embedded_types.h\"\n\n"

    if tree.module is not None:
        namespace = tree.module.mojom_namespace[1]
        res += 'namespace ' + namespace + ' {\n\n'

    for obj in tree.definition_list:
        if isinstance(obj, Interface):
            res += GenerateInterfaceUserModeServer(obj) + '\n'

    if tree.module is not None:
        res += '\n}  // ' + namespace + '\n\n'

    file = open(filename + '.server.h', 'w')
    file.write(res)

    return res

def GenerateInterfaceUserModeServer(interface):
    res = 'class ' + interface.mojom_name + 'Server final : public ' + interface.mojom_name + ' {\n'
    res += '\tpublic:\n'
    for method in interface.body.items:
        res += '\tbool ' + method.mojom_name + '('
        is_empty = True
        for arg in method.parameter_list:
            res += GenerateTypename(arg.typename) + ' ' + arg.mojom_name + ', '
            is_empty = False
        if not is_empty:
            res = res[:-2]
        res += ') override {\n'
        res += '\t\t// Do ' + method.mojom_name + '\n\n'
        res += '\t}\n'
    res += '};\n'

    return res