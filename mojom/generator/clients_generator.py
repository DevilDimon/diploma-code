import uuid

from mojom.parse.ast import Struct, Constraint, Interface, ComparisonPredicate
from mojom.generator.definitions_generator import GenerateTypename

def GenerateClients(tree, filename):
    res = '#pragma once\n'

    import_list = tree.import_list
    if import_list is not None:
        for import_item in import_list:
            res += "#include \"" + import_item.import_filename + ".h\"\n\n"

    res += "#include \"gene_embedded_types.h\"\n\n"

    if tree.module is not None:
        namespace = tree.module.mojom_namespace[1]
        res += 'namespace ' + namespace + ' {\n\n'

    for obj in tree.definition_list:
        if isinstance(obj, Interface):
            res += GenerateInterface(obj) + '\n'
            res += GenerateInterfaceUserModeClient(obj) + '\n'

    if tree.module is not None:
        res += '\n}  // ' + namespace + '\n\n'

    file = open(filename + '.client.h', 'w')
    file.write(res)

    return res

def GenerateInterface(interface):
    res = 'class ' + interface.mojom_name + ' {\n'
    res += '\tpublic:\n'
    res += '\tconst uint64_t __service_id = ' + str(uuid.uuid1().int >> 64) + 'UL;\n'

    for method in interface.body.items:
        res += '\tconst uint64_t ' + GenerateMethodIdField(method) + ' = ' + str(uuid.uuid1().int >> 64) + 'UL;\n'

    res += '\n'
    for method in interface.body.items:
        res += '\tvirtual bool ' + method.mojom_name + '('
        for arg in method.parameter_list:
            res += 'const ' + GenerateTypename(arg.typename) + ' &' + arg.mojom_name + ', '
        res = res[:-2] + ') = 0;\n'
    res += '\n};'

    return res

def GenerateInterfaceUserModeClient(interface):
    res = 'class ' + interface.mojom_name + 'Client final : public ' + interface.mojom_name + ' {\n'
    res += '\tpublic:\n'

    for method in interface.body.items:
        res += '\tbool ' + method.mojom_name + '('
        is_empty = True
        for arg in method.parameter_list:
            res += 'const ' + GenerateTypename(arg.typename) + ' &' + arg.mojom_name + ', '
            is_empty = False
        if not is_empty:
            res = res[:-2]
        res += ') final {\n'
        res += '\t\tgene_internal::container __c;\n'
        res += '\t\treturn gene_internal::serialize(__service_id, __c) &&\n'
        res += '\t\t\tgene_internal::serialize(' + GenerateMethodIdField(method) + ', __c) &&\n'
        for arg in method.parameter_list:
            res += '\t\t\tgene_internal::serialize(' + arg.mojom_name + ', __c) &&\n'
        res += '\t\t\tgene_internal::send_message_internal(__c);\n'
        res += '\t}\n'
    res += '};\n'

    return res

def GenerateMethodIdField(method):
    return '__' + method.mojom_name + '_id'