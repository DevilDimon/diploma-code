from mojom.parse.ast import Mojom, Struct, StructBody, StructField, Module, Import

def Serialize(tree, filename):
    res = ''

    import_list = tree.import_list
    if import_list is not None:
        for import_item in import_list:
            res += "#include \"" + import_item.import_filename + ".h" + "\"\n"

    if tree.module is not None:
        namespace = tree.module.mojom_namespace[1]
        res += 'namespace ' + namespace + ' {\n'

    for obj in tree.definition_list:
        if isinstance(obj, Struct):
            res += SerializeStruct(obj) + '\n'

    if tree.module is not None:
        res += '}  // ' + namespace + '\n'

    file = open(filename + '.h', 'w')
    file.write(res)

    return res

def SerializeStruct(struct):
    res = 'struct ' + struct.mojom_name + ' { '
    for field in struct.body.items:
        if field.typename == 'string':
            res += 'std::string ' + field.mojom_name
        elif field.typename == 'int32':
            res += 'int ' + field.mojom_name
        elif field.typename.endswith('[]'):
            res += SerializeArray(field)
        else:
            res += SerializeField(field)
        if field.default_value is not None:
            res += ' = ' + field.default_value
        res += '; '
    return res + '};'


def SerializeField(field):
    return field.typename + ' ' + field.mojom_name

def SerializeArray(field):
    return SerializeArrayFieldName(field.typename) + field.mojom_name

def SerializeArrayFieldName(typename):
    if typename.endswith('[]'):
        return 'std::vector<' + SerializeArrayFieldName(typename[:-2]) + '> '
    else:
        return typename