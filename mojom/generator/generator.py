from mojom.parse.ast import Struct, Constraint, ConstraintField, ComparisonPredicate
import uuid

def Serialize(tree, filename):
    res = ''

    import_list = tree.import_list
    if import_list is not None:
        for import_item in import_list:
            res += "#include \"" + import_item.import_filename + ".h\"\n\n"

    res += "#include \"gene_embedded_types.h\"\n\n"
    res += "#include <cstdint>\n\n"

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

    res += '\tstatic const uint64_t __type_id = ' + str(uuid.uuid1().int >> 64) + 'UL;\n'

    for field in struct.body.items:
        typename = SerializeTypename(field.typename)
        array_brackets = typename.split('[')
        res += '\t' + SerializeTypename(typename.split('[', 1)[0]) + ' ' + field.mojom_name
        if len(array_brackets) > 1:
            for bracket in array_brackets[1:]:
                res += '[' + bracket


        if field.default_value is not None:
            res += ' = ' + field.default_value

        res += ';\n'
    return res + '\n};'

def SerializeTypename(typename):
    if typename == 'string':
        return 'std::string'
    elif typename == 'int32':
        return 'int32_t'
    elif typename == 'int64':
        return 'int64_t'
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
    constraints = {}
    for obj in tree.definition_list:
        if isinstance(obj, Constraint):
            constraints[obj.mojom_name] = obj.body.items

    res = 'namespace gene_internal {\n\n'
    if tree.module is not None:
        res += 'using namespace ' + tree.module.mojom_namespace[1] + ';\n\n'

    for obj in tree.definition_list:
        if isinstance(obj, Struct):
            res += GenerateStructSerializer(obj, constraints) + '\n};\n\n'
    res += '\n} // gene_internal\n'
    return res



def GenerateStructSerializer(struct, constraints):
    return GenerateSerializeOperator(struct, constraints) + GenerateDeserializeOperator(struct, constraints)

def GenerateSerializeOperator(struct, constraints):
    res = 'template <> struct serializer<' + struct.mojom_name + '> {\n'
    res += '\tbool operator()(const ' + struct.mojom_name + ' &v, container &c) {\n'

    # TODO: handle empty structs here and everywhere
    # if len(struct.body.items) == 0:
    #     res += 'true;\n}'
    #     return res

    field_number = 0
    constrained_fields = {}
    for field in struct.body.items:
        if field.attribute_list is not None and field.attribute_list.items[0].key in constraints:
            constraint = constraints[field.attribute_list.items[0].key]
            constrained_fields[field_number] = []
            constraint_number = 0
            for constraint_field in constraint:
                predicate = constraint_field.predicate
                var_name = 'c' + str(field_number) + '_' + str(constraint_number)
                if isinstance(predicate, ComparisonPredicate):
                    if predicate.mojom_name in ('size', 'value'):
                        name = predicate.mojom_name
                        if predicate.comp_op == '=':
                            res += '\t\t' + name + '_equals_constraint<' + SerializeTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '!=':
                            res += '\t\t' + name + '_not_equals_constraint<' + SerializeTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '<':
                            res += '\t\t' + name + '_lesser_constraint<' + SerializeTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '>':
                            res += '\t\t' + name + '_greater_constraint<' + SerializeTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '<=':
                            res += '\t\t' + 'compound_constraint<' + SerializeTypename(field.typename) + '> ' + var_name + '(' + \
                                   name + '_lesser_constraint<' + SerializeTypename(field.typename) + '>(' + predicate.value + '), ' + \
                                   name + '_equals_constraint<' + SerializeTypename(field.typename) + '>(' + predicate.value + '));\n'
                        elif predicate.comp_op == '>=':
                            res += '\t\t' + 'compound_constraint<' + SerializeTypename(field.typename) + '> ' + var_name + '(' + \
                                   name + '_greater_constraint<' + SerializeTypename(field.typename) + '>(' + predicate.value + '), ' + \
                                   name + '_equals_constraint<' + SerializeTypename(field.typename) + '>(' + predicate.value + '));\n'
                        constrained_fields[field_number].append(var_name)
                constraint_number += 1
        field_number += 1

    field_number = 0
    res += '\n\t\treturn\n'

    for field in struct.body.items:
        if field_number in constrained_fields:
            res += '\t\t\t'
            for var in constrained_fields[field_number]:
                res += var + '.check(v.' + field.mojom_name + ') && '
            res += 'serialize(v.' + field.mojom_name + ', c) &&\n'
        else:
            res += '\t\t\tserialize(v.' + field.mojom_name + ', c) &&\n'
        field_number += 1
    return res[:-4] + ';\n\t}\n'

def GenerateDeserializeOperator(struct, constraints):
    res = '\tbool operator()(const container &c, ' + struct.mojom_name + ' *v) {\n'
    res += '\t\tif (!v)\n\t\t\treturn false;\n'

    field_number = 0
    constrained_fields = {}
    for field in struct.body.items:
        if field.attribute_list is not None and field.attribute_list.items[0].key in constraints:
            constraint = constraints[field.attribute_list.items[0].key]
            constrained_fields[field_number] = []
            constraint_number = 0
            for constraint_field in constraint:
                predicate = constraint_field.predicate
                var_name = 'c' + str(field_number) + '_' + str(constraint_number)
                if isinstance(predicate, ComparisonPredicate):
                    if predicate.mojom_name in ('size', 'value'):
                        name = predicate.mojom_name
                        if predicate.comp_op == '=':
                            res += '\t\t' + name + '_equals_constraint<' + SerializeTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '!=':
                            res += '\t\t' + name + '_not_equals_constraint<' + SerializeTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '<':
                            res += '\t\t' + name + '_lesser_constraint<' + SerializeTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '>':
                            res += '\t\t' + name + '_greater_constraint<' + SerializeTypename(field.typename) + '> ' + var_name + '(' + predicate.value + ');\n'
                        elif predicate.comp_op == '<=':
                            res += '\t\t' + 'compound_constraint<' + SerializeTypename(field.typename) + '> ' + var_name + '(' + \
                                   name + '_lesser_constraint<' + SerializeTypename(field.typename) + '>(' + predicate.value + '), ' + \
                                   name + '_equals_constraint<' + SerializeTypename(field.typename) + '>(' + predicate.value + '));\n'
                        elif predicate.comp_op == '>=':
                            res += '\t\t' + 'compound_constraint<' + SerializeTypename(field.typename) + '> ' + var_name + '(' + \
                                   name + '_greater_constraint<' + SerializeTypename(field.typename) + '>(' + predicate.value + '), ' + \
                                   name + '_equals_constraint<' + SerializeTypename(field.typename) + '>(' + predicate.value + '));\n'
                        constrained_fields[field_number].append(var_name)
                constraint_number += 1
        field_number += 1

    field_number = 0
    res += '\n\t\treturn\n'

    for field in struct.body.items:
        if field_number in constrained_fields:
            res += '\t\t\tdeserialize(c, &v->' + field.mojom_name + ') && '
            for var in constrained_fields[field_number]:
                res += var + '.check(v->' + field.mojom_name + ') && '
            res += '\n'
        else:
            res += '\t\t\tdeserialize(c, &v->' + field.mojom_name + ') &&\n'
        field_number += 1
    return res[:-4] + ';\n\t}\n'