import os

from mojom.parse.ast import Interface
from mojom.generator.definitions_generator import GenerateTypename
from mojom.generator.clients_generator import GenerateMethodIdField


def GenerateRuntime(trees, filenames):
    res = '#pragma once\n'
    for filename in filenames:
        res += '#include \"' + os.path.basename(os.path.normpath(filename)) + '.h\"\n'
        res += '#include \"' + os.path.basename(os.path.normpath(filename)) + '.client.h\"\n'

    res += '\n'
    res += '#include \"gene_includes.h\"\n\n'
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
            res += GenerateHandlerRegistrator(interface) + '\n'

    res += GenerateMessageProcessing(trees) + '\n'

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
    res += '\t\t' + GenerateFieldName(interface) + ' = handler;\n'
    res += '\t}'

    return res

def GenerateMessageProcessing(trees):
    res = '\tvoid ProcessIncomingMessage(gene_internal::container &in, gene_internal::container *out) {\n'
    # Get service id
    res += '\t\tuint64_t service_id;\n'
    res += '\t\tif (!gene_internal::deserialize(in, &service_id)) {\n'
    res += '\t\t\tgene_internal::serialize(gene_internal::gene_error_code, *out);\n'
    res += '\t\t\treturn;\n'
    res += '\t\t}\n'

    for tree in trees:
        for interface in filter(lambda obj: isinstance(obj, Interface), tree.definition_list):
            res += '\t\tif (' + GenerateFieldName(interface) + ' && service_id == ' + GenerateFieldName(interface) + '->__service_id) {\n'
            # Get method id
            res += '\t\t\tuint64_t method_id;\n'
            res += '\t\t\tif (!gene_internal::deserialize(in, &method_id)) {\n'
            res += '\t\t\t\tgene_internal::serialize(gene_internal::gene_error_code, *out);\n'
            res += '\t\t\t\treturn;\n'
            res += '\t\t\t}\n'

            for method in interface.body.items:
                res += '\t\t\tif (method_id == ' + GenerateFieldName(interface) + '->' + GenerateMethodIdField(method) + ') {\n'
                # Deserialize parameters
                for arg in method.parameter_list:
                    res += '\t\t\t\t' + GenerateTypename(arg.typename) + ' ' + arg.mojom_name + ';\n'
                    res += '\t\t\t\tif (!gene_internal::deserialize(in, &' + arg.mojom_name + ')) {\n'
                    res += '\t\t\t\t\tgene_internal::serialize(gene_internal::gene_error_code, *out);\n'
                    res += '\t\t\t\t\treturn;\n'
                    res += '\t\t\t\t}\n'
                # Call the method
                res += '\t\t\t\tauto res = ' + GenerateFieldName(interface) + '->' + method.mojom_name + '('
                is_empty = True
                for arg in method.parameter_list:
                    res += arg.mojom_name + ', '
                    is_empty = False
                if not is_empty:
                    res = res[:-2]
                res += ');\n'
                res += '\t\t\t\tif (!res) {\n'
                res += '\t\t\t\t\tgene_internal::serialize(gene_internal::gene_error_code, *out);\n'
                res += '\t\t\t\t\treturn;\n'
                res += '\t\t\t\t}\n'
                res += '\t\t\t}\n'
            res += '\t\t}\n'
    res += '\t}'

    return res


def GenerateFieldName(interface):
    return '_' + interface.mojom_name + '_handler'