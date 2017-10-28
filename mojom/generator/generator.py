from mojom.parse.ast import Struct


def Serialize(tree, filename):
    res = ''

    import_list = tree.import_list
    if import_list is not None:
        for import_item in import_list:
            res += "#include \"" + import_item.import_filename + ".h\"\n\n"

    res += "#include \"gene_embedded_types.h\"\n\n"

    if tree.module is not None:
        namespace = tree.module.mojom_namespace[1]
        res += 'namespace ' + namespace + ' {\n\n'

    for obj in tree.definition_list:
        if isinstance(obj, Struct):
            res += SerializeStruct(obj) + '\n'

    if tree.module is not None:
        res += '\n}  // ' + namespace + '\n\n'

    res += GenerateSerializer(tree)

    file = open(filename + '.h', 'w')
    file.write(res)

    return res

def SerializeStruct(struct):
    res = 'struct ' + struct.mojom_name + ' {\n'
    for field in struct.body.items:
        res += '\t' + SerializeTypename(field.typename) + ' ' + field.mojom_name

        if field.default_value is not None:
            res += ' = ' + field.default_value

        res += ';\n'
    return res + '\n};'

def SerializeTypename(typename):
    if typename == 'string':
        return 'std::string'
    elif typename == 'int32':
        return 'int'
    elif typename.endswith('[]'):
        return SerializeArrayTypename(typename)
    else:
        return typename

def SerializeArrayTypename(typename):
    if typename.endswith('[]'):
        return 'std::vector<' + SerializeArrayTypename(typename[:-2]) + '> '
    else:
        return SerializeTypename(typename)


def GenerateSerializer(tree):
    res = 'namespace gene {\n\n'
    res += 'using namespace gene_internal;\n\n'
    for obj in tree.definition_list:
        if isinstance(obj, Struct):
            res += GenerateStructSerializer(obj) + '\n}\n\n'
    res += '\n} // gene\n'
    return res



def GenerateStructSerializer(struct):
    res = 'template <> struct serializer<' + struct.mojom_name + '> {\n'
    res += '\tbool operator()(const ' + struct.mojom_name + ' &v, container &c) {\n'
    res += '\t\treturn\n'

    # TODO: handle empty structs here and everywhere
    # if len(struct.body.items) == 0:
    #     res += 'true;\n}'
    #     return res

    for field in struct.body.items:
        res += '\t\t\tserialize(v.' + field.mojom_name + ', c) &&\n'
    res = res[:-4] + ';\n\t}\n'

    res += '\tbool operator()(const container &c, ' + struct.mojom_name + '*v) {\n'
    res += '\t\tif (!v)\n\t\t\treturn false;\n\t\treturn \n'
    for field in struct.body.items:
        res += '\t\t\tdeserialize(c, &v->' + field.mojom_name + ') &&\n'
    return res[:-4] + ';\n\t}'
