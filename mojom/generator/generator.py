from mojom.parse.ast import Struct


def Serialize(tree, filename):
    res = ''

    import_list = tree.import_list
    if import_list is not None:
        for import_item in import_list:
            res += "#include \"" + import_item.import_filename + ".h" + "\"\n"

    res += '\n'

    if tree.module is not None:
        namespace = tree.module.mojom_namespace[1]
        res += 'namespace ' + namespace + ' {\n\n'

    for obj in tree.definition_list:
        if isinstance(obj, Struct):
            res += SerializeStruct(obj) + '\n'

    if tree.module is not None:
        res += '\n}  // ' + namespace + '\n'

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